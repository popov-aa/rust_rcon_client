#include "SettingsReader.h"

namespace RustRconClient {

SettingsReader::SettingsReader() {}

const Settings& SettingsReader::settings() const {
  return _settings;
}

}  // namespace RustRconClient