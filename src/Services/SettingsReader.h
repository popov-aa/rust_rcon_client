#pragma once

#include "Settings/Settings.h"

namespace RustRconClient {

class SettingsReader {

 public:
  SettingsReader();

  const Settings& settings() const;

 private:
  Settings _settings;
};

}  // namespace RustRconClient