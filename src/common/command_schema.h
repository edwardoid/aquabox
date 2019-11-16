#ifndef COMMAND_VALIDATOR_H
#define COMMAND_VALIDATOR_H

#include <json-schema.hpp>

namespace aquabox
{
    class CommandValidator
    {
    public:
        static nlohmann::json_schema::json_validator& validator(); 
    };
}

#endif // COMMAND_VALIDATOR_H
