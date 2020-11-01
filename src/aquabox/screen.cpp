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


#include "screen.h"
#include "log.h"
#include <singleton.h>
#include "aquabox.h"

using namespace aquabox;

void Screen::show()
{
    auto fb = Singleton<LIBFB_NS::FrameBuffer>::get();
    if (fb == nullptr)
        return;
    
    fb->clear(0xbcded0); 
    
    draw();

    fb->flush();
}

std::string Screen::resource(std::string name)
{
    std::string base = "";
    if (aquabox::Singleton<AquaBox>::get()->workingDirectory().empty()) {
        base = ".";
    } else {
        base = aquabox::Singleton<AquaBox>::get()->workingDirectory();
    }
    if (* base.rbegin() != '/') {
        base += "/";
    }
    base += "assets/";
    return base + name;
}

std::string Screen::asset(std::string name)
{
    return resource(name + std::string(".png"));
}

std::string Screen::font(std::string name)
{
    return resource(name + std::string(".ttf"));
}

std::shared_ptr<LIBFB_NS::PNGImage> Screen::pixmap(std::string name)
{
    auto px = Singleton<LIBFB_NS::PNGProvider>::i()->get(asset(name));
    if (px == nullptr) {
        LOG_ERROR << "Can't find: " << asset(name);
    }
    return px;
}
