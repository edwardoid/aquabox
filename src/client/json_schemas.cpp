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

#include "json_schemas.h"

#include <nlohmann/json-schema.hpp>

using namespace aquabox;
using nlohmann::json;
using nlohmann::json_schema::json_validator;

static const auto COMMAND_SCHEMA = R"(
{
  "definitions": {},
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "http://example.com/root.json",
  "type": "object",
  "title": "The Root Schema",
  "required": [
    "seqNum",
    "params",
    "operation"
  ],
  "properties": {
    "seqNum": {
      "$id": "#/properties/seqNum",
      "type": "integer",
      "title": "The Seqnum Schema",
      "default": 0,
      "examples": [
        123
      ]
    },
    "params": {
      "$id": "#/properties/params",
      "type": "object",
      "title": "The Params Schema"
    },
    "operation": {
      "$id": "#/properties/operation",
      "type": "string",
      "title": "The Operation Schema",
      "default": "get_status",
      "examples": [
        "get_device"
      ],
      "pattern": "get_status|get_all_devices|get_device|update_device|get_rules|get_rule|create_rule|update_rule|delete_rule|get_device_rules|scan|connect|turn_on|turn_off"
    }
  }
}
)"_json;

bool JsonEntity::isValidCommand(const nlohmann::json& obj)
{
    static json_validator validator(COMMAND_SCHEMA);

    try {
        validator.validate(obj);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
    return false;
}