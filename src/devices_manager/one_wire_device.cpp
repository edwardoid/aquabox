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

#include "one_wire_device.h"

using namespace aquabox;

OneWireDevice::OneWireDevice(const w1::Device& dev)
    : m_dev(dev)
{
}

bool OneWireDevice::isReady() { return true; }

bool OneWireDevice::open(int) { return true; }

bool OneWireDevice::close() { return true; }

bool OneWireDevice::set(double precision)
{
    int p = precision;
    m_dev.setPrecition(static_cast<w1::Precision>(p));
    return true;
}

double OneWireDevice::get() { return m_dev.temperature(); }