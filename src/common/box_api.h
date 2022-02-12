#pragma once

#include <common/command.h>
#include <functional>
#include <memory>
#include <string>

namespace aquabox
{
    class Event;
    class CommandHandler
    {
    public:
        virtual void runCommand(Command cmd, std::function<void(const Command&, bool)>&& onResult) = 0;
    };

    class RemoteCommandHandler : public CommandHandler
    {
    public:
        virtual bool isConnected() const = 0;
        virtual bool connect() const = 0;
        virtual bool send(const Command&) = 0;
    };

    class RemoteAquabox : public RemoteCommandHandler
    {
    public:
        virtual const std::string& id() const = 0;
        virtual void onEvent(std::shared_ptr<Event> event) = 0;
    };
} // namespace aquabox
