#pragma once

#include <memory>
#include <string>

namespace RustRconClient {

struct RconClientPrivate;

class RconClient {
 public:
  RconClient();
  virtual ~RconClient();

  /// @brief Блокирующий вызов подключения к серверу, в случае ошибки испускает std::runtime_error
  void connect(const std::string& host, std::uint16_t port,
               const std::string& password);

  /// @brief Блокирующая отправка команды
  std::string sendCommand(const std::string& command);

 private:
  std::unique_ptr<RconClientPrivate> _pImpl;
};

}  // namespace RustRconClient