#include "protocol.h"
#include "utils.h"
#include "common.h"

#include <thread>
#include <queue>
#include <utility>

std::thread serialQueueThread;
std::queue<Packet> serialQueue;
std::vector<uint8_t> packetWaitlist;
std::vector<Packet> packetLobby;

void SerialQueueFunction()
{
    while (!g_programShouldExit)
    {
        for (int i = 0; i < serialQueue.size(); i++)
        {
            SendPacket(g_serialPort, serialQueue.front());
            GetPacket(g_serialPort);
            serialQueue.pop();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void StartSerialQueue()
{
    serialQueueThread = std::thread{ SerialQueueFunction };
}

void StopSerialQueue()
{
    serialQueueThread.join();
}

void SendPacket(serial::Serial& serial, Packet& pkt)
{
    try
    {
        if (serial.isOpen())
        {
            uint8_t* data = new uint8_t[64];
            uint8_t* pktData = (uint8_t*)Packets::convert(pkt);
            memcpy(data, pktData, pkt.length + 3);
            if (pkt.length + 3 > 64)
            {
                throw;
            }
            serial.write(data, 64);
            delete[] data;
            delete pktData;
        }
    }
    catch (std::exception ex)
    {
        serial.close();
    }
}

void QueuePacket(const Packet& pkt)
{
    serialQueue.push(pkt);
}

bool GetPacket(serial::Serial& serial, Packet* pkt)
{
    try
    {
        if (serial.isOpen())
        {
            uint8_t buffer[64];
            serial.read(buffer, 64);
            OnSerialReceive((const char*)buffer, buffer[0] + 3);
            if (pkt != nullptr)
            {
                Packets::dataToPkt((const char*)buffer, *pkt);
            }
            return true;
        }
    }
    catch (std::exception ex)
    {
        serial.close();
        return false;
    }
    return true;
}

Packet Packets::Joystick(float x, float y, float speed, uint32_t buttons)
{
    return Packet{ 16, 100, 1, std::vector<uint8_t>{ GET_4BYTES(x), GET_4BYTES(y), GET_4BYTES(speed), GET_4BYTES(buttons)} };
}

Packet Packets::ACK()
{
    return Packet{ 0, 255, 255 };
}

Packet Packets::GetHWState()
{
    return Packet{ 0, CMD_FAMILY_READ, CMD_ID_READ_HWSTATE };
}

Packet Packets::EnterBootloader()
{
    return Packet{ 0, CMD_FAMILY_SYSTEM, CMD_ID_ENTER_BOOT };
}

Packet Packets::WriteCalibration(Eigen::Matrix3f calib)
{
    return Packet{ sizeof(float) * 9, CMD_FAMILY_WRITE, CMD_ID_WRITE_CALIBRATION_MATRIX,
    {
        GET_4BYTES(calib.row(0)[0]), GET_4BYTES(calib.row(0)[1]), GET_4BYTES(calib.row(0)[2]),
        GET_4BYTES(calib.row(1)[0]), GET_4BYTES(calib.row(1)[1]), GET_4BYTES(calib.row(1)[2]),
        GET_4BYTES(calib.row(2)[0]), GET_4BYTES(calib.row(2)[1]), GET_4BYTES(calib.row(2)[2])
    } };
}


Packet Packets::WriteInvCalibration(Eigen::Matrix3f inverse)
{
    return Packet{ sizeof(float) * 9, CMD_FAMILY_WRITE, CMD_ID_WRITE_INV_CALIBRATION_MATRIX,
    {
        GET_4BYTES(inverse.row(0)[0]), GET_4BYTES(inverse.row(0)[1]), GET_4BYTES(inverse.row(0)[2]),
        GET_4BYTES(inverse.row(1)[0]), GET_4BYTES(inverse.row(1)[1]), GET_4BYTES(inverse.row(1)[2]),
        GET_4BYTES(inverse.row(2)[0]), GET_4BYTES(inverse.row(2)[1]), GET_4BYTES(inverse.row(2)[2])
    } };
}

char* Packets::convert(Packet p)
{
    char* pkt = new char[p.length + 3];
    pkt[0] = p.length;
    pkt[1] = p.family;
    pkt[2] = p.cmd_id;

    for (int i = 0; i < p.length; i++)
    {
        pkt[3 + i] = p.data.data()[i];
    }
    return pkt;
}

void Packets::dataToPkt(const char* data, Packet& pkt)
{
    pkt = Packet{ ((uint8_t*)data)[0], ((uint8_t*)data)[1], ((uint8_t*)data)[2] };
    for (int i = 0; i < pkt.length; i++)
    {
        pkt.data.push_back(data[3 + i]);
    }
}

void ProcessReadCMD(Packet pkt)
{
    switch (pkt.cmd_id)
    {
        case CMD_ID_READ_HWSTATE:
            g_HWstate = (*(HardwareState*)pkt.data.data());
        break;
    }
}

void ProcessSystemCMD(Packet pkt)
{
    switch (pkt.cmd_id)
    {
    case CMD_ID_SYSTEM_ACKR:
        g_serialHeartbeatSeconds = 0;
        break;
    }
}

void OnSerialReceive(const char* data, size_t size)
{
    Packet pkt;
    Packets::dataToPkt(data, pkt);
    switch (pkt.family)
    {
    case CMD_FAMILY_SYSTEM:
        ProcessSystemCMD(pkt);
        break;
    case CMD_FAMILY_READ:
        ProcessReadCMD(pkt);
        break;
    }
}
