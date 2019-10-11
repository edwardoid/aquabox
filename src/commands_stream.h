#ifndef COMMANDS_STREAM_H
#define COMMANDS_STREAM_H

#include "cloud_stream.h"

namespace aquabox
{
    class CommandsStream: public CloudStream
    {
    public:
        CommandsStream(std::string url);
        virtual ~CommandsStream() {}
    protected:
        virtual void onMessage(std::shared_ptr<class Connection> connection, std::shared_ptr<class InMessage> message) override;
    };
}

#endif // COMMANDS_STREAM