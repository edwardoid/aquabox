
#include <common/command.h>

namespace aquabox
{
    class CommandsHandler
    {
    public:
        bool connected() const = 0;
        bool connect() = 0;
        bool exec(Command& cmd) = 0;
        void reply(const Command& cmd) = 0;
    };
} // namespace aquabox
