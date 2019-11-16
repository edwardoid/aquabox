#include "cloud_service.h"

int main(int argc, char** argv)
{
    if (aquabox::CloudService::i().start())
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}
