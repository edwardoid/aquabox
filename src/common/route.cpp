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

#include "route.h"
#include <regex>
#include <sstream>

using namespace aquabox;

Route& Route::number(std::string name)
{
    m_components.push_back("([\\d]+)");
    m_component2Indx.insert({name, m_component2Indx.size()});
    return *this;
}

Route& Route::word(std::string word)
{
    m_components.push_back(word);
    return *this;
}

Route& Route::any(std::string name)
{
    m_component2Indx.insert({name, m_components.size()});
    m_components.push_back("[\\s\\w\\W]+");

    return *this;
}

Route& Route::string(std::string name)
{
    m_component2Indx.insert({name, m_components.size()});
    m_components.push_back("([\\w.+-]+)");

    return *this;
}

Route Route::sub() const
{
    return *this;
}

std::string Route::url() const
{
    std::stringstream ss;
    ss << '^';
    for (auto& token : m_components) {
        ss << '/' << token;
    }
    ss << '$';
    return ss.str();
}

std::vector<std::string> Route::parse(std::string& url) const
{
    std::vector<std::string> tokens;
    size_t offset = 1;
    for (auto component : m_components) {
        component = "^" + component;
        std::regex rx(component, std::regex_constants::icase);
        std::cmatch match;
        const char* ptr = url.c_str() + offset;
        if (!std::regex_search(ptr, match, rx)) {
            tokens.clear();
            break;
        }
        offset += match.begin()->str().size() + 1;
        tokens.push_back(match.begin()->str());
    }

    return tokens;
}

std::unordered_map<std::string, std::string> Route::extract(std::string& url) const
{
    std::vector<std::string> c = parse(url);
    std::unordered_map<std::string, std::string> extracted;

    for (auto& pv : m_component2Indx) {
        if (pv.second < c.size()) {
            extracted.insert({pv.first, c[pv.second]});
        }
    }

    return extracted;
}