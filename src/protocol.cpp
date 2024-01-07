#include "protocol.h"
#include "utils.h"
#include "common.h"

void SendPacket(CallbackAsyncSerial* serial, Packet& pkt)
{
    if (serial->isOpen())
    {
        char* data = Packets::convert(pkt);
        serial->write(data, pkt.length + 3);
        delete data;
    }
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
    Packet pkt = Packet{ ((uint8_t*)data)[0], ((uint8_t*)data)[1], ((uint8_t*)data)[2] };
    for (int i = 0; i < pkt.length; i++)
    {
        pkt.data.push_back(data[3 + i]);
    }

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
