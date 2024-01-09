#include "mainWindow.h"
#include "platform.h"

#include "settingsWindow.h"
#include "common.h"
#include "utils.h"

#include <vector>
#include <exception>
#include <string>
#include <cmath>

std::vector<std::string> serialPorts{};

SettingsFrame* settingsFrame;

unsigned int updateTimerTicks = 0;

typedef struct
{
    float joySpeedX;
    float joySpeedY;
    float speedModifier;
    uint32_t joyButtons;
} Joystick_t;

Joystick_t joy;

void MainFrame::Build_Strip(wxMenuBar *menuBar)
{
    wxMenu *menuSettings = new wxMenu;

    menuSettings->Append(0, "&Connection");
    Bind(wxEVT_MENU, &MainFrame::OnSettings, this, 0);



    menuBar->Append(menuSettings, "&Settings");
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Star Tracker Utility")
{
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnExit, this, wxID_ANY);
    SetBackgroundColour(wxColour(240, 240, 240));

    wxWidgetsjoystick = new wxJoystick();

    Bind(wxEVT_JOY_MOVE, &MainFrame::OnJoystick, this, wxID_ANY);
    Bind(wxEVT_JOY_ZMOVE, &MainFrame::OnJoystick, this, wxID_ANY);
    Bind(wxEVT_JOY_BUTTON_UP, &MainFrame::OnJoystick, this, wxID_ANY);
    Bind(wxEVT_JOY_BUTTON_DOWN, &MainFrame::OnJoystick, this, wxID_ANY);

    if (!FileExists("settings.json"))
    {
        SaveSettings(g_defaultSettings); // save default settings file if the settings file does not exist
    }
    LoadSettings(g_settings);

    g_serialPort.setPort(g_settings.serialPort);
    g_serialPort.setTimeout(1000, 1000, 1, 1000, 1);
    g_serialPort.setBaudrate(g_settings.serialBaudRate);

    wxMenuBar* menuBar = new wxMenuBar;

    Build_Strip(menuBar);

    updateTimer = new wxTimer(this);
    Bind(wxEVT_TIMER, &MainFrame::OnUpdateTimer, this, updateTimer->GetId());
    updateTimer->Start(20);

    mainSizer = new wxBoxSizer(wxVERTICAL);
    
    gridSizer = new wxGridSizer(2, 2, wxSize(3, 3));
    
    statusSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Status");
    statusText = new wxStaticText(this, wxID_ANY, "Status will be displayed here");
    gridSizer->Add(statusSizer, 0, wxALL | wxEXPAND, 3);
    statusSizer->Add(statusText, 0, wxALL | wxEXPAND, 3);

    toolSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Device");
    connectButton = new wxButton(this, wxID_ANY, "Connect");
    connectButton->Bind(wxEVT_BUTTON, &MainFrame::OnConnect, this, wxID_ANY);
    gridSizer->Add(toolSizer, 0, wxALL | wxEXPAND, 3);
    toolSizer->Add(connectButton, 0, wxALL | wxEXPAND, 3);

    mainSizer->Add(gridSizer, 0, wxALL | wxEXPAND, 3);
    SetSizer(mainSizer);
    SetMenuBar(menuBar);
    SetClientSize(720, 640);
    Show();

    UpdateStatus();
}

