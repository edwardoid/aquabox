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

#include "hermes_incubator.h"
#include <log.h>

using namespace aquabox;

HermesIncubator::HermesIncubator()
{
    m_routine = std::thread(std::bind(&HermesIncubator::routine, this));
    pthread_setname_np(m_routine.native_handle(), "HermesIncubator");
}

int HermesIncubator::incubate(std::shared_ptr<HermesMaster> obj)
{
    static int idx = 0;
    std::lock_guard<std::mutex> lock(m_mx);
    HermesObjectInfo info;
    info.id = ++idx;
    info.object = obj;
    info.state = ObjectInfoState::Incubating;
    info.tp = std::chrono::high_resolution_clock::now();
    m_objects.push_back(info);
    LOG_DEBUG << "Added new hermes slave for incubation with index " << info.id;
    return info.id;
}

void HermesIncubator::routine()
{
    int round = 0;
    while(1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::lock_guard<std::mutex> lock(m_mx);

        ++round;

        if (m_objects.empty()) continue;

        std::vector<HermesObjectInfo> readyObjects;
        std::vector<HermesObjectInfo> deadObjects;
        std::vector<size_t> toRemove;

        auto now = std::chrono::high_resolution_clock::now();
        for(size_t i = 0; i < m_objects.size(); ++i) {
            HermesObjectInfo& obj = m_objects[i];
            LOG_DEBUG << "Processing slave with state " << obj.state; 
            if (obj.state == Incubating) {
                std::lock_guard<HermesMaster> objLock(*obj.object);
                if (obj.object->hasIncomingMessage()) {
                    LOG_DEBUG << "Hermes slave has incoming message!";
                    if(!obj.object->handleHandshake()) {
                        LOG_DEBUG << "Handshake failed"; 
                        deadObjects.push_back(obj);
                        obj.state = ObjectInfoState::Dead;
                        toRemove.push_back(i);
                    } else {
                        LOG_INFO << "Hermes slave " << obj.object->serial().to_string() << " accepted";
                        readyObjects.push_back(obj);
                        obj.state = ObjectInfoState::Ready;
                    }
                } else if ((now - obj.tp) > std::chrono::seconds(10)) {
                    LOG_DEBUG << "Waiting for handshake message";
                    deadObjects.push_back(obj);
                    obj.state = ObjectInfoState::Dead;
                    toRemove.push_back(i);
                }
            } else if (obj.state == Ready) {
                if (round % 10 == 0) {

                    std::lock_guard<HermesMaster> objLock(*obj.object);
                    if (obj.object->getPropertiesCount() < 1) {
                        deadObjects.push_back(obj);
                        obj.state = ObjectInfoState::Dead;
                        toRemove.push_back(i);
                    }
                }
            }
        }

        for(auto r : readyObjects) {
            ready.emit(r.id, r.object);
        }

        for(auto d : deadObjects) {
            dead.emit(d.id, d.object);
        }

        for(auto i = toRemove.rbegin(); i != toRemove.rend(); ++i) {
            m_objects.erase(m_objects.begin() + *i);
        }
    }
}