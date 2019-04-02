#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace aquabox
{
    struct Config
    {
        static const std::string APPLICATION;
        static const std::string DESCRIPTION;
        static const std::string VERSION;
        static const std::string GIT_HASH;
        static const uint16_t VERSION_MAJOR;
        static const uint16_t VERSION_MINOR;
        static const std::string LOCK_FILE;
        static const std::string AUTHOR;
        static const std::string EMAIL;
        static const std::string ConfigurationFile;

        static int REST_PORT;
        static int WS_PORT;
    };
}

#endif // CONFIG_H