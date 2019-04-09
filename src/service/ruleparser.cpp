#include "ruleparser.h"
#include "rule.h"
#include "path.h"

using namespace aquabox;

std::unordered_map<std::string, std::shared_ptr<Rule>> RulesParser::parse(Path& root)
{
    std::unordered_map<std::string, std::shared_ptr<Rule>> rules;
    LOG_INFO << "Loading rules from" << root.c_str();
    for(Path p : root.children())
    {
        if(p.isDirectory())
            continue;
        LOG_INFO << "Loading rule: " << p.c_str();
        auto rule = std::make_shared<Rule>();
        if(rule->parseFile(p))
            rules.insert({ rule->getId(), rule });
    }
    return rules;
}
