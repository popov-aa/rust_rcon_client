cmake_minimum_required(VERSION 3.15)
project(
  rust_rcon_client
  LANGUAGES CXX
  VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

find_package(nlohmann_json REQUIRED)
find_package(websocketpp REQUIRED)
find_package(Boost REQUIRED)

configure_file(src/Config.h.in Config.h)

add_executable(${PROJECT_NAME} src/main.cpp src/Services/SettingsContainer.cpp
                               src/Services/RconClient.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}" src)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE websocketpp::websocketpp nlohmann_json::nlohmann_json
                          Boost::log Boost::program_options)

install(TARGETS ${PROJECT_NAME} DESTINATION ".")

set(CPACK_GENERATOR "ZIP")
include(CPack)
