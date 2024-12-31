cmake_minimum_required(VERSION 3.15)
project(rust_rcon_client CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(nlohmann_json REQUIRED)
find_package(websocketpp REQUIRED)
find_package(Boost REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/Services/SettingsReader.cpp
                               src/Services/RconClient.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE websocketpp::websocketpp nlohmann_json::nlohmann_json
                          Boost::log Boost::program_options)

install(
  TARGETS ${PROJECT_NAME}
  DESTINATION "."
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
