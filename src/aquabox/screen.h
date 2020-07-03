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

#ifndef SCREEN_H
#define SCREEN_H

#include <pngprovider.h>
#include <framebuffer.h>
#include <pngimage.h>

namespace aquabox
{
    class Screen
    {
    public:
        Screen(std::string fbDev);
        std::string resource(std::string name);
        std::string asset(std::string name);
        std::string font(std::string name);
        std::shared_ptr<LIBFB_NS::PNGImage> pixmap(std::string name);
        void show();
    protected:
        virtual void draw() = 0;
        LIBFB_NS::FrameBuffer& fb();
    private:
        LIBFB_NS::PNGProvider m_provider;
        LIBFB_NS::FrameBuffer m_fb;
    };
}

#endif // SCREEN_H