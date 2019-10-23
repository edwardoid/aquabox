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

#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <unordered_map>
#include <string>

namespace aquabox
{
    class Route
    {
    public:
        Route& any(std::string name = "");
        Route& string(std::string name = "");
        Route& word(std::string work);
        Route& number(std::string name = "");

        Route sub() const;

        std::string url();

        std::vector<std::string> parse(std::string& url) const;
        std::unordered_map<std::string, std::string> extract(std::string& url) const;
    private:
        std::unordered_map<std::string, size_t> m_component2Indx;
        std::vector<std::string> m_components;
    };
}
#endif // ROUTE_H