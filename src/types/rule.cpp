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

#include "rule.h"
#include "../common/log.h"
#include "../common/singleton.h"
#include <fstream>

namespace aquabox {

void to_json(nlohmann::json& j, const Rule& r)
{
    j["id"] = r.id();
    j["name"] = r.name();
    j["index"] = r.index();
    j["device"] = r.device();
    j["enabled"] = r.enabled();
    j["created_at"] = r.createdAt();
    j["last_run"] = r.lastRun();
    j["delete_after"] = r.deleteAfter();
    j["actions"] = r.actions();
}

void from_json(const nlohmann::json& j, Rule& r)
{
    try {
        r.setId(j.at("id"));
        r.setName(j.at("name"));
        r.setIndex(j.at("index"));
        r.setDevice(j.at("device"));
        r.setEnabled(j.at("enabled"));
        r.setCreatedAt(j.at("created_at"));
        r.setLastRun(j.at("last_run"));
        r.setDeleteAfter(j.at("delete_after"));
        r.setActions(j.at("actions"));
    } catch (const nlohmann::detail::exception& e) {
        LOG_ERROR << "Can't serialize Rule: " << e.what();
    }
}
} // namespace aquabox

using namespace aquabox;

const std::string& Rule::id() const { return m_id; }

void Rule::setId(const std::string& id) { m_id = id; }

const std::string& Rule::name() const { return m_name; }

void Rule::setName(const std::string& name) { m_name = name; }

uint32_t Rule::index() const { return m_index; }

void Rule::setIndex(const uint32_t index) { m_index = index; }

const std::string& Rule::device() const { return m_device; }

void Rule::setDevice(const std::string& device) { m_device = device; }

bool Rule::enabled() const { return m_enabled; }

void Rule::setEnabled(bool enabled) { m_enabled = enabled; }

uint64_t Rule::createdAt() const { return m_createdAt; }

void Rule::setCreatedAt(uint64_t createdAt) { m_createdAt = createdAt; }

uint64_t Rule::lastRun() const { return m_lastRun; }

void Rule::setLastRun(uint64_t lastRun) { m_lastRun = lastRun; }

uint64_t Rule::deleteAfter() const { return m_deleteAfter; }

void Rule::setDeleteAfter(uint64_t deleteAfter) { m_deleteAfter = deleteAfter; }

bool Rule::expired() const
{
    if (isTemporary()) {
        auto deleteAt = m_createdAt + m_deleteAfter;
        return std::chrono::system_clock::to_time_t(
                   std::chrono::system_clock::now())
            > deleteAt;
    }
    return false;
}

bool Rule::isTemporary() const { return m_deleteAfter > 0; }

const std::vector<ValueChange>& Rule::actions() const { return m_actions; }

void Rule::setActions(const std::vector<ValueChange>& actions)
{
    m_actions = actions;
}

bool Rule::parseFile(const boost::filesystem::path& file)
{
    try {
        std::ifstream ifs(file.string());
        if (!ifs)
            return false;
        nlohmann::json json;
        ifs >> json;
        *this = json;
        return true;
    } catch (nlohmann::detail::parse_error& e) {
        LOG_ERROR << "Can't parse rule file because " << e.what();
    }
    return true;
}

bool Rule::saveToFile(const boost::filesystem::path& file)
{
    std::ofstream os(file.string());
    if (!os) {
        LOG_ERROR << "Can't write to " << file << strerror(errno);
        return false;
    }
    nlohmann::json data = *this;
    os << data.dump(4);
    os.flush();
    os.close();
    return true;
}
