#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace RustRconClient {

struct Profile {
  std::string host;
  std::uint16_t port;
  std::string password;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Profile, host, port, password)

}  // namespace RustRconClient