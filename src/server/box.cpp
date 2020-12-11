#include "box.h"
#include "log.h"
#include "application_connection.h"
#include "boxes_manager.h"
#include "database.h"
#include <singleton.h>
#include "telegram_bot.h"

using namespace aquabox;

Box::Box(const std::string& id, ResultsMap<Command>& results)
    : BoxConnection(id, results)
{
    UpdateReceived.connect([this](const nlohmann::json& event){
        if(Singleton<TelegramBot>::available) {
            Singleton<TelegramBot>::i()->updateReceived(*this, event);
        }
    });
}

std::vector<std::shared_ptr<ApplicationConnection>> Box::connections() const
{
    return m_applications;
}

void Box::addConnection(std::shared_ptr<ApplicationConnection> connection)
{
    LOG_INFO << "Addind application connection";
    m_applications.push_back(connection);
}

void Box::removeConnection(std::shared_ptr<ApplicationConnection> connection)
{
    LOG_INFO << "Removing application connection";
    auto p = std::find(m_applications.begin(), m_applications.end(), connection);
    if (p != m_applications.end())
        m_applications.erase(p);
}

std::set<int64_t> Box::telegramChats() const
{
    return Database::i().telegramChats(id());
}

