#ifndef RULESCONTAINER_H
#define RULESCONTAINER_H

#include <string>
#include <unordered_map>
#include <memory>

namespace aquabox
{
    class Rule;
    class Path;

    class RulesParser
    {
    public:
        static std::unordered_map<std::string, std::shared_ptr<Rule>> parse(Path& root);
    };
}

#endif // RULE_H