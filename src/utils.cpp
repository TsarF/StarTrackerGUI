#include "utils.h"

#include <fstream>

#ifdef WIN32
#define WIM32_LEAN_AND_MEAN
#include <windows.h>
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