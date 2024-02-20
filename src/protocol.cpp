#include "protocol.h"
#include "utils.h"
#include "common.h"

#include <thread>
#include <queue>
#include <utility>
#include <chrono>

#define time_now std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

const int NUM_RETRIES = 5;

std::thread serialSenderThread;
std::queue<Message_t> serialOutgoingQueue;

std::vector<std::pair<unsigned long long, Message_t>> packetLobby;
uint8_t packet_id = 0;
uint8_t erased_packets = 0;
uint8_t received_packets = 0;
uint32_t receivedByted = 0;
#define CRC16_POLY 0x8005

uint16_t crc16(const uint8_t *data, uint16_t size)
{
	uint16_t out = 0;
	int bits_read = 0, bit_flag;

	/* Sanity check: */
	if(data == NULL)
			return 0;

	while(size > 0)
	{
			bit_flag = out >> 15;

			/* Get next bit: */
			out <<= 1;
			out |= (*data >> bits_read) & 1; // item a) work from the least significant bits

			/* Increment bit counter: */
			bits_read++;
			if(bits_read > 7)
			{
					bits_read = 0;
					data++;
					size--;
			}

			/* Cycle check: */
			if(bit_flag)
					out ^= CRC16_POLY;

	}

	// item b) "push out" the last 16 bits
	int i;
	for (i = 0; i < 16; ++i) {
			bit_flag = out >> 15;
			out <<= 1;
			if(bit_flag)
					out ^= CRC16_POLY;
	}

	// item c) reverse the bits
	uint16_t crc = 0;
	i = 0x8000;
	int j = 0x0001;
	for (; i != 0; i >>=1, j <<= 1) {
			if (i & out) crc |= j;
	}

	return crc;
}

