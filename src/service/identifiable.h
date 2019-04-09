#ifndef IDENTIFIABLE_H
#define IDENTIFIABLE_H

#include <string>

namespace aquabox
{
    class Identifiable
    {
    public:
        Identifiable() = default;
        ~Identifiable() = default;
        Identifiable(const Identifiable&) = default;
        Identifiable(Identifiable&&) = default;

        const std::string& id() const;
        void setId(const std::string& newId);
    private:
        std::string m_id;
    };
}

#endif // IDENTIFIABLE_H