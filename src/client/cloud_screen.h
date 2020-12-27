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

#include <text.h>
#include "screen.h"
#include <timeout.h>
#include <sprite.h>
#include <queue>

namespace aquabox
{
    class CloudScreen: public Screen
    {
    public:
        CloudScreen();
        virtual void draw() override;
        bool hasUpdate();
    private:
        LIBFB_NS::Sprite m_stateImage;
        Timeout m_timeout;
        bool m_lastState = false;
        LIBFB_NS::Text m_text;
        std::string m_icon;
    };
}
