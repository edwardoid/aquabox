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

#include "timer.h"
#include <common/log.h>

using namespace aquabox;

Timer::Timer(std::string name)
    : m_state(State::Idle)
    , m_name(name)
{}

Timer::~Timer()
{
    stop();
}

const std::string& Timer::name() const
{
    return m_name;
}

bool Timer::start(std::function<void(const ClockTimePoint&)>&& callback, std::chrono::milliseconds timeout)
{
    if (m_state.load() != State::Idle) {
        LOG_ERROR << "Timer \"" << m_name << "\" is already running";
        return false;
    }

    LOG_DEBUG << "Starting new timer \"" << m_name << "\"";

    m_callback = callback;
    m_timeout = timeout;

    m_state.store(State::Starting);

    m_thread = std::move(std::thread(std::bind(&Timer::run, this)));
    pthread_setname_np(m_thread.native_handle(), m_name.c_str());
    return true;
}

void Timer::stop()
{
    LOG_ERROR_IF(!m_thread.joinable()) << "Can't stop timer \"" << m_name << "\"";
    m_state.exchange(State::Stoppping);
    wait();
}

void Timer::wait()
{
    m_thread.join();
}

void Timer::run()
{
    if (State::Starting != m_state) {
        return;
    }

    m_state = State::Running;

    uint64_t tick = 0;
    ClockTimePoint startPoint = Clock::now();
    while (m_state.load() == State::Running) {
        ClockTimePoint next = (startPoint + m_timeout * ++tick);

        try {
            ClockTimePoint now = Clock::now();
            m_callback(now);
        }
        catch (std::exception e) {
            LOG_ERROR << "Exception occured in timer \"" << m_name << "\" details: " << e.what();
        }
        catch (...) {
            LOG_ERROR << "Exception occured in timer \"" << m_name << "\" details: " << strerror(errno);
        }

        std::this_thread::sleep_until(next);
    }

    m_state.exchange(State::Stoppping);
}