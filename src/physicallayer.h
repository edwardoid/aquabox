#include "layer.h"

namespace aquabox
{
    class Physical: public Layer
    {
    public:
        std::shared_ptr<Bus> bus(std::string bus) override;
    };
}