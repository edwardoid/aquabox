#ifndef WEB_H
#define WEB_H

#include "path.h"
#include "pistache/http.h"
#include "pistache/router.h"
#include "pistache/description.h"

namespace aquabox
{
    struct ServeDir
    {
    public:
        void setRoot(Pistache::Rest::Description& description, std::string webRoot, Path root, bool recursive = false);
        void operator()(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    private:
        std::string m_webRoot;
        Path m_root;
    };
}

#endif // WEB_H