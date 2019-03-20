#include "time.h"

using namespace aquabox;

Time::Time(std::string name, bool required)
    : UnsignedNumberProperty(name.c_str(), required)
{}