#include "SettingsContainer.h"

#include <boost/filesystem/operations.hpp>
#include <boost/log/trivial.hpp>
#include <fstream>

namespace RustRconClient {

struct SettingsContainerPrivate {

  Settings _settings;

  SettingsContainerPrivate();

  bool read();
  void write();
  void readOrCreateDefault();

 private:
  boost::filesystem::path _settingsDirpath;
  boost::filesystem::path _settingsFilepath;

  void checkSettingsDirectory();
};

SettingsContainerPrivate::SettingsContainerPrivate() {
  //FIXME Find more crossplatform variant
  const boost::filesystem::path homePath(
#ifdef WIN32
      std::format("{}{}", getenv("HOMEDRIVE"), getenv("HOMEPATH"))
#else
      std::string(getenv("HOME"))
#endif
  );
  _settingsDirpath = boost::filesystem::path(
      std::format("{}/.config/rust_rcon_client", homePath.string()));
  _settingsFilepath = boost::filesystem::path(
      std::format("{}/settings.json", _settingsDirpath.string()));
}

bool SettingsContainerPrivate::read() {
  std::ifstream stream;
  stream.open(_settingsFilepath.native(), std::ifstream::in);

  if (!stream.is_open()) {
    return false;
  }

  std::stringstream buffer;
  buffer << stream.rdbuf();

  const auto json = nlohmann::json::parse(buffer.str());
  if (!json.is_object()) {
    throw std::runtime_error(
        "Failed to parse settings: it is not json object.");
  }

  _settings = json.template get<Settings>();
  return true;
}

void SettingsContainerPrivate::write() {
  checkSettingsDirectory();

  std::ofstream stream(_settingsFilepath.native());
  stream << nlohmann::json(_settings).dump();
}

void SettingsContainerPrivate::readOrCreateDefault() {
  if (!read()) {
    _settings.profiles.insert({"default", Profile()});
    write();
  }
}

void SettingsContainerPrivate::checkSettingsDirectory() {
  boost::system::error_code error;
  if (!boost::filesystem::exists(_settingsDirpath)) {
    boost::filesystem::create_directories(_settingsDirpath, error);
  }
}

SettingsContainer::SettingsContainer()
    : _pImpl(std::make_unique<SettingsContainerPrivate>()) {}

SettingsContainer::~SettingsContainer() {}

bool SettingsContainer::read() {
  return _pImpl->read();
}

void SettingsContainer::write() {
  _pImpl->write();
}

void SettingsContainer::readOrCreateDefault() {
  _pImpl->readOrCreateDefault();
}

const Settings& SettingsContainer::settings() const {
  return _pImpl->_settings;
}

}  // namespace RustRconClient