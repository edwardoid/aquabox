#include "commands_stream.h"
#include "log.h"
#include <nlohmann/json.hpp>

using namespace aquabox;

CommandsStream::CommandsStream(std::string url)
    : CloudStream(url)
{}

void CommandsStream::onMessage(std::shared_ptr<class Connection> connection, std::shared_ptr<class InMessage> message)
{
    try {
        connection->send(message->string());
    } catch (nlohmann::json::parse_error err) {
        LOG_ERROR << "Can't parse command: " << err.what();
    }
}