#include "cloud_service.h"
#include "applications_api.h"
#include "boxes_manager.h"
#include "database.h"

#define LOG_FILE_SIZE_IN_MB 10
#define BYTES_IN_MB 1000000
#define MAX_LOG_FILES 50

using namespace aquabox;

CloudService CloudService::m_instance;

CloudService::CloudService()
    : m_fileAppender("./aquabox.log", LOG_FILE_SIZE_IN_MB * BYTES_IN_MB, MAX_LOG_FILES)
    , m_logAppender("./aquabox.csv", LOG_FILE_SIZE_IN_MB * BYTES_IN_MB, 1)
{

}

CloudService& CloudService::i()
{
    return m_instance;
}

bool CloudService::start()
{
    plog::init(plog::Severity::debug, &m_consoleAppender);
    plog::init( plog::Severity::debug, &m_logAppender);

    LOG_INFO << "Starting Aquabox Cloud Service...";
    ApplicationsApi::start();
    return BoxesManager::start();
}
