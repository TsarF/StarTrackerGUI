#pragma once

#include <cstdint>

#pragma pack(1)
struct Packet
{
    uint8_t length;
    uint8_t family;
    uint8_t cmd_id;
    uint8_t data[252];
};

Packet JoystickPacket(float x, float y)

#pragma pack(pop)