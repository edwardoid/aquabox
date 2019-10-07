#include "mock_hardware.h"
#include "log.h"
#include <nlohmann/json.hpp>
#include <fstream>

using namespace aquabox;

MockHardware::MockHardware(std::string identifier)
    : Hardware()
    , m_identifier(identifier)
{
    readMockData();
}

bool MockHardware::open(int flags)
{
    LOG_ERROR << "Opening with flags " << flags;
    m_flags = flags;
    m_opened = true;
    writeMockData();
    return true;
}

bool MockHardware::close()
{
    LOG_ERROR << "Closing device";
    m_opened = false;
    writeMockData();
    return true;
}

BytesCount MockHardware::send(const Bytes& data, const BytesCount count)
{
    LOG_ERROR << "Writing " << data.size() << " bytes";
    m_data = data;
    m_data.resize(count);
    writeMockData();
    return count;
}

BytesCount MockHardware::read(Bytes& data, const Offset)
{
    readMockData();
    data = m_data;
    return data.size();
}

void MockHardware::writeMockData()
{
    nlohmann::json data ({
        { "identifier", m_identifier },
        { "opened", m_opened },
        { "flags", m_flags },
        { "data", m_data }
    });

    std::ofstream os(m_identifier + "_mock.json");
    os << data.dump(4);
    os.close();
}

void MockHardware::readMockData(bool force)
{
    if (!force && (time(NULL) % 5 != 0)) {
        return;
    }

    std::ifstream is(m_identifier + "_mock.json");
    std::string dataStr, line;
    while(std::getline(is, line)) {
        dataStr += line;
    }
    try {
        nlohmann::json data = nlohmann::json::parse(dataStr);

        m_opened = data["opened"].get<bool>();
        m_flags = data["flags"].get<int>();
        std::vector<int> rawdata = data["data"].get<std::vector<int>>();
        m_data.resize(rawdata.size());
        m_data.assign(rawdata.begin(), rawdata.end());
    } catch(nlohmann::json::parse_error ex) {
        LOG_ERROR << "Can't deserialize mock data file: " << ex.what() << " content: " << dataStr;
        writeMockData();
    } catch(std::exception ex) {
        LOG_ERROR << "Can't deserialize mock data file: " << ex.what() << " content: " << dataStr;
        writeMockData();
    }

    is.close();
}
