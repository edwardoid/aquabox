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

#include "rule_parser.h"
#include "rule.h"
#include "path.h"

using namespace aquabox;

std::unordered_map<std::string, std::shared_ptr<Rule>> RulesParser::parse(Path& root)
{
    std::unordered_map<std::string, std::shared_ptr<Rule>> rules;
    LOG_INFO << "Loading rules from" << root.c_str();
    for(Path p : root.children())
    {
        if(p.isDirectory() || p.extension() != "json")
            continue;
        LOG_INFO << "Loading rule: " << p.c_str();
        auto rule = std::make_shared<Rule>();
        if(rule->parseFile(p)) {
            rules.insert({ rule->getId(), rule });
        }
    }
    return rules;
}
