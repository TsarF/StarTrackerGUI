#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <serial.h>
#include "settings.h"
#include "protocol.h"


#define SOFTWARE_VERSION 1,0,0,0

#define SERIAL_TIMEOUT_SECONDS 5

extern std::vector<std::string> g_availableSerialPorts;
extern CallbackAsyncSerial* g_serialPort;
extern unsigned int g_serialHeartbeatSeconds;
extern std::mutex g_serialMutex;

extern Settings g_settings;
extern Settings g_defaultSettings;

extern HardwareState g_HWstate;
