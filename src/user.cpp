#include "user.h"
#include "log.h"
#include "crypto.h"

using namespace aquabox;

bool User::init(const nlohmann::json& data)
{
    auto pos = data.find("user");
    if (pos == data.end()) {
        LOG_ERROR << "Can't find user name in JSON";
        return false;
    }
    if (!pos->is_string()) {
        LOG_ERROR << "User name is not string";
        return false;
    }

    m_user = pos->get<std::string>();

    pos = data.find("password");
    if (pos == data.end()) {
        LOG_ERROR << "Can't find password in JSON";
        return false;
    }
    if (!pos->is_string()) {
        LOG_ERROR << "Password is not string";
        return false;
    }

    m_password = pos->get<std::string>();
    return true;
}

const std::string& User::user() const
{
    return m_user;
}
        
const std::string& User::password() const
{
    return m_password;
}
        
std::string User::token() const
{
    const std::string raw = m_user + ":" + m_password;
    return SHA512::hash(raw);
}