#include "RconClient.h"
#include "Messages/RconRequest.h"
#include "Messages/RconResponse.h"

#define NOMINMAX 1

#include <boost/log/trivial.hpp>
#include <format>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/extensions/permessage_deflate/enabled.hpp>

namespace RustRconClient {

typedef websocketpp::client<websocketpp::config::asio_client> Client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

struct RconClientPrivate {
  Client _websocketClient;
  Client::connection_ptr _websocketConnection;
  websocketpp::lib::shared_ptr<websocketpp::lib::thread> _thread;
  std::atomic_flag _atomicFlag;
  std::uint64_t _requestedIndentifier{0};
  RconResponse _rconResponse;
  std::optional<std::exception> _sendCommandError;

  RconClientPrivate();
  ~RconClientPrivate();

  /// @brief Инициализация вебсокета
  void init();

  /// @brief Подключение к серверу
  void connect(const std::string& host, std::uint16_t port,
               const std::string& password);

  std::string sendCommand(const std::string& command);

 private:
  /// @brief Успешное подключение
  void onOpen(websocketpp::connection_hdl hdl);

  /// @brief Ошибка подключения
  void onFail(websocketpp::connection_hdl hdl);

  /// @brief Соединение разорвано сервером
  void onClose(websocketpp::connection_hdl hdl);

  /// @brief Получено сообщение
  void onMessage(websocketpp::connection_hdl hdl, message_ptr msg);

  // @brief Обработка таймера
  void onTimeout(const boost::system::error_code&);
};

RconClientPrivate::~RconClientPrivate() {
  _websocketClient.stop();
  _thread->join();
}

RconClientPrivate::RconClientPrivate() {
  init();
}

void RconClientPrivate::init() {
  // Настройки клиента websocket
  _websocketClient.clear_access_channels(websocketpp::log::alevel::all);
  _websocketClient.clear_error_channels(websocketpp::log::elevel::all);
  _websocketClient.init_asio();
  _websocketClient.start_perpetual();

  // Устанавливаем обработчик успешного подключения
  _websocketClient.set_open_handler(bind(&RconClientPrivate::onOpen, this,
                                         websocketpp::lib::placeholders::_1));

  // Устанавливаем обработчик ошибки подключения
  _websocketClient.set_fail_handler(bind(&RconClientPrivate::onFail, this,
                                         websocketpp::lib::placeholders::_1));

  // Устанавливаем обработчик закрытия подключения сервером
  _websocketClient.set_close_handler(bind(&RconClientPrivate::onClose, this,
                                          websocketpp::lib::placeholders::_1));

  // Устанавливаем обработчик получения сообщения
  _websocketClient.set_message_handler(websocketpp::lib::bind(
      &RconClientPrivate::onMessage, this, websocketpp::lib::placeholders::_1,
      websocketpp::lib::placeholders::_2));

  // Запускаем главный цикл обработки событий в другом потоке
  _thread.reset(new websocketpp::lib::thread(&Client::run, &_websocketClient));
}

void RconClientPrivate::connect(const std::string& host, std::uint16_t port,
                                const std::string& password) {
  _atomicFlag.clear();
  _websocketClient.get_io_service().dispatch([=, this]() {
    std::string uri = std::format("ws://{}:{}/{}", host, port, password);
    BOOST_LOG_TRIVIAL(debug)
        << std::format("Connecting to {}:{}", host, port) << std::endl;

    // Инициирование нового подключения
    websocketpp::lib::error_code errorCode;
    _websocketConnection = _websocketClient.get_connection(uri, errorCode);
    _websocketClient.connect(_websocketConnection);
  });
  _atomicFlag.wait(false);
  if (_websocketConnection->get_state() != websocketpp::session::state::open) {
    throw std::runtime_error(std::format(
        "Failed to connect: {}", _websocketConnection->get_ec().message()));
  }
}

std::string RconClientPrivate::sendCommand(const std::string& command) {
  _atomicFlag.clear();
  _websocketClient.get_io_service().dispatch([=, this]() {
    RconRequest rconRequest{.Identifier = ++_requestedIndentifier,
                            .Message = command,
                            .Name = "RustRconClient"};
    _websocketClient.send(_websocketConnection,
                          nlohmann::json(rconRequest).dump(),
                          websocketpp::frame::opcode::text);
  });
  _atomicFlag.wait(false);
  return _rconResponse.Message;
}

void RconClientPrivate::onOpen(websocketpp::connection_hdl hdl) {
  BOOST_LOG_TRIVIAL(debug) << "onOpen" << std::endl;
  _atomicFlag.test_and_set();
  _atomicFlag.notify_one();
}

void RconClientPrivate::onFail(websocketpp::connection_hdl hdl) {
  BOOST_LOG_TRIVIAL(debug) << "onFail" << std::endl;
  _atomicFlag.test_and_set();
  _atomicFlag.notify_one();
}

void RconClientPrivate::onClose(websocketpp::connection_hdl hdl) {
  BOOST_LOG_TRIVIAL(debug) << "onClose" << std::endl;
  _atomicFlag.test_and_set();
  _atomicFlag.notify_one();
}

void RconClientPrivate::onMessage(websocketpp::connection_hdl hdl,
                                  message_ptr msg) {
  BOOST_LOG_TRIVIAL(debug) << "onMessage" << msg->get_payload() << std::endl;
  const auto json = nlohmann::json::parse(msg->get_payload());
  if (!json.is_object()) {
    BOOST_LOG_TRIVIAL(warning) << "Invalid response. Wait next...";
    return;
  }

  RconResponse rconResponse = json.template get<RconResponse>();
  if (rconResponse.Identifier != _requestedIndentifier) {
    BOOST_LOG_TRIVIAL(warning)
        << std::format("Invalid Identifier ({}) of response. Wait next...",
                       _requestedIndentifier);
    return;
  }

  _rconResponse = rconResponse;
  _atomicFlag.test_and_set();
  _atomicFlag.notify_one();
}

void RconClientPrivate::onTimeout(const boost::system::error_code&) {}

RconClient::RconClient() : _pImpl(std::make_unique<RconClientPrivate>()) {}

RconClient::~RconClient() {};

void RconClient::connect(const std::string& host, std::uint16_t port,
                         const std::string& password) {
  _pImpl->connect(host, port, password);
}

std::string RconClient::sendCommand(const std::string& command) {
  return _pImpl->sendCommand(command);
}

}  // namespace RustRconClient