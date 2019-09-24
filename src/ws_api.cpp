#include "ws_api.h"
#include "event.h"
#include "enumerations.h"
#include "command.h"
#include "command_executor.h"

using namespace aquabox::ws;

void API::onOpen(const std::string& route, Connection connection)
{
    std::lock_guard<std::mutex> guard(m_lock);

    auto connections = m_connections.find(route);
    if (connections == m_connections.end()) {
        m_connections.insert({ route, std::vector<Connection>({ connection })});
    } else  {
        connections->second.push_back(connection);
    }
}
void API::onClose(const std::string& route, Connection connection, Status status, const std::string& reason) 
{
    std::lock_guard<std::mutex> guard(m_lock);

    auto connections = m_connections.find(route);
    if (connections == m_connections.end()) {
        return;
    }

    for(auto i = connections->second.begin(); i != connections->second.end(); ++i)
    {
        if(i->get() == connection.get())
        {
            connections->second.erase(i);
            break;
        }
    }
}

void API::postEvent(Event event)
{
    std::lock_guard<std::mutex> guard(m_lock);
    auto data = event->serialize().dump(4);

    auto connections = m_connections.find(UpdatesUrl);
    if (connections == m_connections.end()) {
        return;
    }

    for(auto& connection : connections->second)
    {
        connection->send(data);
    }
}

DEFINE_WS_API_IMPL(Updates)
{
    connection->send("Updates");
}

DEFINE_WS_API_IMPL(Commands)
{
    nlohmann::json request;
    try
    {
        request = nlohmann::json::parse(response->string());      
    }
    catch(nlohmann::json::parse_error& e)
    {
        LOG_ERROR << "Can't parse command: " << e.what();
        return;
    }

    auto rqid = request.find("rqid");
    if (rqid == request.end()) {
        LOG_ERROR << "No request ID in command";
        return;
    }

    if (!rqid->is_number_unsigned()) {
        LOG_ERROR << "Request id is not a number";
        return;
    }

    auto cmd = request.find("command");
    if (cmd == request.end()) {
        LOG_ERROR << "Request does not contain command";
        return;
    }

    if (!cmd->is_string()) {
        LOG_ERROR << "Command is not a string";
        return;
    }

    Operation op = Serializable<Operation>::convert(cmd->get<std::string>());
    if (Operation::Unknown == op) {
        LOG_ERROR << "Unknown command";
        return;
    }


    auto body = request.find("data");

    Command command;
    command.setOperation(op);

    if (body != request.end()) {
        command.setBody(body.value().dump());
    }

    auto params = request.find("params");
    if (params != request.end() && params->is_object())
    {
        for(auto& x : nlohmann::json::iterator_wrapper(params.value())) {
            command.setParameter(x.key(), x.value());
        }
    }
    
    
    CommandExecutor exec;
    if(!exec.exec(command)) {
        LOG_ERROR << "Can't execute command! " << (int)(command.operation());
    } else {
        LOG_DEBUG << "Command " << (int)(command.operation()) << " finished with " << (int)(command.result()) << " data: " << command.response();
    }

    nlohmann::json resp;
    resp["rpid"] = rqid->get<unsigned int>();
    resp["result"] = (int)command.result();

    try
    {
        nlohmann::json resultData = nlohmann::json::parse(command.response());
        resp["data"] = resultData;
    } catch(...) {}
    

    connection->send(resp.dump(4));
}