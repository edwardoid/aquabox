#include "time.h"

using namespace aquabox;

Time::Time(std::string name, bool required)
    : UnsignedNumberProperty(name.c_str(), required)
{}


std::tm Time::getDetails() const
{
    std::time_t moment = static_cast<time_t>(Value);
    const std::tm* details = std::gmtime(&moment);
    if (details == nullptr) {
        LOG_ERROR << "Can't get date details from " << Value;
    }
    return *details;
}