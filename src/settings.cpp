#include "settings.h"

#include "nlohmann/json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

void to_json(json& j, const Settings& s)
{
    j=json
    {
        {"serialPort", s.serialPort},
        {"serialBaudRate", s.serialBaudRate}
    };
}

void from_json(const json& j, Settings& s)
{
    s.serialPort = j["serialPort"];
    s.serialBaudRate = j["serialBaudRate"];
}

void SaveSettings(Settings& s)
{
    std::ofstream file{"settings.json"};
    json j = s;
    std::string dump = j.dump(4);
    file.write(dump.c_str(), dump.size());
    file.close();
}

void LoadSettings(Settings& s)
{
    std::ifstream file{"settings.json"};
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    json j = json::parse(ss.str());
    s=j;
}