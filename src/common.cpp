#include "common.h"

std::vector<std::string> g_availableSerialPorts{};
serial::Serial g_serialPort{};
unsigned int g_serialHeartbeatSeconds = 0;
std::mutex g_serialMutex{};

Settings g_settings;
Settings g_defaultSettings = 
{
    "",
    921600
};

HardwareState g_HWstate{};

Eigen::Matrix3f g_calibrationMatrix;
Eigen::Matrix3f g_inverseCalibrationMatrix;

CalibrationData_t g_calibrationData;
