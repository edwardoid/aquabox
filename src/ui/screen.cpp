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
#include <common/log.h>
#include <common/singleton.h>

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

boost::filesystem::path Screen::resource(std::string name)
{
    return boost::filesystem::current_path() / "assets/" / name;
}

boost::filesystem::path Screen::asset(std::string name)
{
    return resource(name + std::string(".png"));
}

boost::filesystem::path Screen::font(std::string name)
{
    return resource(name + std::string(".ttf"));
}

std::shared_ptr<LIBFB_NS::PNGImage> Screen::pixmap(std::string name)
{
    auto px = Singleton<LIBFB_NS::PNGProvider>::i()->get(asset(name).c_str());
    if (px == nullptr) {
        LOG_ERROR << "Can't find: " << asset(name).c_str();
    }
    return px;
}
