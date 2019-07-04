#include "time.h"

using namespace aquabox;

Time::Time(std::string name, bool required)
    : UnsignedNumberProperty(name.c_str(), required)
{}


TimeDetails Time::getDetails() const
{
    std::time_t moment = static_cast<time_t>(Value / 1000);
    return TimeDetails(moment);
}