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

#pragma once

#include <string>
#include <unordered_map>

namespace aquabox {

template <typename Enum>
struct EnumHash {
    std::hash<int> i;
    size_t operator()(const Enum& v) const noexcept
    {
        return i(static_cast<int>(v));
    }
};

template <typename EnumClass>
struct Serializable {
    using Str2EnumMap = const std::unordered_map<std::string, EnumClass>;
    using Enum2StrMap = const std::unordered_map<EnumClass, std::string, EnumHash<EnumClass>>;

    static Str2EnumMap str2Val;
    static Enum2StrMap val2Str;

    static EnumClass convert(const std::string value, bool* ok = nullptr)
    {
        auto p = str2Val.find(value);

        if (ok)
            *ok = p != str2Val.end();

        if (p != str2Val.end())
            return p->second;

        return EnumClass();
    }
    static std::string convert(EnumClass value)
    {
        auto pos = val2Str.find(value);
        return pos->second;
    }
};
}
