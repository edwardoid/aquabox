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

#include "device.h"
#include <map>

namespace aquabox {
class Step : public AbstractStep {
    static const uint64_t INVALID_PRIORITY = static_cast<uint64_t>(-1);

public:
    Step();
    Step(const Step& src) = default;
    Step(Step&& src) = default;

    virtual void addPendingAction(uint64_t priority, ValueChange action) override;
    virtual bool performPendingAction(ValueChange& action) override;

    virtual nlohmann::json value(std::string property) const override;
    virtual bool setValue(std::string property, const nlohmann::json& value) override;

protected:
    virtual int32_t steps() const override;
    virtual void makeSteps(int32_t times = 1) override;

private:
    std::map<int64_t, ValueChange> m_pendingActions;
};
}
