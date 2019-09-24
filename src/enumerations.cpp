#include "enumerations.h"

using namespace aquabox;

template<>
Serializable<ActionType>::Enum2StrMap Serializable<ActionType>::val2Str({
    { ActionType::TurnOn, "turn_on" },
    { ActionType::TurnOff, "turn_off" }
});

template<>
Serializable<ActionType>::Str2EnumMap Serializable<ActionType>::str2Val({
    { "turn_on", ActionType::TurnOn },
    { "turn_off", ActionType::TurnOff }
});