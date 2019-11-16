#ifndef VALIDATION_ERROR_HANDLER_H
#define VALIDATION_ERROR_HANDLER_H

#include <json-schema.hpp>
namespace aquabox
{  
    class ValidationErrorHandler : public nlohmann::json_schema::basic_error_handler
	{
    public:
		void error(const nlohmann::json_pointer<nlohmann::basic_json<>> &pointer, const nlohmann::json &instance, const std::string &message) override;
        const std::string& error() const;
    private:
        std::string m_error;
	};
}

#endif // VALIDATION_ERROR_HANDLER_H