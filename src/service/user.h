#ifndef USER_H
#define USER_H

#include <string>
#include <nlohmann/json.hpp>

namespace aquabox
{
    class User
    {
    public:
        bool init(const nlohmann::json& data);

        const std::string& user() const;
        const std::string& password() const;
        std::string token() const;

    private:
        std::string m_user;
        std::string m_password;
    };
}

#endif // USER_H