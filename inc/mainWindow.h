#pragma once
#include <wx/wx.h>
#include <wx/joystick.h>
#include "serial/serial.h"

class MainFrame : public wxFrame
{
public:
    MainFrame();

private:
    wxTimer* updateTimer;
    
    wxJoystick* wxWidgetsjoystick;

    wxBoxSizer* mainSizer;
    wxGridSizer* gridSizer;
    wxStaticBoxSizer* statusSizer;
    wxStaticText* statusText;

    wxStaticBoxSizer* toolSizer;
    wxButton* connectButton;

    void UpdateStatus();
    void UpdateJoystick();
    void SerialHeartbeat();

    void OnUpdateTimer(wxTimerEvent& event);
    void OnJoystick(wxJoystickEvent& event);
    void OnExit(wxCloseEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnSerialTimeout(serial::Serial& serial);

    void OnConnect(wxCommandEvent& event);
    
    void Build_Strip(wxMenuBar* menuBar);
};