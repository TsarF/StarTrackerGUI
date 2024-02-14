#include "mainWindow.h"
#include "platform.h"

#include "settingsWindow.h"
#include "common.h"
#include "utils.h"
#include "firmwareUpdateWindow.h"
#include "toshimiTaki.h"

#include <vector>
#include <exception>
#include <string>
#include <cmath>

std::vector<std::string> serialPorts{};

SettingsFrame* settingsFrame;
FirmwareUpdateFrame* firmwareUpdateFrame;

unsigned int updateTimerTicks = 0;

typedef struct
{
    float joySpeedX;
    float joySpeedY;
    float speedModifier;
    uint32_t joyButtons;
} Joystick_t;

Joystick_t joy;

PolarAlignmentTab::PolarAlignmentTab(wxNotebook* parent) : wxPanel(parent, wxID_ANY)
{
    wxFlexGridSizer* textEntryGrid = new wxFlexGridSizer(5, 4, wxSize(0, 0));
    wxStaticText* raLabel = new wxStaticText(this, wxID_ANY, "RA");
    wxStaticText* decLabel = new wxStaticText(this, wxID_ANY, "DEC");
    wxStaticText* star1Label = new wxStaticText(this, wxID_ANY, "Star 1");
    wxStaticText* star2Label = new wxStaticText(this, wxID_ANY, "Star 2");
    wxStaticText* star1ObservedLabel = new wxStaticText(this, wxID_ANY, "Observed Star 1");
    wxStaticText* star2ObservedLabel = new wxStaticText(this, wxID_ANY, "Observed Star 2");

    mainSizer = new wxBoxSizer(wxVERTICAL);
    star1RATextbox = new wxTextCtrl(this, wxID_ANY, "0.0");
    star1DECTextbox = new wxTextCtrl(this, wxID_ANY, "0.0");
    star1ObservedRATextbox = new wxTextCtrl(this, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    star1ObservedDECTextbox = new wxTextCtrl(this, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    star1AcquireButton = new wxButton(this, wxID_ANY, "Acquire");
    star1AcquireButton->Bind(wxEVT_BUTTON, &PolarAlignmentTab::OnStar1Acquire, this, wxID_ANY);
    star2RATextbox = new wxTextCtrl(this, wxID_ANY, "0.0");
    star2DECTextbox = new wxTextCtrl(this, wxID_ANY, "0.0");
    star2ObservedRATextbox = new wxTextCtrl(this, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    star2ObservedDECTextbox = new wxTextCtrl(this, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    star2AcquireButton = new wxButton(this, wxID_ANY, "Acquire");
    star2AcquireButton->Bind(wxEVT_BUTTON, &PolarAlignmentTab::OnStar2Acquire, this, wxID_ANY);

    //star1Sizer->Add(star1RATextbox, 0, wxALL | wxEXPAND, 3);
    //star1Sizer->Add(star1DECTextbox, 0, wxALL | wxEXPAND, 3);

    //star2Sizer->Add(star2RATextbox, 0, wxALL | wxEXPAND, 3);
    //star2Sizer->Add(star2DECTextbox, 0, wxALL | wxEXPAND, 3);

    textEntryGrid->Add(new wxStaticText(this, wxID_ANY, ""));
    textEntryGrid->Add(raLabel, 1, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(decLabel, 1, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(new wxStaticText(this, wxID_ANY, ""), 1, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star1Label, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 3);
    textEntryGrid->Add(star1RATextbox, 0, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star1DECTextbox, 0, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(new wxStaticText(this, wxID_ANY, ""), 1, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star1ObservedLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 3);
    textEntryGrid->Add(star1ObservedRATextbox, 0, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star1ObservedDECTextbox, 0, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star1AcquireButton, 0, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star2Label, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 3);
    textEntryGrid->Add(star2RATextbox, 0, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star2DECTextbox, 0, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(new wxStaticText(this, wxID_ANY, ""), 1, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star2ObservedLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 3);
    textEntryGrid->Add(star2ObservedRATextbox, 1, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star2ObservedDECTextbox, 1, wxALL | wxEXPAND, 3);
    textEntryGrid->Add(star2AcquireButton, 0, wxALL | wxEXPAND, 3);

    textEntryGrid->AddGrowableCol(1, 2);
    textEntryGrid->AddGrowableCol(2, 2);

    calibrateButton = new wxButton(this, wxID_ANY, "Calibrate");
    calibrateButton->Bind(wxEVT_BUTTON, &PolarAlignmentTab::OnCalibrate, this, wxID_ANY);

    mainSizer->Add(textEntryGrid, 0, wxALL | wxEXPAND, 3);
    mainSizer->Add(calibrateButton, 0, wxALL | wxEXPAND, 3);

    SetSizer(mainSizer);
}

void PolarAlignmentTab::OnStar1Acquire(wxCommandEvent& event)
{
    QueuePacket(Packets::GetHWState());
    GetPacket(g_serialPort);
    star1ObservedPosition[0] = g_HWstate.positionRA;
    star1ObservedPosition[1] = g_HWstate.positionDEC;
    star1Time = g_HWstate.time;

    star1ObservedRATextbox->Clear();
    star1ObservedRATextbox->SelectAll();
    star1ObservedRATextbox->WriteText(string_format("%.2f", star1ObservedPosition[0]));
    star1ObservedDECTextbox->Clear();
    star1ObservedDECTextbox->SelectAll();
    star1ObservedDECTextbox->WriteText(string_format("%.2f", star1ObservedPosition[1]));
}

void PolarAlignmentTab::OnStar2Acquire(wxCommandEvent& event)
{
    QueuePacket(Packets::GetHWState());
    GetPacket(g_serialPort);
    star2ObservedPosition[0] = g_HWstate.positionRA;
    star2ObservedPosition[1] = g_HWstate.positionDEC;
    star2Time = g_HWstate.time;

    star2ObservedRATextbox->Clear();
    star2ObservedRATextbox->SelectAll();
    star2ObservedRATextbox->WriteText(string_format("%.2f", star2ObservedPosition[0]));
    star2ObservedDECTextbox->Clear();
    star2ObservedDECTextbox->SelectAll();
    star2ObservedDECTextbox->WriteText(string_format("%.2f", star2ObservedPosition[1]));
}

void PolarAlignmentTab::OnCalibrate(wxCommandEvent& event)
{
    bool fail = false; // this is so that multiple message boxes can be shown upon one button press
    if (!validateStringToDegrees(star1RATextbox->GetValue().ToStdString()))
    {
        wxMessageBox("Star 1 RA: invalid input", "Error", wxICON_ERROR | wxOK);
        fail = true;
    }
    if (!validateStringToDegrees(star1DECTextbox->GetValue().ToStdString()))
    {
        wxMessageBox("Star 1 DEC: invalid input", "Error", wxICON_ERROR | wxOK);
        fail = true;
    }
    if (!validateStringToDegrees(star2RATextbox->GetValue().ToStdString()))
    {
        wxMessageBox("Star 2 RA: invalid input", "Error", wxICON_ERROR | wxOK);
        fail = true;
    }
    if (!validateStringToDegrees(star2DECTextbox->GetValue().ToStdString()))
    {
        wxMessageBox("Star 2 DEC: invalid input", "Error", wxICON_ERROR | wxOK);
        fail = true;
    }

    if (fail)
    {
        return;
    }

    g_calibrationData.star1[0] = d2rf(stringToDegrees(star1RATextbox->GetValue().ToStdString()));
    g_calibrationData.star1[1] = d2rf(stringToDegrees(star1DECTextbox->GetValue().ToStdString()));
    g_calibrationData.star1[2] = star1Time;

    g_calibrationData.star2[0] = d2rf(stringToDegrees(star2RATextbox->GetValue().ToStdString()));
    g_calibrationData.star2[1] = d2rf(stringToDegrees(star2DECTextbox->GetValue().ToStdString()));
    g_calibrationData.star2[2] = star2Time;

    g_calibrationData.star1Observed[0] = d2rf(star1ObservedPosition[0]);
    g_calibrationData.star1Observed[1] = d2rf(star1ObservedPosition[1]);

    g_calibrationData.star2Observed[0] = d2rf(star2ObservedPosition[0]);
    g_calibrationData.star2Observed[1] = d2rf(star2ObservedPosition[1]);

    g_calibrationData.isValid = true;


    ComputerPerfectTransform2Star(g_calibrationMatrix, g_HWstate.time, g_calibrationData.star1[2], g_calibrationData.star1.head<2>(), g_calibrationData.star1Observed.head<2>(),
        g_calibrationData.star2[2], g_calibrationData.star2.head<2>(), g_calibrationData.star2Observed.head<2>());
    
    g_inverseCalibrationMatrix = g_calibrationMatrix.inverse();
    QueuePacket(Packets::WriteCalibration(g_calibrationMatrix));
    QueuePacket(Packets::WriteInvCalibration(g_inverseCalibrationMatrix));
}

MosaicTab::MosaicTab(wxNotebook* parent) : wxPanel(parent, wxID_ANY)
{

}

DitheringTab::DitheringTab(wxNotebook* parent) : wxPanel(parent, wxID_ANY)
{

}

CameraControlTab::CameraControlTab(wxNotebook* parent) : wxPanel(parent, wxID_ANY)
{

}

void MainFrame::Build_Strip(wxMenuBar* menuBar)
{
    wxMenu *menuSettings = new wxMenu;
    menuSettings->Append(1, "&Connection");
    Bind(wxEVT_MENU, &MainFrame::OnSettings, this, 1);

    wxMenu* menuTools = new wxMenu;
    menuTools->Append(2, "&Firmware Update");
    Bind(wxEVT_MENU, &MainFrame::OnFirnmwareUpdate, this, 2);

    menuBar->Append(menuSettings, "&Settings");
    menuBar->Append(menuTools, "&Tools");
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Star Tracker Utility")
{
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnExit, this, wxID_ANY);
    //SetBackgroundColour(wxColour(255, 255, 255));

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
    StartSerialQueue();

    wxMenuBar* menuBar = new wxMenuBar;

    Build_Strip(menuBar);

    updateTimer = new wxTimer(this);
    Bind(wxEVT_TIMER, &MainFrame::OnUpdateTimer, this, updateTimer->GetId());
    updateTimer->Start(20);

    mainSizer = new wxBoxSizer(wxVERTICAL);
    
    gridSizer = new wxFlexGridSizer(2, 2, wxSize(3, 3));
    
    statusSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Status");
    for (int i = 0; i < statusLabels.size(); i++)
    {
        statusLabels[i] = new wxStaticText(this, wxID_ANY, "Status will be displayed here");
        statusSizer->Add(statusLabels[i], 0, wxALL | wxEXPAND, 3);
    }
    gridSizer->Add(statusSizer, 0, wxALL | wxEXPAND, 3);

    toolSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Device");

    serialComboBox = new wxComboBox(this, wxID_ANY, g_settings.serialPort.empty() ? "Serial Port" : g_settings.serialPort.c_str(), wxDefaultPosition, wxDefaultSize,
        wxArrayString(g_availableSerialPorts.size(), (wxString*)g_availableSerialPorts.data()), wxCB_READONLY);// wxSize(40, 10));

    serialComboBox->Bind(wxEVT_COMBOBOX, &MainFrame::OnSerialComboBox, this);
    serialComboBox->Bind(wxEVT_COMBOBOX_DROPDOWN, &MainFrame::OnSerialComboBoxDrop, this);
    connectButton = new wxButton(this, wxID_ANY, "Connect");
    connectButton->Bind(wxEVT_BUTTON, &MainFrame::OnConnect, this, wxID_ANY);
    wxBoxSizer* serialSizer = new wxBoxSizer(wxHORIZONTAL);
    serialSizer->Add(serialComboBox, 3, wxALL | wxEXPAND, 3);
    serialSizer->Add(connectButton, 1, wxALL | wxSHRINK, 3);
    QuerySerialPorts(g_availableSerialPorts);
    serialComboBox->Clear();
    for (int i = 0; i < g_availableSerialPorts.size(); i++)
    {
        serialComboBox->Append(g_availableSerialPorts[i].c_str());
    }
    serialComboBox->Refresh();
    serialComboBox->WriteText(g_settings.serialPort);

    toolSizer->Add(serialSizer, 0, wxALL | wxSHRINK, 3);
    gridSizer->Add(toolSizer, 0, wxALL | wxEXPAND, 3);

    utilitiesNotebook = new wxNotebook(this, wxID_ANY);
    polarAlignmentTab = new PolarAlignmentTab(utilitiesNotebook);
    mosaicTab = new MosaicTab(utilitiesNotebook);
    ditheringTab = new DitheringTab(utilitiesNotebook);
    cameraControlTab = new CameraControlTab(utilitiesNotebook);
    utilitiesNotebook->AddPage(polarAlignmentTab, "Polar Alignment");
    utilitiesNotebook->AddPage(mosaicTab, "Mosaic");
    utilitiesNotebook->AddPage(ditheringTab, "Dithering");
    utilitiesNotebook->AddPage(cameraControlTab, "Camera Control");

    utilitiesSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Utilities");
    utilitiesSizer->Add(utilitiesNotebook, 1, wxALL | wxEXPAND, 3);
    gridSizer->Add(utilitiesSizer, 0, wxALL | wxEXPAND, 3);

    gridSizer->AddGrowableRow(0, 1);
    gridSizer->AddGrowableRow(1, 3);
    gridSizer->AddGrowableCol(0, 3);
    gridSizer->SetFlexibleDirection(wxBOTH);

    mainSizer->Add(gridSizer, 1, wxALL | wxEXPAND, 3);
    SetSizer(mainSizer);
    SetMenuBar(menuBar);
    SetClientSize(1080, 640);
    Show();

    UpdateStatus();
}

void MainFrame::UpdateStatus()
{
    std::string statusStr;
    statusStr += "Device:\t";
    if (g_serialPort.isOpen())
    {
        statusStr += "Connected";
    }
    else
    {
        statusStr += "Disconnected";
    }
    statusLabels[0]->SetLabelText(wxString(statusStr));
    statusStr = "";

    statusStr += "Joystick:\t";
    if (wxWidgetsjoystick->IsOk())
    {
        statusStr += "Connected";
        QueuePacket(Packets::Joystick(joy.joySpeedX, joy.joySpeedY, joy.speedModifier, joy.joyButtons));
    }
    else
    {
        statusStr += "Disconnected";
    }
    statusLabels[1]->SetLabelText(wxString(statusStr));
    statusStr = "";

    statusStr += "RA:\t";
    statusStr += string_format("%.4f", g_HWstate.positionRA);
    statusLabels[2]->SetLabelText(wxString(statusStr));
    statusStr = "";

    statusStr += "DEC:\t";
    statusStr += string_format("%.4f", g_HWstate.positionDEC);
    statusLabels[3]->SetLabelText(wxString(statusStr));
    statusStr = "";

    statusStr += "Firmware version:\t";
    statusStr += string_format("%i.%i.%i build %i", GET_4BYTES(g_HWstate.firwareVersion));
    statusLabels[4]->SetLabelText(wxString(statusStr));
    statusStr = "";

    statusStr += "Software version:\t";
    statusStr += string_format("%i.%i.%i build %i", SOFTWARE_VERSION);
    statusLabels[5]->SetLabelText(wxString(statusStr));
    statusStr = "";

    statusSizer->Layout();
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
        QueuePacket(Packets::ACK());
        g_serialHeartbeatSeconds++;
        if (g_serialHeartbeatSeconds > SERIAL_TIMEOUT_SECONDS)
        {
            if (g_serialPort.isOpen())
            {
                g_serialPort.close();
            }
            g_calibrationData.isValid = false;
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
            QueuePacket(Packets::GetHWState());
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
    g_programShouldExit = true;
    StopSerialQueue();
    exit(0);
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
            QueuePacket(Packets::GetHWState());
            GetPacket(g_serialPort);
        }
    }
    catch (std::exception ex)
    {
        wxMessageBox(string_format("An exception occured: %s", ex.what()), "Error", wxICON_ERROR | wxOK);
        g_serialPort.close();
        return;
    }
}


void MainFrame::OnSettings(wxCommandEvent& event)
{
    settingsFrame = new SettingsFrame();
    settingsFrame->ShowModal();
    settingsFrame->Close();
    settingsFrame->Destroy();
}

void MainFrame::OnFirnmwareUpdate(wxCommandEvent& event)
{
    firmwareUpdateFrame = new FirmwareUpdateFrame();
    firmwareUpdateFrame->ShowModal();
    firmwareUpdateFrame->Close();
    firmwareUpdateFrame->Destroy();
}

void MainFrame::OnSerialComboBox(wxCommandEvent& event)
{
    int selectedIndex = serialComboBox->GetSelection();
    g_settings.serialPort = g_availableSerialPorts[selectedIndex];
    SaveSettings(g_settings);
}

void MainFrame::OnSerialComboBoxDrop(wxCommandEvent& event)
{
    QuerySerialPorts(g_availableSerialPorts);
    serialComboBox->Clear();
    for (int i = 0; i < g_availableSerialPorts.size(); i++)
    {
        serialComboBox->Append(g_availableSerialPorts[i].c_str());
    }
    serialComboBox->Refresh();
}
