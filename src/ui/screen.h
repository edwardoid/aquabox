/*
 * AquaBox
 * Copyright (C) 2019-2020  Eduard Sargsyan
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

#include <framebuffer.h>
#include <pngimage.h>
#include <pngprovider.h>
#include <boost/filesystem.hpp>

namespace aquabox {
class Screen {
public:
    Screen() = default;
    static const boost::filesystem::path& resource(const std::string& name);
    static const boost::filesystem::path& asset(const std::string& name);
    static const boost::filesystem::path& font(const std::string& name);
    static std::shared_ptr<LIBFB_NS::PNGImage> pixmap(const std::string& name);
    void show();

protected:
    virtual void draw() = 0;
};
}
