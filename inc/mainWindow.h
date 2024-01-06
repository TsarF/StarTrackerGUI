#pragma once
#include <wx/wx.h>
#include <wx/joystick.h>

class MainFrame : public wxFrame
{
public:
    MainFrame();

    void UpdateStatus();
    void UpdateJoystick();

private:
    wxTimer* updateTimer;
    
    wxJoystick* wxWidgetsjoystick;

    wxBoxSizer* mainSizer;
    wxGridSizer* gridSizer;
    wxStaticBoxSizer* statusSizer;
    wxStaticText* statusText;

    wxStaticBoxSizer* toolSizer;
    wxButton* connectButton;

    void OnUpdateTimer(wxTimerEvent& event);
    void OnJoystick(wxJoystickEvent& event);
    void OnExit(wxCloseEvent& event);
    void OnSettings(wxCommandEvent& event);

    void OnConnect(wxCommandEvent& event);
    
    void Build_Strip(wxMenuBar* menuBar);
};