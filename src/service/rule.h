#ifndef RULE_H
#define RULE_H

#include <string>
#include <unordered_map>
#include <memory>
#include "repeattimeout.h"
#include "action.h"
#include "time.h"
#include "devicereference.h"
#include "enumerations.h"
#include "property.h"

namespace aquabox
{
    /*
        {
            "id" : "someId",
            "device" : "dev01",
            "actions" : [
                {
                    "action" : "turn_on",
                    "at" : "09:00"
                },
                {
                    "action" : "turn_off",
                    "at" : "18:55"
                }
            ],
            "createdAt" : <date>,
            "expiresAt" : <date>,
            "expireIn" : "permanent"
            "repeat" : {
                "every" : "minute|hour|day|workday|month",
                "count" : <number>
            }
        }
    */

    class Rule: public ClassProperty
    {
    public:
        Rule();

        Rule(const Rule& src);
        Rule(Rule&& src);
        const Rule& operator = (const Rule& src);

        bool parseFile(const std::string& file);

        StringProperty Id;
        DeviceReference Device;
        Time CreatedAt;
        RepeatTimeout Repeat;
        ClassArrayProperty<Action> Actions;
    };

    using RulesMap = std::unordered_map<std::string, std::shared_ptr<Rule>>;
}

#endif // RULE_H