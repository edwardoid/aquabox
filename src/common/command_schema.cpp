/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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