#define DO_COFIGURE 1

#include "config.h"

#define _S(X) #X
#define _C(X, Y)

using namespace aquabox;

const std::string Config::GIT_HASH = AB_GIT_HASH ;
const std::string Config::APPLICATION = "GoAqua";
const std::string Config::DESCRIPTION = "Some kind of smart aquarium controller";
const std::string Config::VERSION = AB_VERSION;
const uint16_t Config::VERSION_MAJOR = AB_VERSION_MAJOR;
const uint16_t Config::VERSION_MINOR = AB_VERSION_MINOR;
const std::string Config::LOCK_FILE = "/tmp/" + Config::APPLICATION + ".lock";
const std::string Config::AUTHOR = "Edward Sargsyan";
const std::string Config::EMAIL = "edward.sarkisyan@gmail.com";
const std::string Config::ConfigurationFile = "config.json";
int Config::REST_PORT = 1213;
int Config::WS_PORT = 1214;