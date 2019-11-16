#ifndef CLOUD_SERVICE_H
#define CLOUD_SERVICE_H

#include "log.h"

namespace aquabox
{
    class CloudService
    {
    public:
        static CloudService& i();

        bool start();
    private:
        CloudService();
    private:
        static CloudService m_instance;
        plog::ColorConsoleAppender<plog::TxtFormatter> m_consoleAppender;
        plog::RollingFileAppender<plog::CsvFormatter> m_logAppender;
        plog::RollingFileAppender<plog::TxtFormatter> m_fileAppender;
    };
}

#endif // CLOUD_SERVICE