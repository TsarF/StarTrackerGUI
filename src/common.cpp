#include "common.h"

std::vector<std::string> g_availableSerialPorts{};
CallbackAsyncSerial* g_serialPort;

Settings g_settings;
Settings g_defaultSettings = 
{
    "",
    115200
};
