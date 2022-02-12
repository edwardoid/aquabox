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

#include "gpio.h"
#include <common/log.h>
#include <map>
#include <string.h>

using namespace aquabox;

const std::string GPIO::SysClassGpio("/sys/class/gpio");
const std::string GPIO::SysClassGpioExport("/sys/class/gpio/export");
const std::string GPIO::SysClassGpioUnexport("/sys/class/gpio/unexport");

std::mutex GPIO::SysClassGpioExportLock;
std::mutex GPIO::SysClassGpioUnexportLock;

GPIO::GPIO(std::string pin)
    : Pin(pin)
    , ValueFile(SysClassGpio + std::string("/gpio") + pin + std::string("/value"))
    , DirectionFile(SysClassGpio + std::string("/gpio") + pin + std::string("/direction"))
    , m_mode(Mode::Unknown)
{
    Bytes b({1});
    open(static_cast<int>(Mode::Out));
    send(b, b.size());
}

GPIO::Mode GPIO::readMode()
{
    m_mode = Mode::Unknown;
    auto fp = fopen(DirectionFile.c_str(), "r");
    if (fp != nullptr) {
        char data[32];
        auto count = fread(data, sizeof(char), 32, fp);
        data[31] = '\0';
        fclose(fp);
        if (count < 2) {
            LOG_ERROR << "Read less than expected. Got " << count << " expected minimum 2";
            return m_mode;
        }

        if (strncmp(data, "in", 2) == 0)
            m_mode = Mode::In;
        else if (strncmp(data, "out", 3) == 0)
            m_mode = Mode::Out;
        else
            LOG_DEBUG << "Got " << data << " from " << DirectionFile;
    }
    else {
        LOG_DEBUG << "Can't open " << DirectionFile << " error: " << strerror(errno);
    }

    return m_mode;
}

bool GPIO::writeMode(Mode mode)
{
    if (mode == Mode::Unknown) {
        return close();
    }

    auto fp = fopen(DirectionFile.c_str(), "w");
    if (fp != nullptr) {
        const char* dir = mode == Mode::In ? "in" : "out";

        auto count = fwrite(dir, sizeof(char), strlen(dir) + 1, fp);
        fflush(fp);
        fclose(fp);

        m_mode = count == (strlen(dir) + 1) ? mode : Mode::Unknown;
    }
    else {
        LOG_DEBUG << "Can't open " << DirectionFile << " error: " << strerror(errno);
    }

    return m_mode == mode;
}

bool GPIO::exported() const
{
    struct stat buffer;
    return (stat(DirectionFile.c_str(), &buffer) == 0) && (stat(ValueFile.c_str(), &buffer) == 0);
}

bool GPIO::open(int flags)
{
    if (exported()) {
        writeMode(static_cast<Mode>(flags));
        return true;
    }

    std::lock_guard<std::mutex> lock(SysClassGpioExportLock);
    LOG_DEBUG << "Opening " << SysClassGpioExport << " for " << Pin;
    auto fp = fopen(SysClassGpioExport.c_str(), "w");
    if (fp == nullptr) {
        LOG_ERROR << "Can't open " << SysClassGpioExport << " for write errno: " << strerror(errno);
        return false;
    }

    auto count = fwrite(Pin.c_str(), sizeof(std::string::value_type), Pin.size() + 1, fp);
    fflush(fp);
    if (fclose(fp) != 0) {
        LOG_ERROR << "Can't close " << SysClassGpioExport << " errno: " << strerror(errno);
    }

    writeMode(static_cast<Mode>(flags));

    return count == Pin.size() + 1 && readMode() != static_cast<Mode>(flags);
}

bool GPIO::close()
{
    if (!exported()) {
        return true;
    }

    std::lock_guard<std::mutex> lock(SysClassGpioUnexportLock);

    LOG_DEBUG << "Opening " << SysClassGpioUnexport << " for " << Pin;

    auto fp = fopen(SysClassGpioUnexport.c_str(), "w");
    if (fp == nullptr) {
        LOG_ERROR << "Can't open " << SysClassGpioUnexport << " error: " << strerror(errno);
        return false;
    }

    auto count = fwrite(Pin.c_str(), sizeof(std::string::value_type), Pin.size() + 1, fp);
    fflush(fp);
    if (fclose(fp) != 0) {
        LOG_ERROR << "Can't close " << SysClassGpioUnexport << " error: " << strerror(errno);
    }

    m_mode = Mode::Unknown;
    return count == Pin.size() + 1;
}

BytesCount GPIO::send(const Bytes& data, const BytesCount count)
{
    auto fp = fopen(ValueFile.c_str(), "w");
    if (fp != nullptr) {
        if (count > 0) {
            const char v[] = {static_cast<char>(data[0] + '0'), 0};
            auto count = fwrite(&v, sizeof(char), 2, fp);
            fflush(fp);
        }
        fclose(fp);
        return count > 0;
    }
    else {
        LOG_DEBUG << "Can't open " << DirectionFile << " error: " << strerror(errno);
    }

    return 0;
}

BytesCount GPIO::read(Bytes& data, const Offset)
{
    if (m_mode == Mode::Unknown) {
        LOG_DEBUG << "GPIO is in Unknown mode";
        return 0;
    }

    bool isHigh = false;
    auto fp = fopen(ValueFile.c_str(), "r");
    if (fp != nullptr) {
        char data[] = {0, 0};
        auto count = fread(data, sizeof(char), 2, fp);
        if (fclose(fp) != 0) {
            LOG_ERROR << "Can't close " << ValueFile << " error: " << strerror(errno);
        }
        if (count == 0) {
            LOG_DEBUG << "From " << ValueFile << " read " << count << " bytes.";
        }
        isHigh = count > 0 ? data[0] == '1' : false;
    }
    else {
        LOG_DEBUG << "Can't open " << ValueFile << " error: " << strerror(errno);
        return 0;
    }

    data[0] = isHigh ? 1 : 0;
    return 1;
}

void GPIO::setOn()
{
    static Bytes ON({0});
    if (ON.size() != send(ON, ON.size())) {
        LOG_ERROR << "Can't write to GPIO " << ValueFile;
    }
}

void GPIO::setOff()
{
    static Bytes OFF({1});
    if (OFF.size() != send(OFF, OFF.size())) {
        LOG_ERROR << "Can't write to GPIO " << ValueFile;
    }
}

bool GPIO::isOn()
{
    static Bytes V({1});
    if (0 == read(V)) {
        return false;
    }
    return V[0] == 0;
}

bool GPIO::isReady()
{
    return true;
}
