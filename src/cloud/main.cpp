#include "cloud_service.h"
#include "settings.h"
#include "backward.hpp"

int main(int argc, char** argv)
{
    backward::SignalHandling sh;
    aquabox::Settings::setDomain("cloud");
    if (aquabox::CloudService::i().start())
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}
