#include "box_connection.h"
#include <nlohmann/json.hpp>
#include "enumerations.h"
#include "log.h"

using namespace aquabox;

BoxConnection::BoxConnection(ResultsMap<Command>& resutsMap)
    : results(resutsMap)
{
}

void BoxConnection::ready(std::string body)
{
    Command cmd;
    cmd.response(Result::ServerError);
    try 
    {
        nlohmann::json data = nlohmann::json::parse(body);
        cmd.fromJSON(data);
    }
    catch(nlohmann::json::parse_error ex ) {

    }
    catch(...) {}
    results.ready(cmd.seqNum(), cmd);
}

void BoxConnection::runCommand(Command& cmd)
{
    if (commands == nullptr) {
        cmd.response(Result::AccessDenied);
        return;
    }
    nlohmann::json cmdJson = cmd.toJSON();

    LOG_DEBUG << "Executing command\n" << cmd.toJSON();

    commands->send(cmdJson.dump());

    if(!results.waitFor(cmd.seqNum(), cmd)) {
        cmd.response(Result::AccessDenied);
    }
}