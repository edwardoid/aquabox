#include "application.h"
#include "boxes_manager.h"
#include "database.h"

using namespace aquabox;

bool Application::isAuthorized(std::string boxId)
{
    return true;
}

Command& Application::runCommand(std::string boxId, Command& command)
{
    auto box = BoxesManager::get(boxId);
    if (box == nullptr) {
        command.response(Result::NotFound);
        return command;
    }
    return command;
}
