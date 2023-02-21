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

#include "cloud_screen.h"
#include <common/log.h>
#include <common/singleton.h>
#include <framebuffer.h>
#include <libfb_globals.h>
#include <micro_clients/cloud.h>
#include <micro_clients/settings.h>
using namespace aquabox;

CloudScreen::CloudScreen()
    : m_stateImage(pixmap("cloud"), 64, 64, 2)
    , m_timeout(std::chrono::seconds(5))
{
    m_text.setColor(0x000000ff);
    m_text.setSize(20);
    m_text.setFont(font("DroidSansMono").c_str());
    m_text.setAngle(0.);
}

void CloudScreen::draw()
{
    auto fb = Singleton<LIBFB_NS::FrameBuffer>::i();
    if (fb == nullptr) {
        return;
    }

    if (m_lastState) {
        m_text.setColor(0x5aa61aff);
        m_text.setText("Online");
    }
    else {
        m_text.setColor(0xdd1f0bff);
        m_text.setText("Offline");
    }

    const int height = 64 + 4 + m_text.height();

    int y = (fb->height() - height) / 2;

    m_stateImage.mapTo((fb->width() - m_stateImage.width()) / 2, y);

    m_text.mapTo((fb->width() - m_text.width()) / 2, y + m_stateImage.height() + 4);

    fb->draw(&m_text);
    fb->draw(&m_stateImage);
}

bool CloudScreen::hasUpdate()
{
    if (Singleton<ICloud>::i() == nullptr) {
        return false;
    }

    if (m_lastState != Singleton<ICloud>::i()->connected()) {
        m_lastState = !m_lastState;
        m_stateImage.jump(m_lastState ? 1 : 0);
        m_timeout.reset();
    }

    return m_timeout.expired() == 0;
}
