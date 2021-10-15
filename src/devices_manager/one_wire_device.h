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

#include "hardware.h"
#include "ow_device.h"

namespace aquabox {
class OneWireDevice : public FloatValueDevice {
public:
    OneWireDevice(const w1::Device& dev);

public:
    virtual bool isReady() override;
    virtual bool open(int flags) override;
    virtual bool close() override;
    virtual bool set(double precision) override;
    virtual double get() override;

private:
    w1::Device m_dev;
};
}