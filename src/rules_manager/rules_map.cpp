/*
 * AquaBox
 * Copyright (C) 2019-2022  Eduard Sargsyan
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

#include "rules_map.h"
#include "rules_scheduler.h"
#include <common/log.h>
#include <micro_clients/devices_manager.h>
#include <micro_clients/events.h>
#include <micro_clients/settings.h>

using namespace aquabox;

bool RulesMap::addRule(std::shared_ptr<Rule> rule, bool replace)
{
    nlohmann::json beforeUpdate;

    bool replaceError = false;

    if (on(rule->id(), [this, rule, &beforeUpdate, replace, &replaceError](auto& k, auto& v) {
            replaceError = !replace;

            if (!replaceError) {
                beforeUpdate = *v;
                v = rule;
            }
        })) {
        if (replaceError) {
            LOG_ERROR << "Rule " << rule->id() << " has already beed added, and replace flag was not set";
            return false;
        };
    }
    else {
        insert(rule->id(), rule);
    }
    
    if (saveRule(rule->id())) {
        auto dev = Singleton<IDevicesManager>::i()->get(rule->device());
        Singleton<IEvents>::i()->notify(std::make_shared<DeviceRulesChangedEvent>(dev));
        if (replace) {
            Singleton<IEvents>::i()->notify(std::make_shared<RuleUpdatedEvent>(*rule, beforeUpdate));
        }
        else {
            Singleton<IEvents>::i()->notify(std::make_shared<RuleCreatedEvent>(*rule));
        }
        LOG_INFO << "Rule saved";
        return true;
    }
    LOG_ERROR << "Can't save rule!";
    return false;
}

bool RulesMap::saveRule(const std::string& id)
{
    bool saved = false;
    on<bool>(
        id,
        [this](auto id, auto rule) {
            boost::filesystem::path root = Singleton<ISettings>::i()->path(Settings::Keys::DATA_ROOT, Settings::Defaults::DATA_ROOT)
                                            / Singleton<ISettings>::i()->path(Settings::Keys::RULES_DATA_SUBROOT, Settings::Defaults::RULES_DATA_SUBROOT)
                                            / rule->id();

            root.replace_extension("json");

            if (!rule->saveToFile(root)) {
                LOG_ERROR << "Can't save rule file " << root.c_str();
                erase(rule->id());
                return false;
            }

            LOG_DEBUG << "Updated rule file " << root.c_str();

            return true;
        },
        saved);

    return saved;
}

bool RulesMap::removeRule(const std::string& id)
{
    DeviceRef device;
    on<DeviceRef>(
        id,
        [this](auto id, auto rule) {
            Singleton<IEvents>::i()->notify(std::make_shared<RuleDeletedEvent>(*rule));
            Singleton<RuleScheduler>::i()->remove(*rule);
            return Singleton<IDevicesManager>::i()->get(rule->device());
        },
        device);

    if (erase(id)) {
        Singleton<RuleScheduler>::i()->updateDeviceRules(device.id());
        Singleton<IEvents>::i()->notify(std::make_shared<DeviceRulesChangedEvent>(device));
        boost::filesystem::path root = Singleton<ISettings>::i()->path(Settings::Keys::DATA_ROOT, Settings::Defaults::DATA_ROOT)
                                            / Singleton<ISettings>::i()->path(Settings::Keys::RULES_DATA_SUBROOT, Settings::Defaults::RULES_DATA_SUBROOT)
                                            / id;

        root.replace_extension("json");

        if (!boost::filesystem::exists(root)) {
            LOG_ERROR << "Can't find rule " << id << " tried: " << root.string();
            return false;
        }
        boost::filesystem::remove(root);

        if (boost::filesystem::exists(root)) {
            LOG_ERROR << "Can't remove file " << root.string();
            return false;
        }
    }
    return true;
}