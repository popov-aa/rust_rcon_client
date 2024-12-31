#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace RustRconClient {

struct RconRequest {
  std::uint64_t Identifier;
  std::string Message;
  std::string Name;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RconRequest, Identifier, Message, Name)

}  // namespace RustRconClient