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
#include <mutex>
#include <string>

namespace aquabox
{
    class GPIO : public OnOffDevice
    {
    public:
        GPIO(std::string pin);
        enum class Mode
        {
            Unknown = 0,
            In = 1,
            Out = 2
        };

    private:
        Mode readMode();
        bool writeMode(Mode mode);
        bool exported() const;
        BytesCount send(const Bytes& data, const BytesCount count = 0);
        BytesCount read(Bytes& data, const Offset = 0);

    public:
        virtual bool isReady() override;
        bool isOpen() const;
        virtual bool open(int flags) override;
        virtual bool close() override;

        virtual void setOn() override;
        virtual void setOff() override;
        virtual bool isOn() override;

    private:
        static const std::string SysClassGpio;
        static const std::string SysClassGpioExport;
        static const std::string SysClassGpioUnexport;
        static std::mutex SysClassGpioExportLock;
        static std::mutex SysClassGpioUnexportLock;
        std::string Pin;
        const std::string ValueFile;
        const std::string DirectionFile;
        Mode m_mode;
    };
} // namespace aquabox
