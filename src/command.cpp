#include "command.h"

using namespace aquabox;

void Command::setBody(std::string body)
{
    m_body = body;
}

const std::string& Command::body() const
{
    return m_body;
}

void Command::setParameters(const std::unordered_map<std::string, std::string>& params)
{
    m_params = params;
}

void Command::setParameter(std::string name, std::string value)
{
    m_params.insert({ name, value });
}

bool Command::hasParameter(std::string name) const
{
    auto pos = m_params.find(name);
    return pos != m_params.end() && !pos->second.empty();
}

std::string Command::getParameter(std::string name) const
{
    auto p = m_params.find(name);
    if (p == m_params.end()) {
        return "";
    }
    return p->second;
}

void Command::setOperation(Operation op)
{
    m_operation = op;
}

Operation Command::operation() const
{
    return m_operation;
}

void Command::response(const nlohmann::json data, Result result)
{
    m_data = data.dump(4);
    m_result = result;
}

void Command::response(Result result)
{
    static const std::string NoData("");
    response(NoData, result);
}

const std::string& Command::response() const
{
    return m_data;
}

Result Command::result() const
{
    return m_result;
}
