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

#include "../hardware.h"
#include "hermes_master.h"
#include <memory>

namespace aquabox
{
    class HermesHardware : public virtual Hardware, public virtual OnOff, public virtual IntegerDevice
    {
    public:
        HermesHardware(std::shared_ptr<HermesMaster> impl);
        std::shared_ptr<HermesMaster> master() const { return m_impl; };
        void setMaster(std::shared_ptr<HermesMaster> impl) { m_impl = impl; };

        // Hardware
        virtual bool isReady() override;
        virtual bool open(int flags) override { return true; }
        virtual bool close() override { return true; }

        // OnOff
        virtual void setOn() override;
        virtual void setOff() override;
        virtual bool isOn() override;

        // IntegerDevice
        virtual bool set(int32_t type) override;
        virtual int32_t get() override;

        bool call(const std::string& method);
        bool callInProgress(const std::string& method);

    private:
        mutable std::shared_ptr<HermesMaster> m_impl;
    };
} // namespace aquabox