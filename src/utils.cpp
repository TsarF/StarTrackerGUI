#include "utils.h"

#include <fstream>
#include <memory>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#define WIM32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef MACOS
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/serial/ioss.h>
#include <IOKit/IOKitLib.h>
#endif

void QuerySerialPorts(std::vector<std::string>& portnames)
{
    #ifdef WIN32
    portnames.clear();
    unsigned long portNumbers[255];
    unsigned long portNumbersFound = 0;
    GetCommPorts(portNumbers, 255, &portNumbersFound);
    for(int i = 0; i < portNumbersFound; i++)
    {
        portnames.push_back(std::string("COM") + std::string(std::to_string(portNumbers[i])));
    }
    #endif
    #ifdef MACOS
    io_iterator_t serialPortIterator;
    kern_return_t result = IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching(kIOSerialBSDServiceValue), &serialPortIterator);

    if (result != KERN_SUCCESS)
    {
        std::cerr << "Error: Unable to get matching services." << std::endl;
        return
    }

    // Iterate over the available serial ports
    io_object_t serialPort;
    while ((serialPort = IOIteratorNext(serialPortIterator)) != 0)
    {
        // Get the path of the serial port
        CFStringRef devicePath = (CFStringRef)IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
        if (devicePath)
        {
            char path[PATH_MAX];
            CFStringGetCString(devicePath, path, sizeof(path), kCFStringEncodingUTF8);
            portnames.push_back(path);

            CFRelease(devicePath);
        }

        // Release the current serial port
        IOObjectRelease(serialPort);
    }

    // Release the iterator
    IOObjectRelease(serialPortIterator);
    #endif
    #ifdef UNIX
    throw std::logic_error(std::string(__FUNCTION_NAME__) + std::string(" function not yet implemented"));
    //TODO: Linux / MACOS implementation
    #endif
}

bool FileExists(char* path)
{
    std::ifstream file{path};
    return file.good();
}

std::string Matrix3ToString(Eigen::Matrix3<float> m, bool multiline)
{
    if (!multiline)
    {
        return string_format("{%.2f, %.2f, %.2f;   %.2f, %.2f, %.2f;   %.2f, %.2f, %.2f}", m.col(0)[0], m.col(0)[1], m.col(0)[2],
            m.col(1)[0], m.col(1)[1], m.col(1)[2],
            m.col(2)[0], m.col(2)[1], m.col(2)[2]);
    }
    else
    {
        return string_format("[%.2f, %.2f, %.2f]\n[%.2f, %.2f, %.2f]\n[%.2f, %.2f, %.2f]", m.col(0)[0], m.col(0)[1], m.col(0)[2],
            m.col(1)[0], m.col(1)[1], m.col(1)[2],
            m.col(2)[0], m.col(2)[1], m.col(2)[2]);
    }
}

std::string Vector3ToString(Eigen::Vector3<float> v)
{
    return string_format("{%.2f, %.2f, %.2f}", v[0], v[1], v[2]);
}

float r2df(float r) { return r / M_PI * 180.0f; }
float d2rf(float d) { return d * M_PI / 180.0f; }

Degrees GetDegrees(Radians r) { return r / M_PI * 180.0f; }
Radians GetRadians(Degrees d) { return d * M_PI / 180.0f; }

Eigen::Vector2f r2dv2f(Eigen::Vector2f r) { return Eigen::Vector2f(r2df(r[0]), r2df(r[1])); }
Eigen::Vector2f d2rv2f(Eigen::Vector2f d) { return Eigen::Vector2f(d2rf(d[0]), d2rf(d[1])); }

Eigen::Vector3f r2dv3f(Eigen::Vector3f r) { return Eigen::Vector3f(r2df(r[0]), r2df(r[1]), r2df(r[2])); }
Eigen::Vector3f d2rv3f(Eigen::Vector3f d) { return Eigen::Vector3f(d2rf(d[0]), d2rf(d[1]), d2rf(d[2])); }
