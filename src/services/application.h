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

#include <boost/program_options.hpp>

namespace aquabox
{
    class Application
    {
    public:
        Application(const std::string& identifier);
        Application(const Application&) = delete;
        const Application& operator=(const Application&) = delete;
        inline static Application* app() { return m_thisApp; }
        inline const std::string& identifier() const { return m_identifier; }
        void addOption(const std::string& arg, const std::string& description);
        bool optionsIsSet(const std::string option);
        int exec(int argc = 0, char** argv = nullptr);
        virtual int start();

    private:
        std::string m_identifier;
        static Application* m_thisApp;
        boost::program_options::options_description m_optionsDescription;
        boost::program_options::variables_map m_options;
    };
} // namespace aquabox