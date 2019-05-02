#ifndef STATUS_IMAGE_H
#define STATUS_IMAGE_H

#include <stdint.h>
#include <string>

namespace aquabox
{
    class StatusImage
    {
    public:
        static void display(std::string fbDevice = "/dev/fb0");
    };
}

#endif // STATUS_IMAGE_H