#pragma once

#include "Profile.h"

#include <map>

namespace RustRconClient {

struct Settings {
  std::map<std::string, Profile> profiles;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, profiles)

}  // namespace RustRconClient