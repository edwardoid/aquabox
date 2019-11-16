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

#include "rule.h"
#include "log.h"
#include <fstream>

using namespace aquabox;

Rule::Rule()
    : ClassProperty("-class Rule", {&m_Id, &m_Name, &m_Index, &m_Device, &m_Enabled,&m_CreatedAt, &m_LastRunAt, &m_Repeat, &m_Actions} , false)
    , m_Id("id")
    , m_Name("name")
    , m_Index("index", false)
    , m_Device("device")
    , m_Enabled("enabled")
    , m_CreatedAt("created_at")
    , m_LastRunAt("lastRun", false)
    , m_Repeat("repeat")
    , m_Actions("actions")
{}

Rule::Rule(const Rule& src)
    : Rule()
{
    *this = src;
}

Rule::Rule(Rule&& src)
    : Rule()
{
    moveData(src);
    moveProperty(Id);
    moveProperty(Name);
    moveProperty(Index);
    moveProperty(Device);
    moveProperty(Enabled);
    moveProperty(CreatedAt);
    moveProperty(LastRunAt);
    moveProperty(Repeat);
    moveProperty(Actions);
}
        
const Rule& Rule::operator = (const Rule& src)
{
    copyData(src);
    copyProperty(Id);
    copyProperty(Name);
    copyProperty(Index);
    copyProperty(Device);
    copyProperty(Enabled);
    copyProperty(CreatedAt);
    copyProperty(LastRunAt);
    copyProperty(Repeat);
    copyProperty(Actions);
    return *this;
}


bool Rule::parseFile(const std::string& file)
{
    std::ifstream is(file);

    std::string content, line;
    while(std::getline(is, line))
    {
        content += line;
    }

    nlohmann::json root = nlohmann::json::parse(content);

    if(!parse(root))
    {
        LOG_ERROR << "Rule from " << file << " can not be parsed!";
        return false; 
    }

    if(nullptr == m_Device.resolve())
    {
        LOG_ERROR << "Device " << m_Device.Value << " can not be found";
        return false;
    }
    return true;
}

bool Rule::saveToFile(const std::string& file)
{
    std::ofstream os(file);
    if(!os) {
        LOG_ERROR << "Can't write to " << file << strerror(errno);
        return false;
    }
    nlohmann::json data;
    json(data);
    os << data.dump(4);
    os.flush();
    os.close();
    return true;
}
