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
#include "../networking/network_manager.h"
#include <framebuffer.h>
#include <libfb_globals.h>
#include <common/log.h>
#include <common/singleton.h>
#include <micro_clients/settings.h>

using namespace aquabox;

NetworkChangeScreen::NetworkChangeScreen()
    : m_timeout(std::chrono::seconds(10))
    , m_stageImage(pixmap("networking_status"), 48, 48, 3)
{
    if (Singleton<NetworkManager>::available) {
        auto nm = Singleton<NetworkManager>::i();
        m_lastState = (NetworkDeviceState)nm->deviceState;
        LOG_DEBUG << "Attaching to NetworkManager device state change. Signal Id: "
                  << nm->deviceState.connect(
                         [this](NetworkDeviceState state) { updateState(state); });
    } else {
        LOG_INFO << "Network Manager disabled.";
    }

    m_text.setColor(0x000000ff);
    m_text.setSize(10);
    m_text.setFont(font("DroidSansMono"));
    m_text.setAngle(0.);

    m_networkName.setColor(0x000000ff);
    m_networkName.setSize(10);
    m_networkName.setFont(font("DroidSansMono"));
    m_networkName.setAngle(0.);
}

void NetworkChangeScreen::draw()
{
    auto fb = Singleton<LIBFB_NS::FrameBuffer>::i();
    if (fb == nullptr) {
        return;
    }

    int height = 48 + 5 + m_text.height() + 5 + m_networkName.height();

    int y = (fb->height() - height) / 2;

    int imageX = (fb->width() - 48) / 2;

    int stageTextX = (fb->width() - m_text.width()) / 2;

    int ssidX = (fb->width() - m_networkName.width()) / 2;

    m_stageImage.mapTo(imageX, y);
    fb->draw(&m_stageImage);

    m_text.mapTo(stageTextX, m_stageImage.y() + 5 + m_stageImage.height());

    m_networkName.mapTo(ssidX, m_text.y() + m_text.height() + 5);

    fb->draw(&m_text);
    fb->draw(&m_networkName);
}

bool NetworkChangeScreen::hasUpdate() const
{
    return m_timeout.expired() == 0 || m_inProgress;
}

void NetworkChangeScreen::updateState(NetworkDeviceState state)
{
    m_timeout.reset();
    std::string text;
    m_stageIcon = 0;
    switch (state) {
    case NetworkDeviceState::NM_DEVICE_STATE_DEACTIVATING: {
        text = "Disconnecting";
        m_lastState = state;
        m_inProgress = true;
        break;
    }

    case NetworkDeviceState::NM_DEVICE_STATE_DISCONNECTED: {
        if (m_lastState != NetworkDeviceState::NM_DEVICE_STATE_FAILED || m_text.text().empty()) {
            text = "Disconnected";
            m_stageIcon = 0;
        } else {
            m_stageIcon = 1;
        }
        m_lastState = state;
        m_inProgress = false;
        break;
    }

    case NetworkDeviceState::NM_DEVICE_STATE_PREPARE:
    case NetworkDeviceState::NM_DEVICE_STATE_CONFIG: {
        if (m_lastState == NetworkDeviceState::NM_DEVICE_STATE_NEED_AUTH)
            text = "Authenticating";
        else if (m_lastState == NetworkDeviceState::NM_DEVICE_STATE_DISCONNECTED)
            text = "Connecting";
        m_inProgress = true;
        m_stageIcon = 0;
        break;
    }

    case NetworkDeviceState::NM_DEVICE_STATE_NEED_AUTH: {
        text = "Authenticating";
        m_lastState = state;
        m_inProgress = true;
        m_stageIcon = 0;
        break;
    }

    case NetworkDeviceState::NM_DEVICE_STATE_IP_CONFIG:
    case NetworkDeviceState::NM_DEVICE_STATE_IP_CHECK: {
        text = "Optaining IP";
        m_lastState = state;
        m_inProgress = true;
        m_stageIcon = 0;
        break;
    }

    case NetworkDeviceState::NM_DEVICE_STATE_FAILED: {
        text = "Connection failed";
        m_lastState = state;
        m_inProgress = false;
        m_stageIcon = 1;
        break;
    }

    case NetworkDeviceState::NM_DEVICE_STATE_ACTIVATED: {
        text = "Connected!";
        m_stageIcon = 2;
        m_lastState = state;
        m_inProgress;
        break;
    }

    default: {
        m_inProgress = false;
    }
    }

    m_stageImage.jump(m_stageIcon);

    m_networkName.setText(
        Singleton<NetworkManager>::i()->currentConnection->ssid);

    if (text != "") {
        LOG_DEBUG << "Device state changed text changed to " << text;
        m_text.setText(text);
    }
}