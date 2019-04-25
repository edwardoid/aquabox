#include "device.h"
#include "aquabox.h"
#include "rule.h"
#include "log.h"
#include "gpio.h"
#include "event.h"
#include <time.h>

using namespace aquabox;

Device::Device()
    : ClassProperty("-class Device", { &m_Id, &m_Index, &m_Pin, &m_RulesEnabled, &m_Name, &m_Type, &m_Class, &m_Meta })
    , m_Id("id"), m_Index("index", false), m_Pin("pin"), m_RulesEnabled("rulesEnabled")
    , m_Name("name"), m_Type("type"), m_Class("class"), m_Meta("meta")
{}

Device::Device(const Device& src)
    : Device()
{
    *this = src;
}

Device::Device(Device&& src)
    : Device()
{
    moveData(src);
    moveProperty(Id);
    moveProperty(Index);
    moveProperty(Pin);
    moveProperty(RulesEnabled);
    moveProperty(Name);
    moveProperty(Type);
    moveProperty(Class);
    moveProperty(Meta);
}

const Device& Device::operator = (const Device& src)
{
    copyData(src);
    copyProperty(Id);
    copyProperty(Index);
    copyProperty(Pin);
    copyProperty(RulesEnabled);
    copyProperty(Name);
    copyProperty(Type);
    copyProperty(Class);
    copyProperty(Meta);

    return *this;
}

void Device::copyData(const Device& src)
{
    copyProperty(Index);
    copyProperty(RulesEnabled);
    copyProperty(Name);
    copyProperty(Type);
    copyProperty(Class);
    copyProperty(Meta);
}

std::vector<std::shared_ptr<Rule>> Device::rules() const
{
    RulesMap rules = AquaBox::i().rules();
    std::vector<std::shared_ptr<Rule>> res;
    for(auto r : rules) {
        if (r.second->getDevice().Value == m_Id.Value) {
            res.push_back(r.second);
        }
    }

    return res;
}

bool Device::turnedOn() const
{
    auto gpio = GPIO::get(m_Pin.Value);
    return !gpio->isHigh();
}

void Device::turnOn()
{
    auto gpio = GPIO::get(m_Pin.Value);
    if (gpio == nullptr) {
        LOG_ERROR << "Can't find attached GPIO " << (int)m_Pin.Value;
    }

    if (turnedOn()) {
        return;
    }

    static Bytes ON({ 0 });
    if (ON.size() != gpio->send(ON, ON.size())) {
        LOG_ERROR << "Can't write to GPIO " << (int)m_Pin.Value;
    }

    LOG_DEBUG << "Turned on";
}

void Device::turnOff()
{
    auto gpio = GPIO::get(m_Pin.Value);
    if (gpio == nullptr) {
        LOG_ERROR << "Can't find attached GPIO " << (int)m_Pin.Value;
    }

    if (!turnedOn()) {
        return;
    }

    static Bytes OFF({ 1 });
    if (OFF.size() != gpio->send(OFF, OFF.size())) {
        LOG_ERROR << "Can't write to GPIO " << (int)m_Pin.Value;
    }

    LOG_DEBUG << "Turned off";
}

void Device::addPendingAction(uint64_t priority, ActionType action)
{
    if (m_pendingActionPriority == INVALID_PRIORITY || m_pendingActionPriority >= priority) {
        m_pendingAction = action;
        m_pendingActionPriority = priority;
    }
}

bool Device::performPendingAction()
{
    if (m_pendingActionPriority != INVALID_PRIORITY) {
        switch(m_pendingAction)
        {
            case ActionType::TurnOn: {
                turnOn();
                break;
            }
            case ActionType::TurnOff: {
                turnOff();
                break;
            }
        }
        m_pendingActionPriority = INVALID_PRIORITY;
        return true;
    }

    return false;
}
