#include "Config.h"
#include "Services/RconClient.h"
#include "Services/SettingsContainer.h"

#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace RustRconClient;

void printHelp(const char* applicationFilepath,
               const boost::program_options::options_description& options) {
  std::cout
      << std::endl
      << std::format(
             R"(
Usage:
  {0} --help
  {0} --host <host> --port <port> --password <password> --command <command>
  {0} --profile <profile> --command <command>
)",
             boost::filesystem::path(applicationFilepath).filename().string())
      << std::endl
      << options << std::endl;
}

int main(int argc, char** argv) {

  boost::program_options::options_description options("Allowed options");
  options.add_options()                                                       //
      ("help", "Produce help message")                                        //
      ("version", "Version")                                                  //
      ("verbose", "Verbose mode")                                             //
      ("host", boost::program_options::value<std::string>(), "Host")          //
      ("port", boost::program_options::value<std::string>(), "Port")          //
      ("password", boost::program_options::value<std::string>(), "Password")  //
      ("profile", boost::program_options::value<std::string>(), "Profile")    //
      ("command", boost::program_options::value<std::string>(), "Command")    //
      ;

  boost::program_options::variables_map variablesMap;
  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, options),
      variablesMap);
  boost::program_options::notify(variablesMap);

  if (variablesMap.size() == 0 || variablesMap.count("help")) {
    printHelp(argv[0], options);
    return 1;
  }

  if (variablesMap.count("version")) {
    std::cout << "Version: " << VERSION << std::endl;
    return 1;
  }

  if (!variablesMap.count("verbose")) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                        boost::log::trivial::info);
  }

  RconClient rconClient;
  SettingsContainer settingsContainer;
  std::string host, password;
  std::uint16_t port;
  std::string command;

  try {
    settingsContainer.readOrCreateDefault();

    if (!variablesMap.count("command")) {
      printHelp(argv[0], options);
      return 1;
    }
    command = variablesMap["command"].as<std::string>();

    if (variablesMap.count("profile")) {
      const auto profileName = variablesMap["profile"].as<std::string>();
      if (!settingsContainer.settings().profiles.contains(profileName)) {
        std::cerr << std::format("Profile \"{}\" does not exists.",
                                 profileName);
        return 1;
      }
      const Profile profile =
          settingsContainer.settings().profiles.at(profileName);
      host = profile.host;
      port = profile.port;
      password = profile.password;
    } else if (variablesMap.count("host") == 1 &&
               variablesMap.count("port") == 1 &&
               variablesMap.count("password") == 1) {
      host = variablesMap["host"].as<std::string>();
      port = std::atoi(variablesMap["port"].as<std::string>().c_str());
      password = variablesMap["password"].as<std::string>();
    } else {
      printHelp(argv[0], options);
      return 1;
    }

    rconClient.connect(host, port, password);
    std::cout << rconClient.sendCommand(command);
  } catch (const std::exception& ex) {
    std::cerr << ex.what();
    return 2;
  }

  return 0;
}
