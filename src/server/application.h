#ifndef APPLICATION_H
#define APPLICATION_H

#include "command.h"

namespace aquabox
{
    class Application
    {
    public:
        bool isAuthorized(std::string box);
        Command& runCommand(std::string box, Command& cmd);
    };
}

#endif // APPLICATION_H
