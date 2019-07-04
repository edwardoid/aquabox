#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <unordered_map>
#include <string>

namespace aquabox
{
    class Route
    {
    public:
        Route& any(std::string name = "");
        Route& string(std::string name = "");
        Route& word(std::string work);
        Route& number(std::string name = "");

        Route sub() const;

        std::string url();

        std::vector<std::string> parse(std::string& url) const;
        std::unordered_map<std::string, std::string> extract(std::string& url) const;
    private:
        std::unordered_map<std::string, size_t> m_component2Indx;
        std::vector<std::string> m_components;
    };
}
#endif // ROUTE_H