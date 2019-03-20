#include "web.h"
#include "log.h"

using namespace aquabox;

struct RoutingInfo
{
    Path realPath;
    std::string alias;
};

void ServeDir::setRoot(Pistache::Rest::Description& description, std::string webRoot, Path root, bool recursive)
{
    m_root = root;
    m_webRoot = webRoot;


    LOG_ERROR_IF(!m_root.isDirectory()) << m_root.c_str() << " is not a directory and can't be hosted";
    LOG_ERROR_IF(!m_root.normalize()) << "Can't get full path of serving root: " << m_root;

    RoutingInfo first;
    first.realPath = root;
    first.alias = webRoot;

    std::list<RoutingInfo> routes({ first });

    description.route(description.get(webRoot + "/"), webRoot)
               .bind(&ServeDir::operator(), this);

    while(!routes.empty())
    {
        auto front = routes.front();
        routes.pop_front();
        if (!front.realPath.isDirectory())
            continue;

        LOG_VERBOSE << "Serving " << front.realPath.c_str() << " as " << front.alias + "/:file";
        description.route(description.get(front.alias + "/:file"), front.realPath.c_str())
                   .bind(&ServeDir::operator(), this);
        
        for(auto& c : front.realPath.children())
        {
            if (!c.isDirectory())
                continue;

            RoutingInfo info;
            info.realPath = c;
            info.alias = front.alias + "/" + c.lastToken();
            routes.push_back(info);
        }
    }
}


void ServeDir::operator()(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
    auto file = request.resource();

    file = file.substr(m_webRoot.size());

    if (file.empty() || file == "/")
        file = "index.html";

    Path p = m_root;
    p.append(file);

    if (!p.normalize())
    {
        LOG_ERROR << "Can't normalize requested file path " << p;
    }
    else
    {
        LOG_VERBOSE << "Requested file = " << file;
    }

    if (!p.contains(m_root))
    {
        LOG_ERROR << "Not contains";
        response.send(Pistache::Http::Code::Forbidden);
        return;
    }

    Pistache::Http::serveFile(response, p);
}