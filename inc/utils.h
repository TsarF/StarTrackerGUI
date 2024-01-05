#pragma once
#include "platform.h"

#include <string>
#include <vector>

void QuerySerialPorts(std::vector<std::string>& portnames);
bool FileExists(char* path);
