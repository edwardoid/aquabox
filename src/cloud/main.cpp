#include "cloud_service.h"
#include "settings.h"

int main(int argc, char** argv)
{
    aquabox::Settings::setDomain("cloud");
    if (aquabox::CloudService::i().start())
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}
