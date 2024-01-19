#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "Eigen/Dense"
#include "serial/serial.h"
#include "settings.h"
#include "protocol.h"
#include "astroUtils.h"


#define SOFTWARE_VERSION 1,0,0,0

#define SERIAL_TIMEOUT_SECONDS 5

extern std::vector<std::string> g_availableSerialPorts;
extern serial::Serial g_serialPort;
extern unsigned int g_serialHeartbeatSeconds;
extern std::mutex g_serialMutex;

extern Settings g_settings;
extern Settings g_defaultSettings;

extern HardwareState g_HWstate;

extern Eigen::Matrix3f g_calibrationMatrix;
extern Eigen::Matrix3f g_inverseCalibrationMatrix;

extern CalibrationData_t g_calibrationData;
