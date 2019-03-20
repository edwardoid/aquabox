#include "aquabox.h"
#include "config.h"
#include "settings.h"
#include "lockfile.h"
#include "log.h"
#include "restapi.h"
#include "path.h"
#include "pistache/net.h"
#include "pistache/http.h"
#include "pistache/router.h"
#include <exception>
#include <random>

using namespace aquabox;

AquaBox AquaBox::Instance;


AquaBox& AquaBox::i()
{
    return Instance;
}

AquaBox::AquaBox()
    : m_desc(Config::APPLICATION, Config::VERSION, Config::DESCRIPTION)
{
}

int AquaBox::start()
{
    plog::init(plog::verbose, &m_consoleAppender);
    
    Lockfile lock;

    LOG_INFO << "Starting " << Config::APPLICATION << " ver. " << std::hex << Config::VERSION_MAJOR << "." << Config::VERSION_MINOR;
    if(lock.exists())
    {
        LOG_FATAL << "Another instance of " << Config::APPLICATION << " is running...";
        std::exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    std::string procPath = "/proc/";
    procPath += std::to_string(static_cast<int>(getpid()));
    procPath += "/exe";

    char cwd[1024];
    memset(cwd,'\0', 1024);

    int bytes = std::min<size_t>(readlink(procPath.c_str(), cwd, 1024), 1024 - 1);
    if(bytes >= 0)
        cwd[bytes] = '\0';

    

    Path wd = Settings::string(Settings::WORKING_DIRECTORY, cwd);
    if (!wd.normalize())
    {
        LOG_ERROR << "Bad working directory path " << wd;
    }
    else
    {
        if(0 != chdir(wd.c_str()))
        {
            LOG_ERROR << "Can't change working directory to " << wd << " errno: " << errno << " : " << strerror(errno);
        }
        else
        {
            LOG_INFO << "Changed workign direcotry to " << wd;
        }
    }

    Pistache::Address address(Pistache::Ipv4::any(), Settings::integer(Settings::HTTP_PORT, Config::REST_PORT));

    readDevices();
    readUsers();
    readRules(wd);
    initRestApi();
    m_webServer.setRoot(m_desc, "", Settings::string(Settings::WEB_ROOT, cwd), true);

    m_router.initFromDescription(m_desc);

    m_server = std::make_shared<Pistache::Http::Endpoint>(address);
    auto opts = Pistache::Http::Endpoint::options().threads(1).flags(Pistache::Tcp::Options::ReuseAddr | Pistache::Tcp::Options::InstallSignalHandler);
    
    try
    {
        m_server->init(opts);
        m_server->setHandler(m_router.handler());
        LOG_INFO << "Started REST api on " << address.host() << ":" << address.port();
        m_server->serveThreaded();
    }
    catch(std::exception& e)
    {
        LOG_FATAL << "Error occured on REST API startup. Reason: " << e.what();
    }
    catch(...)
    {
        LOG_FATAL << "Unhandled on starting REST API exception...";
    }

    std::getchar();

    return EXIT_SUCCESS;
}

Pistache::Rest::Description AquaBox::initRestApi()
{
    using namespace Pistache;
    using namespace Pistache::Rest;
    using namespace Pistache::Rest::Schema;

    m_desc = Pistache::Rest::Description(Config::APPLICATION, Config::VERSION, Config::DESCRIPTION);
    m_desc.info().contact(Config::APPLICATION, "", Config::EMAIL);
    auto error = m_desc.response(Http::Code::Internal_Server_Error, "An error occured with the backend");

    m_desc.schemes(Scheme::Http)
            .basePath("/v1")
            .produces(MIME(Application, Json))
            .consumes(MIME(Application, Json));

    auto v1 = m_desc.path("/api/v1");

    v1.route(m_desc.post("/login"), "Authenticate")
       .bind(&rest::API::Login, &REST)
       .produces(MIME(Application, Json))
       .response(error);

    v1.route(m_desc.get("/status"), "Get system current state")
       .bind(&rest::API::Status, &REST)
       .produces(MIME(Application, Json))
       .response(error);

    v1.route(m_desc.get("/status/qr"), "Get system current state as QR code")
       .bind(&rest::API::WhoAmI, &REST)
       .response(error);

    v1.route(m_desc.get("/devices"), "Get available devices")
       .bind(&rest::API::Devices, &REST)
       .produces(MIME(Application, Json))
       .response(error);

    v1.route(m_desc.get("/rules"), "Get available rules")
       .bind(&rest::API::Rules, &REST)
       .produces(MIME(Application, Json))
       .response(error);

    v1.route(m_desc.get("/rules/:dev"), "Get available rules for specific device")
       .bind(&rest::API::DeviceRules, &REST)
       .produces(MIME(Application, Json))
       .response(error);

    auto wifi = v1.path("/wifi");
    wifi.route(m_desc.get("/qr"), "Get QR code for easy WiFi authorisation")
       .bind(&rest::API::WiFiQR, &REST)
       .produces(MIME(Image, Png))
       .response(error);

    return m_desc;
}

void AquaBox::readDevices()
{
    auto devs = Settings::array(Settings::DEVICES);
    
    LOG_INFO << "Loading " << devs.size() << " devices...";
    for (auto d : devs)
    {
        auto dev = std::make_shared<Device>();
        if(dev->init(d))
            m_devices.insert({ dev->id(), dev});
    }
}

void AquaBox::readUsers()
{
    auto users = Settings::array(Settings::USERS);
    
    LOG_INFO << "Loading " << users.size() << " users...";
    for (auto u : users)
    {
        auto usr = std::make_shared<User>();
        if(usr->init(u))
            m_users.insert({usr->user(), usr});
    }
}

void AquaBox::readRules(const Path& p)
{
    Path root = p;
    root.append("rules");
    LOG_INFO << "Loading rules from" << root.c_str();
    for(Path p : root.children())
    {
        if(p.isDirectory())
            continue;
        LOG_INFO << "Loading rule: " << p.c_str();
        auto rule = std::make_shared<Rule>();
        if(rule->parseFile(p))
            m_rules.insert({ rule->Id.Value, rule });
    }
}

std::shared_ptr<Device> AquaBox::findDevice(std::string id)
{
    auto p = m_devices.find(id);
    if (p == m_devices.end())
        return nullptr;
    return p->second;
}

std::shared_ptr<Rule> AquaBox::findRule(std::string id)
{
    auto p = m_rules.find(id);
    if (p == m_rules.end())
        return nullptr;
    return p->second;
}

const DevicesMap& AquaBox::devices() const
{
    return m_devices;
}

const RulesMap& AquaBox::rules() const
{
    return m_rules;
}

const UsersMap& AquaBox::users() const
{
    return m_users;
}