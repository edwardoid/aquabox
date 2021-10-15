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

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

namespace aquabox {
namespace services {
    template <typename Interface>
    class ServerBase : public Interface ::Service {
    public:
        ServerBase()
        {
            plog::init(plog::Severity::debug, &m_consoleAppender);
        }

        virtual bool init()
        {
            return true;
        }

    private:
        plog::ColorConsoleAppender<plog::TxtFormatter> m_consoleAppender;
    };
}
}