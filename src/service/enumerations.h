#ifndef ENUMERRATIONS_H
#define ENUMERRATIONS_H

#include <unordered_map>

namespace aquabox
{

    template<typename Enum>
    struct EnumHash {
        std::hash<int> i;
        size_t operator()(const Enum& v) const noexcept
        { return i(static_cast<int>(v)); }
    };

    template<typename EnumClass>
    struct Serializable
    {
        using Str2EnumMap = const std::unordered_map<std::string, EnumClass>;
        using Enum2StrMap = const std::unordered_map<EnumClass, std::string, EnumHash<EnumClass>>;

        static Str2EnumMap str2Val;
        static Enum2StrMap val2Str;

        static EnumClass convert(const std::string value, bool* ok = nullptr)
        {
            auto p = str2Val.find(value);
            
            if (ok) *ok = p != str2Val.end();
            
            if (p != str2Val.end()) return p->second;

            return EnumClass();
        }
        static std::string convert(EnumClass value) { auto pos = val2Str.find(value); return pos->second; }
    };

    enum class ActionType : int {
        TurnOn = 0,
        TurnOff
    };
}

#endif // ENUMERRATIONS_H