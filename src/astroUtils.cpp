#include "astroUtils.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <sstream>

/*
float ParseTimeToRad(std::string time)
{
    std::string hour, minute, second;

    int i = 0;
    while(time.c_str()[i] < '0' && time.c_str()[i] > '9' && i < time.size()) {i++;}
    time.erase(0, i);
    i = 0;
    while(time.c_str()[i] >= '0' && time.c_str()[i] <= '9' && i < time.size()) {i++;}
    hour = time.substr(0, i+1);

    i = 0;
    while(time.c_str()[i] < '0' && time.c_str()[i] > '9' && i < time.size()) {i++;}
    time.erase(0, i);
    i = 0;
    while(time.c_str()[i] >= '0' && time.c_str()[i] <= '9' && i < time.size()) {i++;}
    minute = time.substr(0, i+1);

    i = 0;
    while(time.c_str()[i] < '0' && time.c_str()[i] > '9' && i < time.size()) {i++;}
    time.erase(0, i);
    i = 0;
    while(time.c_str()[i] >= '0' && time.c_str()[i] <= '9' && i < time.size()) {i++;}
    second = time.substr(0, i+1);

    float hours = std::stof(hour) + std::stof(minute)/60.0f + std::stof(second)/60.0f/60.0f;

    return hours / 24.0f * 2.0f * M_PI;
}*/

bool validateStringToDegrees(std::string str)
{
    std::istringstream iss(str);
    float f;
    iss >> str >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return !(iss.eof() && !iss.fail());
}

float stringToDegrees(std::string str)
{
    return std::stof(str);
}
