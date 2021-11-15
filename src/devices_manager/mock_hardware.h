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

#include "hardware.h"
#include <common/log.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

namespace aquabox {
template <typename MockedData, class HardwareInterface>
class MockHardware : public HardwareInterface {
public:
    MockHardware(std::string identifier)
        : m_identifier(identifier)
    {
        readMockData();
    }

public:
    bool open(int flags) override
    {
        LOG_ERROR << "Opening with flags " << flags;
        m_flags = flags;
        m_opened = true;
        writeMockData();
        return true;
    }

    bool close() override
    {
        LOG_ERROR << "Closing device";
        m_opened = false;
        writeMockData();
        return true;
    }

protected:
    void writeMockData()
    {
        nlohmann::json data({ { "identifier", m_identifier },
            { "opened", m_opened },
            { "flags", m_flags },
            { "data", m_data } });

        std::ofstream os(m_identifier + "_mock.json");
        os << data.dump(4);
        os.close();
    }
    void readMockData(bool force = false)
    {
        if (!force && (time(NULL) % 5 != 0)) {
            return;
        }

        std::ifstream is(m_identifier + "_mock.json");
        std::string dataStr, line;
        while (std::getline(is, line)) {
            dataStr += line;
        }
        try {
            nlohmann::json data = nlohmann::json::parse(dataStr);

            m_opened = data["opened"].get<bool>();
            m_flags = data["flags"].get<int>();
            m_data = data["data"].get<MockedData>();
        } catch (nlohmann::json::parse_error ex) {
            LOG_ERROR << "Can't deserialize mock data file: " << ex.what() << " content: " << dataStr;
            writeMockData();
        } catch (std::exception ex) {
            LOG_ERROR << "Can't deserialize mock data file: " << ex.what() << " content: " << dataStr;
            writeMockData();
        }

        is.close();
    }

protected:
    std::string m_identifier;
    bool m_opened = false;
    int m_flags = 0;
    MockedData m_data;
};

struct MockedOnOffDevice : public MockHardware<bool, OnOffDevice> {
    MockedOnOffDevice(std::string identifier)
        : MockHardware(identifier)
    {
    }

    virtual bool isReady() { return true; }

    virtual void setOn() override
    {
        m_data = true;
        writeMockData();
    }

    virtual void setOff() override
    {
        m_data = false;
        writeMockData();
    }
    virtual bool isOn() override
    {
        return m_data;
    }
};
}