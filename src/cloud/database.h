#ifndef DATABASE_H
#define DATABASE_H

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <string>

namespace aquabox
{
    class Database
    {
    public:
        static Database& i();
        bool wasProduced(const std::string id);
    protected:
        Database();
        Database(const Database&) = delete;
        Database(Database&&) = delete;
        Database& operator = (const Database) = delete;
        Database& operator = (Database&&) = delete;

        bool connect();
    private:
        mongocxx::client m_client;
        mongocxx::database m_aquabox;
    };
}

#endif // DATABASE_H