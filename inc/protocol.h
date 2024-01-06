#pragma once

#include <cstdint>
#include <vector>

#define CMD_FAMILY_SYSTEM 255
#define CMD_ID_SYSTEM_ACK 255
#define CMD_ID_SYSTEM_ACKR 254

#define CMD_FAMILY_READ 1
#define CMD_ID_READ_ALIGNMENT_MODE 1
#define CMD_ID_READ_APERTURE_AREA 2
#define CMD_ID_READ_APERTURE_DIAMETER 3
#define CMD_ID_READ_FOCAL_LENGTH 4
#define CMD_ID_READ_GUIDE_RATE_RA 5
#define CMD_ID_READ_GUIDE_RATE_DEC 6
#define CMD_ID_READ_HWSTATE 255

#define CMD_FAMILY_WRITE 2
#define CMD_ID_WRITE_ALIGNMENT_MODE 1
#define CMD_ID_WRITE_APERTURE_AREA 2
#define CMD_ID_WRITE_APERTURE_DIAMETER 3
#define CMD_ID_WRITE_FOCAL_LENGTH 4
#define CMD_ID_WRITE_GUIDE_RATE_RA 5
#define CMD_ID_WRITE_GUIDE_RATE_DEC 6
#define CMD_ID_WRITE_CORRECTION_FACTOR 255
#define CMD_ID_WRITE_CORRECTION_PHASE 254

#define CMD_FAMILY_CONFIG 3

#define CMD_FAMILY_JOY 100
#define CMD_ID_JOY_DATA 1

#pragma pack(push, 1)
struct Packet
{
    uint8_t length;
    uint8_t family;
    uint8_t cmd_id;
    std::vector<uint8_t> data;
};

struct HardwareState
{
    uint32_t firwareVersion;
    float time;
    float positionRA;
    float positionDEC;
    float speedRA;
    float speedDEC;
    float calibrationTransform[9];
};

#pragma pack(pop)

namespace Packets
{
    Packet Joystick(float x, float y, float speed, uint32_t buttons);
    Packet ACK();
    Packet GetHWState();

    char* convert(Packet p);
};

void OnSerialReceive(const char* data, size_t size);
