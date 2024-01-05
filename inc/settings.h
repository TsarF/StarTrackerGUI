#pragma once
#include <string>

struct Settings
{
    std::string serialPort;
    int serialBaudRate;
};

void SaveSettings(Settings& s);
void LoadSettings(Settings& s);
