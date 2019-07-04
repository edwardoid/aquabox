#include "route.h"
#include <sstream>
#include <regex>

using namespace aquabox;

Route& Route::number(std::string name)
{
    m_components.push_back("([\\d]+)");
    m_component2Indx.insert({ name, m_component2Indx.size() });
    return *this;
}

Route& Route::word(std::string word)
{
    m_components.push_back(word);
    return *this;
}

Route& Route::any(std::string name)
{
    m_component2Indx.insert({ name, m_components.size() });
    m_components.push_back("[\\s\\w\\W]+");

    return *this;
}

Route& Route::string(std::string name)
{
    m_component2Indx.insert({ name, m_components.size() });
    m_components.push_back("([\\w]+)");

    return *this;
}

Route Route::sub() const
{
    return *this;
}

std::string Route::url()
{
    std::stringstream ss;
    ss << '^';
    for (auto& token : m_components)
    {
        ss << '/' << token;
    }
    ss << '$';
    return ss.str();
}

std::vector<std::string> Route::parse(std::string& url) const
{
    std::vector<std::string> tokens;
    size_t offset = 1;
    for (auto component : m_components)
    {
        component = "^" + component;
        std::regex rx(component, std::regex_constants::icase);
        std::cmatch match;
        if (!std::regex_search(url.c_str() + offset, match, rx))
        {
            tokens.clear();
            break;
        }
        offset += match.begin()->str().size() + 1;
        tokens.push_back(match.begin()->str());
    }

    return tokens;
}

std::unordered_map<std::string, std::string> Route::extract(std::string& url) const
{
    std::vector<std::string> c = parse(url);
    std::unordered_map<std::string, std::string> extracted;

    for(auto& pv : m_component2Indx)
    {
        if (pv.second < c.size())
            extracted.insert({ pv.first, c[pv.second] });
    }

    return extracted;
}