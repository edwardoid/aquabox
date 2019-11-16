#include "validation_error_handler.h"
#include <sstream>

using namespace aquabox;

void ValidationErrorHandler::error(const nlohmann::json_pointer<nlohmann::basic_json<>> &pointer, const nlohmann::json &instance, const std::string &message)
{
    std::stringstream ss;
    nlohmann::json_schema::basic_error_handler::error(pointer, instance, message);
    ss << "ERROR: '" << pointer << "' - '" << instance << "': " << message;
    m_error = ss.str();
}

const std::string& ValidationErrorHandler::error() const
{
    return m_error;
}