#include "identifiable.h"

using namespace aquabox;

const std::string& Identifiable::id() const
{
    return m_id;
}

void Identifiable::setId(const std::string& id)
{
    m_id = id;
}