#ifndef MOCK_GPIO_H
#define MOCK_GPIO_H

namespace aquabox
{
    namespace mock
    {
        struct GPIOInfo {
            std::string value;
            std::string direction;
        };
        class GPIO {
            public:
            GPIO();
        };
    }
}


#endif // MOCK_GPIO_H