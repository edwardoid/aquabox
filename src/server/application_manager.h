#ifndef APPLICATION_MANAGER_H
#define APPLICATION_MANAGER_H

namespace aquabox
{
    class Application;

    class ApplicationManager
    {
    public:
        static Application* getAppication(std::string id);
    };
}

#endif // APPLICATION_MANAGER_H