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

#include "application.h"
#include "stacktrace.h"
#include <stdlib.h>
#include <iostream>

using namespace aquabox;

Application* Application::m_thisApp = nullptr;

Application::Application(const std::string& identifier)
    : m_identifier(identifier)
{
    m_thisApp = this;
}

void Application::addOption(const std::string& arg, const std::string& description)
{
    m_optionsDescription.add_options()(arg.c_str(), description.c_str());
}

bool Application::optionsIsSet(const std::string option)
{
    return m_options.count(option);
}

int Application::exec(int argc, char** argv)
{
    StackTrace trace;
    try {
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(m_optionsDescription).run(), m_options);
        if (optionsIsSet("help") || optionsIsSet("h")) {
            std::cout << m_identifier << " available options: \n" << m_optionsDescription << std::endl;
            return EXIT_SUCCESS;
        }
        return start();
    }
    catch(std::exception& e) {
        std::cerr << e.what();
    }
    catch(...) {

    }
    return EXIT_FAILURE;
}

int Application::start()
{
    return EXIT_SUCCESS;
}