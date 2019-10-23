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

#ifndef PATH_H
#define PATH_H

#include <string>
#include <list>

namespace aquabox
{
    class Path: public std::string
    {
    public:
        Path(std::string str = "");
        std::string lastToken() const;
        bool isRelative() const;
        bool normalize();
        bool append(std::string sub);
        bool isDirectory() const;
        bool exists() const;
        std::list<Path> children() const;
        bool contains(const Path& subPath) const;
        Path subPath(const Path& to) const;
        std::string extension() const;
    };
}

#endif // PATH_H