#define DO_COFIGURE 1

#define VER_MAJ 1
#define VER_MIN 1
#define VER_FULL "1.1"

#include "config.h"

using namespace aquabox;

const std::string Config::APPLICATION = "GoAqua";
const std::string Config::DESCRIPTION = "Some kind of smart aquarium controller";
const std::string Config::VERSION = VER_FULL;
const uint16_t Config::VERSION_MAJOR = VER_MAJ;
const uint16_t Config::VERSION_MINOR = VER_MIN;
const std::string Config::LOCK_FILE = "/tmp/" + Config::APPLICATION + ".lock";
const std::string Config::AUTHOR = "Edward Sargsyan";
const std::string Config::EMAIL = "edward.sarkisyan@gmail.com";
const std::string Config::ConfigurationFile = "config.json";
int Config::REST_PORT = 1213;