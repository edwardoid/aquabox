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

#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include "hardware.h"
#include <string>

namespace aquabox
{
    class MockHardware: public Hardware
    {
    public:
        MockHardware(std::string identifier);
    public:
        bool open(int flags) override;
        bool close() override;
        BytesCount send(const Bytes& data, const BytesCount count = 0) override;
        BytesCount read(Bytes& data, const Offset = 0) override;
    private:
        void writeMockData();
        void readMockData(bool force = false);
    private:
        std::string m_identifier;
        bool m_opened = false;
        int m_flags = 0;
        Bytes m_data;
    };
}

#endif // GPIO_H