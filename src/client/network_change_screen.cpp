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

#include "network_change_screen.h"
#include <log.h>
#include "settings.h"
#include "network_manager.h"
#include <framebuffer.h>
#include <singleton.h>
#include <libfb_globals.h>
using namespace aquabox;

NetworkChangeScreen::NetworkChangeScreen()
    : m_timeout(std::chrono::seconds(5))
{
    if (Singleton<NetworkManager>::available)
    {
        auto nm = Singleton<NetworkManager>::i();
        m_lastState = (NetworkDeviceState) nm->deviceState;
        LOG_DEBUG << "Attaching to NetworkManager device state change. Signal Id: " << nm->deviceState.connect([this](NetworkDeviceState state) { updateState(state); });
    } else {
        LOG_INFO << "Network Manager disabled.";
    }

    m_text.setColor(0x5aa61aff);
    m_text.setSize(12);
    m_text.setFont(font("DroidSansMono"));
    m_text.setAngle(0.);
}

void NetworkChangeScreen::draw()
{
    auto fb = Singleton<LIBFB_NS::FrameBuffer>::i();
    if (fb == nullptr) {
        return;
    }

    StatusBarScreen::draw();

    m_text.mapTo((fb->width() - m_text.width()) / 2, 
                 (fb->height() - m_text.height()) / 2 + m_text.height() / 2);

    fb->draw(&m_text);
}

bool NetworkChangeScreen::hasUpdate() const
{
    return m_timeout.expired() == 0;
}

void NetworkChangeScreen::updateState(NetworkDeviceState state)
{
    m_timeout.reset();
    std::string text;
    switch(state)
    {
        case NetworkDeviceState::NM_DEVICE_STATE_DEACTIVATING: {
            text = "Disconnecting";
            m_lastState = state;
            break;
        }

        case NetworkDeviceState::NM_DEVICE_STATE_DISCONNECTED: {
            text = "Disconnected";
            m_lastState = state;
            break;
        }

        case NetworkDeviceState::NM_DEVICE_STATE_PREPARE: {
            if (m_lastState == NetworkDeviceState::NM_DEVICE_STATE_NEED_AUTH)
                text = "Authentificating";
            else if (m_lastState == NetworkDeviceState::NM_DEVICE_STATE_DISCONNECTED)
                text = "Connecting";
            m_lastState = state;
            break;
        }

        case NetworkDeviceState::NM_DEVICE_STATE_NEED_AUTH: {
            text = "Authentificating";
            m_lastState = state;
            break;
        }

        case NetworkDeviceState::NM_DEVICE_STATE_IP_CONFIG:
        case NetworkDeviceState::NM_DEVICE_STATE_IP_CHECK: {
            text = "Optaining IP";
            m_lastState = state;
            break;
        }

        case NetworkDeviceState::NM_DEVICE_STATE_FAILED: {
            text = "Connection failed";
            m_lastState = state;
            break;
        }

        case NetworkDeviceState::NM_DEVICE_STATE_ACTIVATED: {
            text = "Connected!";
            m_lastState = state;
            break;
        }

        default: {}
    }

    if (text != "") {
        LOG_DEBUG << "Device state changed text changed to " << text;
        m_text.setText(text);
    }
}