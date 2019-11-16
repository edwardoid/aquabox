#include "command_schema.h"

using namespace aquabox;
using namespace nlohmann;

json_schema::json_validator& CommandValidator::validator()
{
    static const json schema = R"(
        {
            "definitions": {},
            "$schema": "http://json-schema.org/draft-07/schema#",
            "$id": "http://example.com/root.json",
            "type": "object",
            "title": "The Root Schema",
            "required": [
                "seqNum",
                "operation",
                "data",
                "params",
                "result"
            ],
            "properties": {
                "seqNum": {
                "$id": "#/properties/seqNum",
                "type": "integer",
                "title": "The Seqnum Schema",
                "default": 0,
                "examples": [
                    0
                ]
                },
                "operation": {
                "$id": "#/properties/operation",
                "type": "string",
                "title": "The Operation Schema",
                "default": "",
                "examples": [
                    "get_device"
                ],
                "pattern": "^(.*)$"
                },
                "data": {
                "$id": "#/properties/data",
                "type": [
                    "object",
                    "string"
                ],
                "title": "In/Out data",
                "default": "",
                "examples": [
                    ""
                ],
                "pattern": "^(.*)$"
                },
                "params": {
                "$id": "#/properties/params",
                "type": "object",
                "title": "The Params Schema"
                },
                "result": {
                "$id": "#/properties/result",
                "type": "string",
                "title": "The Result Schema",
                "default": "",
                "examples": [
                    "OK"
                ],
                "pattern": "^(.*)$"
                }
            }
        }
    )"_json;

    static json_schema::json_validator validator(schema);
    return validator;
}