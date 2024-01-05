#pragma once
#include <string>
#include <vector>
#include "serial.h"
#include "settings.h"


extern std::vector<std::string> g_availableSerialPorts;
extern CallbackAsyncSerial* g_serialPort;

extern Settings g_settings;
extern Settings g_defaultSettings;
