#include "box_connection.h"
#include <nlohmann/json.hpp>
#include "enumerations.h"
#include "log.h"

using namespace aquabox;

BoxConnection::BoxConnection(const std::string& id, ResultsMap<Command>& resutsMap)
    : results(resutsMap)
    , m_id(id)
{
}

const std::string& BoxConnection::id() const
{
    return m_id;
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
    nlohmann::json response = cmd.responseJson();

    LOG_DEBUG << "Executing command\n" << response.dump();

    commands->send(response.dump());

    if(!results.waitFor(cmd.seqNum(), cmd)) {
        cmd.response(Result::AccessDenied);
    }
}

bool BoxConnection::empty() const
{
    return events == nullptr || commands == nullptr;
}