void SerialQueueFunction()
{
    while (!g_programShouldExit)
    {
        if(g_serialPort.isOpen())
        {
            for (int i = 0; i < serialOutgoingQueue.size(); i++)
            {
                Message_t msg = serialOutgoingQueue.front();
                Message_t response;
                uint8_t retries = 0;
                do
                {
                    SendPacket(g_serialPort, msg);
                    uint8_t buffer[64];
                    unsigned long long startTime = time_now;
                    while (startTime + 1000 > time_now)
                    {
                        volatile uint8_t b = g_serialPort.available();
                        if (b >= 64)
                        {
                            uint8_t bytesRead = g_serialPort.read((uint8_t*)buffer, 64);
                            receivedByted += bytesRead;

                            if (bytesRead == 64)
                            {
                                received_packets++;
                                OnSerialReceive((const char*)buffer, bytesRead);
                                Packets::dataToMsg(response, (char*)buffer);
                                packetLobby.push_back(std::pair<unsigned long long, Message_t>(0, response));
                                retries = NUM_RETRIES;
                                break;
                            }
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    retries++;
                } while (retries < NUM_RETRIES);

                serialOutgoingQueue.pop();
            }
        }
        for (int i = 0; i < packetLobby.size(); i++)
        {
            packetLobby[i].first++;
            if (packetLobby[i].first >= 5000)
            {
                packetLobby.erase(packetLobby.begin() + i);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void StartSerialQueue()
{
    serialSenderThread = std::thread{ SerialQueueFunction };
}

void StopSerialQueue()
{
    serialSenderThread.join();
}

uint8_t SendPacket(serial::Serial& serial, Message_t& msg)
{
    try
    {
        if (serial.isOpen())
        {
            uint8_t* data = new uint8_t[64];
            msg.header.CRC16 = crc16((uint8_t*)(&msg)+2, sizeof(Message_t)-2);
            Packets::msgToData((char*)data, msg);
            serial.write(data, 64);
            delete[] data;
        }
    }
    catch (std::exception ex)
    {
        serial.close();
    }
    return 0;
}

uint8_t QueuePacket(Message_t& msg)
{
    packet_id++;
    msg.header.id = packet_id;
    serialOutgoingQueue.push(msg);
    return packet_id;
}

bool GetPacket(serial::Serial& serial, const uint8_t msg_id, Message_t* msg)
{
    try
    {
        unsigned long long startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        while(serial.isOpen() && startTime + 5000 > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
        {
            for(int i = 0; i < packetLobby.size(); i++)
            {
                if(packetLobby[i].second.header.id == msg_id)
                {
                    if(msg != nullptr)
                    {
                        memcpy(msg, &(packetLobby[i]), sizeof(Message_t));
                    }
                    packetLobby.erase(packetLobby.begin() + i);
                    erased_packets++;
                    return true;
                }
            }
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return false;
        /*
        if (serial.isOpen())
        {
            uint8_t buffer[64];
            uint8_t bytesRead = serial.read((uint8_t*)buffer, 64);
            OnSerialReceive((const char*)buffer, bytesRead);
            if (msg != nullptr)
            {
                Packets::dataToMsg(*msg, (const char*)buffer);
            }
            return true;
        }
        */
    }
    catch (std::exception ex)
    {
        serial.close();
        return false;
    }
    return true;
}

Message_t Packets::Joystick(const float x, const float y, const float speed, const uint32_t buttons)
{
    return Message_t{ {0, 0, CMD_ID_JOY_DATA}, {GET_4BYTES(x), GET_4BYTES(y), GET_4BYTES(speed), GET_4BYTES(buttons)}};
}

Message_t Packets::ACK()
{
    return Message_t{ {0, 0, CMD_ID_SYSTEM_ACK}, {} };
}

Message_t Packets::GetHWState()
{
    return Message_t{ {0, 0, CMD_ID_READ_HWSTATE}, {} };
}

Message_t Packets::EnterBootloader()
{
    return Message_t{ {0, 0, CMD_ID_SYSTEM_ENTER_BOOT}, {} };
}

Message_t Packets::EraseFlash()
{
    return Message_t{ {0, 0, CMD_ID_SYSTEM_FLASH_ERASE}, {} };
}

Message_t Packets::FirmwareChunk(uint8_t* data, uint8_t length)
{
    Message_t ret{ {0, 0, CMD_ID_SYSTEM_FLASH_DATA}, {} };
    memcpy(ret.data, data, length);
    return ret;
}

Message_t Packets::WriteCalibration(const Eigen::Matrix3f calib)
{
    return Message_t{ {0, 0, CMD_ID_WRITE_CALIBRATION_MATRIX},
    {
        GET_4BYTES(calib(0, 0)), GET_4BYTES(calib(0, 1)), GET_4BYTES(calib(0, 2)),
        GET_4BYTES(calib(1, 0)), GET_4BYTES(calib(1, 1)), GET_4BYTES(calib(1, 2)),
        GET_4BYTES(calib(2, 0)), GET_4BYTES(calib(2, 1)), GET_4BYTES(calib(2, 2))
    } };
}


Message_t Packets::WriteInvCalibration(const Eigen::Matrix3f inverse)
{
    return Message_t{ {0, 0, CMD_ID_WRITE_INV_CALIBRATION_MATRIX},
    {
        GET_4BYTES(inverse(0, 0)), GET_4BYTES(inverse(0, 1)), GET_4BYTES(inverse(0, 2)),
        GET_4BYTES(inverse(1, 0)), GET_4BYTES(inverse(1, 1)), GET_4BYTES(inverse(1, 2)),
        GET_4BYTES(inverse(2, 0)), GET_4BYTES(inverse(2, 1)), GET_4BYTES(inverse(2, 2))
    } };
}

void Packets::msgToData(char* data, const Message_t msg)
{
    if(data != nullptr)
    {
        memcpy(data, (void*)(&msg), sizeof(Message_t));
    }
    else
    {
        throw;
    }
}

void Packets::dataToMsg(Message_t& msg, const char* data)
{
    msg = *(Message_t*)data;
}

void ProcessCMD(Message_t msg)
{
    switch (msg.header.commandID)
    {
        case CMD_ID_READ_HWSTATE:
            g_HWstate = (*(HardwareState*)msg.data);
            
        break;
        case CMD_ID_SYSTEM_ACKR:
            g_serialHeartbeatSeconds = 0;
        break;

    }
}

void OnSerialReceive(const char* data, size_t size)
{
    Message_t msg;
    Packets::dataToMsg(msg, data);
    ProcessCMD(msg);
}
