#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace RustRconClient {

struct RconResponse {
  std::uint64_t Identifier;
  std::string Message;
  std::string Type;
  std::string Stacktrace;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RconResponse, Identifier, Message, Type,
                                   Stacktrace)

}  // namespace RustRconClient