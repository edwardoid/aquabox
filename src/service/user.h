#ifndef USER_H
#define USER_H

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <memory>

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

    using UsersMap = std::unordered_map<std::string, std::shared_ptr<User>>;
}

#endif // USER_H