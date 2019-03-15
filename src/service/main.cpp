#include "aquabox.h"
#include <unistd.h>

int main()
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    static plog::RollingFileAppender<plog::CsvFormatterUtcTime> fileAppender("UtcTime.csv", 10000, 2);    
    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);
    
    aquabox::AquaBox theApp;
    return theApp.start();
}