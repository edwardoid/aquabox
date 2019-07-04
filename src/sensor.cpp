#include "sensor.h"
#include "layer.h"

using namespace aquabox;


double Sensor::value() const
{
    auto val = read(64);

    if (val.empty()) {
        return 0.0;
    }

    double floatVal = (double) atoi(reinterpret_cast<const char*>(val.data()));
    return floatVal / 1000.;
}