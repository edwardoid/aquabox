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
#include "backward.hpp"
#include <iostream>

int main(int argc, char** argv)
{
  aquabox::Settings::setDomain("aquabox");
  backward::SignalHandling sh;
  for(int i = 1; i < argc; ++i)
  {
    if(strcmp(argv[i], "--service") == 0) {
      aquabox::Options::Service = true;
    } else if(strcmp(argv[i], "--nodraw") == 0) {
      aquabox::Options::Render = false;
    }
    else if(strcmp(argv[i], "--ignore-lock") == 0) {
      aquabox::Options::Lock = false;
    }
    else if(strcmp(argv[i], "--disable-access-point") == 0) {
      aquabox::Options::AccessPoint = false;
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
    else if(strcmp(argv[i], "--help") == 0) {
      std::cout << "aquabox [OPTIONS]\n"
                << " --service                Applications will be runned as part of Linux service\n"
                << " --nodraw                 Applications will not render on display\n"
                << " --ignore-lock            Do not create lock/runfiles\n"
                << " --disable-access-point   Do not switch to AP mode if there is no internet connection\n"
                << " --standalone             Do not connect to remote service\n"
                << " --help                   Show this help"
                << std::endl;
      return 0;
    }
  }
  return aquabox::AquaBox::i().start();
}