#pragma once

#include <cstdint>
#include <vector>

#include "serial/serial.h"
#include "Eigen/Dense"

#define CMD_ID_JOY_DATA 222
#define CMD_ID_SYSTEM_ACK 255
#define CMD_ID_SYSTEM_ACKR 254
#define CMD_ID_SYSTEM_ENTER_BOOT 200
#define CMD_ID_SYSTEM_FLASH_ERASE 201
#define CMD_ID_SYSTEM_FLASH_DATA 202
#define CMD_ID_READ_HWSTATE 100
#define CMD_ID_READ_CALIBRATION_MATRIX 101
#define CMD_ID_READ_INV_CALIBRATION_MATRIX 102
#define CMD_ID_READ_ALIGNMENT_MODE 103
#define CMD_ID_READ_APERTURE_AREA 104
#define CMD_ID_READ_APERTURE_DIAMETER 105
#define CMD_ID_READ_FOCAL_LENGTH 106
#define CMD_ID_READ_GUIDE_RATE_RA 107
#define CMD_ID_READ_GUIDE_RATE_DEC 108
#define CMD_ID_WRITE_CALIBRATION_MATRIX 151
#define CMD_ID_WRITE_INV_CALIBRATION_MATRIX 152
#define CMD_ID_WRITE_ALIGNMENT_MODE 153
#define CMD_ID_WRITE_APERTURE_AREA 154
#define CMD_ID_WRITE_APERTURE_DIAMETER 155
#define CMD_ID_WRITE_FOCAL_LENGTH 156
#define CMD_ID_WRITE_GUIDE_RATE_RA 157
#define CMD_ID_WRITE_GUIDE_RATE_DEC 158

#pragma pack(push, 1)
typedef struct
{
	uint16_t CRC16;
	uint8_t id;
	uint8_t commandID;
} Message_Header_t;

typedef struct
{
    Message_Header_t header;
    uint8_t data[64-sizeof(Message_Header_t)];
} Message_t;

struct HardwareState
{
    uint32_t firwareVersion;
    float time;
    float positionRA;
    float positionDEC;
    float speedRA;
    float speedDEC;
    float calibrationTransform[9];
    float inverseCalibrationTransform[9];
};

#pragma pack(pop)

namespace Packets
{
    Message_t Joystick(const float x, const float y, const float speed, const uint32_t buttons);
    Message_t ACK();
    Message_t GetHWState();
    Message_t EnterBootloader();
    Message_t EraseFlash();
    Message_t FirmwareChunk(uint8_t* data, uint8_t length);

    Message_t WriteCalibration(const Eigen::Matrix3f calib);
    Message_t WriteInvCalibration(const Eigen::Matrix3f inverse);

    void msgToData(char* data, const Message_t msg);
    void dataToMsg(Message_t& msg, const char* data);
};

void StartSerialQueue();
void StopSerialQueue();

uint8_t QueuePacket(Message_t& msg);
uint8_t SendPacket(serial::Serial& serial, Message_t& msg);
bool GetPacket(serial::Serial& serial, const uint8_t msg_id, Message_t* msg = nullptr);

void OnSerialReceive(const char* data, size_t size);
