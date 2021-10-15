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

#include "../networking/network_manager_device_state.h"
#include "screen.h"
#include <sprite.h>
#include <text.h>
#include <common/timeout.h>

namespace aquabox {
class NetworkChangeScreen : public Screen {
public:
    NetworkChangeScreen();
    virtual void draw() override;
    bool hasUpdate() const;

private:
    void updateState(NetworkDeviceState state);

private:
    LIBFB_NS::Text m_text;
    LIBFB_NS::Text m_networkName;
    bool m_inProgress = false;
    uint8_t m_stageIcon = 0;
    LIBFB_NS::Sprite m_stageImage;
    NetworkDeviceState m_lastState;
    Timeout m_timeout;
};
}
