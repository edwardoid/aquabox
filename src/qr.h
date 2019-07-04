#ifndef QR_CODE
#define QR_CODE

#include <string>

namespace qr
{
    class Qr
    {
    public:
        static bool create(std::string data, std::string pngPath, double zoomFactor = 1.);
    };
}

#endif // QR_CODE