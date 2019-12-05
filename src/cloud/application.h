#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
namespace aquabox
{
    class Application
    {
    public:
        std::string id() const;
        std::string name() const;
        std::string author() const;
        bool isAuthorizedOn(std::string boxId);
        bool authorizeOn(std::string boxId);
        bool deauthodirzeOn(std::string boxId);
        std::string create(std::string name, std::string author);
        void deleteApp();
        std::string session(std::string box);
        void createSession(std::string box);
        void removeSession(std::string box);
    };
}

#endif // APPLICATION_H