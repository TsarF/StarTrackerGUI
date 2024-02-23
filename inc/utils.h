#pragma once
#include "platform.h"
#include "Eigen/Dense"

#include <string>
#include <vector>
#include <memory>

#define GET_4BYTES(A) ((uint8_t*)&(A))[0], ((uint8_t*)&(A))[1], ((uint8_t*)&(A))[2], ((uint8_t*)&(A))[3]

#ifdef _DEBUG
#define USE(A) assert(&A != 0)
#else
#define USE(A)
#endif

#define time_now std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

void QuerySerialPorts(std::vector<std::string>& portnames);
bool FileExists(const char* path);
std::string Matrix3ToString(Eigen::Matrix3<float> m, bool multiline = false);
std::string Vector3ToString(Eigen::Vector3<float> v);

float r2df(float r);
float d2rf(float d);

typedef float Radians;
typedef float Degrees;

Degrees GetDegrees(Radians r);
Radians GetRadians(Degrees d);

Eigen::Vector2f r2dv2f(Eigen::Vector2f r);
Eigen::Vector2f d2rv2f(Eigen::Vector2f d);

Eigen::Vector3f r2dv3f(Eigen::Vector3f r);
Eigen::Vector3f d2rv3f(Eigen::Vector3f d);
