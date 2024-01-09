#include "common.h"

std::vector<std::string> g_availableSerialPorts{};
serial::Serial g_serialPort{};
unsigned int g_serialHeartbeatSeconds = 0;
std::mutex g_serialMutex{};

Settings g_settings;
Settings g_defaultSettings = 
{
    "",
    115200
};

HardwareState g_HWstate{};