void MainFrame::UpdateStatus()
{
    std::string statusStr;
    statusStr += "Device:\t";
    if (g_serialPort.isOpen())
    {
        statusStr += "Connected\n";
    }
    else
    {
        statusStr += "Disconnected\n";
    }

    statusStr += "Joystick:\t";
    if (wxWidgetsjoystick->IsOk())
    {
        statusStr += "Connected\n";
        SendPacket(g_serialPort, Packets::Joystick(joy.joySpeedX, joy.joySpeedY, joy.speedModifier, joy.joyButtons));
    }
    else
    {
        statusStr += "Disconnected\n";
    }

    statusStr += "RA:\t";
    statusStr += string_format("%.4f\n", g_HWstate.positionRA);
    statusStr += "DEC:\t";
    statusStr += string_format("%.4f\n", g_HWstate.positionDEC);

    statusStr += "\nFirmware version:\t";
    statusStr += string_format("%i.%i.%i build %i\n", GET_4BYTES(g_HWstate.firwareVersion));

    statusStr += "Software version:\t";
    statusStr += string_format("%i.%i.%i build %i\n", SOFTWARE_VERSION);

    statusText->SetLabelText(wxString(statusStr));
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

float DeadZone(float input, float zone)
{
    if (abs(input) > zone)
    {
        return (input - sgn(input) * zone) / (1.0f - zone);
    }
    else
    {
        return 0.0f;
    }
}

void MainFrame::UpdateJoystick()
{
    if (!wxWidgetsjoystick->IsOk())
    {
        delete wxWidgetsjoystick;
        wxWidgetsjoystick = new wxJoystick();
    }
    else
    {
        joy.joySpeedX = DeadZone((wxWidgetsjoystick->GetPosition(0) - 0x8000) / (float)0x8000, 0.05f);
        joy.joySpeedY = DeadZone((wxWidgetsjoystick->GetPosition(1) - 0x8000) / (float)0x8000, 0.05f);
        joy.speedModifier = (DeadZone((wxWidgetsjoystick->GetPosition(2) - 0x8000) / (float)0x8000, 0.05f) > 0.0f) ?
            DeadZone((wxWidgetsjoystick->GetPosition(2) - 0x8000) / (float)0x8000, 0.05f) : 0.0f;
        joy.joyButtons = wxWidgetsjoystick->GetButtonState();
    }
}

void MainFrame::SerialHeartbeat()
{
    try
    {
        SendPacket(g_serialPort, Packets::ACK());
        GetPacket(g_serialPort);
        g_serialHeartbeatSeconds++;
        if (g_serialHeartbeatSeconds > SERIAL_TIMEOUT_SECONDS)
        {
            if (g_serialPort.isOpen())
            {
                g_serialPort.close();
            }
        }
    }
    catch (std::exception ex)
    {
        wxMessageBox(string_format("An exception occured: %s", ex.what()), "Error", wxICON_ERROR | wxOK);
        g_serialPort.close();
        return;
    }
}

void MainFrame::OnUpdateTimer(wxTimerEvent& event)
{
    updateTimerTicks++;
    UpdateJoystick();
    UpdateStatus();
    if (!(updateTimerTicks % 50))
    {
        if (g_serialPort.isOpen())
        {
            SerialHeartbeat();
            SendPacket(g_serialPort, Packets::GetHWState());
            GetPacket(g_serialPort);
        }
    }
}

void MainFrame::OnJoystick(wxJoystickEvent& event)
{
    joy.joySpeedX = event.GetPosition().x;
    joy.joySpeedY = event.GetPosition().y;
    joy.speedModifier = event.GetZPosition();
}

void MainFrame::OnExit(wxCloseEvent& event)
{
    Destroy();
    event.Skip();
    updateTimer->Stop();
    exit(0);
}

void MainFrame::OnSettings(wxCommandEvent &event)
{
    settingsFrame = new SettingsFrame();
    settingsFrame->ShowModal();
    settingsFrame->Close();
    settingsFrame->Destroy();
}

void MainFrame::OnConnect(wxCommandEvent& event)
{
    try
    {
        if (!g_serialPort.isOpen())
        {
            if (g_settings.serialPort.empty())
            {
                wxMessageBox("Please select a serial port in Settings -> Connection", "Error", wxICON_ERROR | wxOK);
                return;
            }
            g_serialPort.setPort(g_settings.serialPort);
            g_serialPort.open();
            if (!g_serialPort.isOpen())
            {
                wxMessageBox("Failed opening serial port", "Error", wxICON_ERROR | wxOK);
                g_serialPort.close();
                return;
            }
            g_serialHeartbeatSeconds = 0;
        }
    }
    catch (std::exception ex)
    {
        wxMessageBox(string_format("An exception occured: %s", ex.what()), "Error", wxICON_ERROR | wxOK);
        g_serialPort.close();
        return;
    }
}

