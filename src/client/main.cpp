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

#include "aquabox.h"
#include "options.h"
#include "settings.h"
#include "stacktrace.h"
#include "screen_manager.h"
#include "cloud.h"
#include "network_manager.h"
#include <iostream>
#include <framebuffer.h>
#include <singleton.h>

int main(int argc, char** argv)
{
    aquabox::Settings::setDomain("aquabox");

    bool handleSignals = true;

    aquabox::StackTrace();
    for(int i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "--service") == 0) {
            aquabox::Options::Service = true;
        } else if(strcmp(argv[i], "--nodraw") == 0) {
            aquabox::Options::Render = false;
        }
        else if(strcmp(argv[i], "--lock") == 0) {
            aquabox::Options::Lock = true;
        }
        else if(strcmp(argv[i], "--disable-access-point") == 0) {
            aquabox::Options::AccessPoint = false;
        }
        else if(strcmp(argv[i], "--ignore-networking") == 0) {
            aquabox::Options::UseNetworkManager = false;
        }
        else if(strcmp(argv[i], "--standalone") == 0) {
            aquabox::Options::EnableCloud = false;
        }
        else if(strcmp(argv[i], "--debug") == 0) {
            aquabox::Options::Render = false;
            aquabox::Options::Lock = false;
            aquabox::Options::AccessPoint = false;
            break;
        }
        else if(strcmp(argv[i], "--ignore-signals") == 0) {
            handleSignals = false;
        }
        else if(strcmp(argv[i], "--help") == 0) {
            std::cout << "aquabox [OPTIONS]\n"
                      << " --service                Applications will be runned as part of Linux service\n"
                      << " --nodraw                 Applications will not render on display\n"
                      << " --ignore-lock            Do not create lock/runfiles\n"
                      << " --disable-access-point   Do not switch to AP mode if there is no internet connection\n"
                      << " --standalone             Do not connect to remote service\n"
                      << " --ignore-networking      Do not connect to NetworkManager\n"
                      << " --ignore-signals         Dump stack traces on errors\n"
                      << " --help                   Show this help"
                      << std::endl;
            return 0;
        }
    }

    if (handleSignals) {
        aquabox::StackTrace();
    }
    aquabox::Singleton<LIBFB_NS::FrameBuffer>::available = aquabox::Options::Render;
    aquabox::Singleton<aquabox::ScreenManager>::available = aquabox::Singleton<LIBFB_NS::FrameBuffer>::available;
    aquabox::Singleton<aquabox::Cloud>::available = aquabox::Options::EnableCloud;
    aquabox::Singleton<aquabox::NetworkManager>::available = aquabox::Options::UseNetworkManager;
    return aquabox::Singleton<aquabox::AquaBox>::get()->start();
}