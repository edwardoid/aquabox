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

#include "hermes_bus.h"
#include "../devices_map.h"
#include <types/box_event.h>
#include <types/update_events.h>
#include "../events/device_events.h"
#include "../device.h"
#include "../hermes/hermes_device_builder.h"
#include "../hermes/hermes_hardware.h"
#include "../hermes/hermes_incubator.h"
#include <common/log.h>
#include <common/singleton.h>
#include <common/thread_pool.h>
#include <services/events.h>
#include <micro_clients/settings.h>
#include <micro_clients/events.h>

using namespace aquabox;

HermesBus::HermesBus(const std::string& address)
    : Bus(address)
{
    Singleton<HermesIncubator>::i()->ready.connect(
        [this](int id, std::shared_ptr<HermesMaster> master) {
            if (m_master.find(id) != m_master.end()) {
                LOG_DEBUG << "Slave " << master->serial().to_string() << " index "
                          << id << " is ready";
                bool enableCaching = false;
                if (master->getProperty<bool>("_cache", enableCaching)) {
                    if (enableCaching) {
                        master->enableCaching();
                    } else {
                        master->disableCaching();
                    }
                } else {
                    master->disableCaching();
                }
                std::string address = m_address + ":";
                address += master->serial().to_string();
                std::string devId = Singleton<DevicesMap>::i()->deviceIdByAddress(address);

                if (devId.empty()) {
                    auto dev = HermesDeviceBuilder::build(address, master);
                    if (dev != nullptr) {
                        devId = Singleton<DevicesMap>::i()->nextId();
                        dev->setId(devId);
                        dev->accept(false);
                        dev->setAvailable(true);
                        std::string newDevId = Singleton<DevicesMap>::i()->addDevice(dev);
                        if (!newDevId.empty()) {
                            Singleton<IEvents>::i()->notify(std::make_shared<DeviceAddedEvent>(dev));
                        }
                    } else {
                        LOG_ERROR << "Can't build Hermes Device " << address;
                    }
                } else {
                    LOG_DEBUG << "Device " << address << " already exists";
                    Singleton<DevicesMap>::i()->on(
                        devId,
                        [address](const std::string&, std::shared_ptr<Device> dev) {
                            dev->setAvailable(true);
                            nlohmann::json upd = { { "available", true } };
                            Singleton<IEvents>::i()->notify(std::make_shared<DevicePropertyUpdateEvent>(dev, upd));
                        });
                }

                m_master[id]->setMaster(master);
            }
        });

    Singleton<HermesIncubator>::i()->dead.connect(
        [this](int id, std::shared_ptr<HermesMaster> master) {
            if (m_master.find(id) != m_master.end()) {
                LOG_DEBUG << "Slave " << master->serial().to_string() << " index "
                          << id << " is dead";
                std::string address = m_address + ":";
                address += master->serial().to_string();

                std::string devId = Singleton<DevicesMap>::i()->deviceIdByAddress(address);
                Singleton<DevicesMap>::i()->on(
                    devId, [](const std::string&, std::shared_ptr<Device> dev) {
                        dev->setAvailable(false);
                        nlohmann::json upd = { { "available", false } };
                        Singleton<IEvents>::i()->notify(std::make_shared<DevicePropertyUpdateEvent>(dev, upd));
                    });
                m_master.erase(id);
            }
        });
}

HermesBus::~HermesBus() { }

void HermesBus::start()
{
    m_thread = std::thread([this] { loop(); });
}

std::shared_ptr<Hardware> HermesBus::hw(const std::string& hw)
{
    for (auto m : m_master) {
        if (m.second != nullptr && m.second->master() != nullptr && m.second->master()->serial().to_string() == hw) {
            return m.second;
        } else {
            LOG_DEBUG << "Hermes slave " << hw << " is not ready";
        }
    }

    return nullptr;
}

void HermesBus::newConnection(std::shared_ptr<proto::MessageIO> io,
    const HermesSerial& serial)
{
    auto master = std::make_shared<HermesMaster>(io);
    if (serial.isValid()) {
        master->setSerial(serial);
    }
    auto hw = std::make_shared<HermesHardware>(master);
    auto id = Singleton<HermesIncubator>::i()->incubate(master);
    m_master[id] = hw;
}

void HermesBus::reconnected(const HermesSerial& serial)
{
    for (auto& entry : m_master) {
        auto master = entry.second->master();
        if (master != nullptr && master->serial() == serial) {
            m_master.erase(entry.first);
            newConnection(master->io(), master->serial());
        }
    }
}
