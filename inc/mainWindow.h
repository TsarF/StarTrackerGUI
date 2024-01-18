#pragma once
#include <wx/wx.h>
#include <wx/joystick.h>
#include "serial/serial.h"
#include "wx/notebook.h"

class PolarAlignmentTab : public wxPanel
{
public: 
    PolarAlignmentTab(wxNotebook* parent);

private:
    wxBoxSizer* mainSizer;
    wxBoxSizer* star1Sizer;
    wxTextCtrl* star1RATextbox;
    wxTextCtrl* star1DECTextbox;
    wxBoxSizer* star2Sizer;
    wxTextCtrl* star2RATextbox;
    wxTextCtrl* star2DECTextbox;
};

class MosaicTab : public wxPanel
{
public:
    MosaicTab(wxNotebook* parent);
};


class DitheringTab : public wxPanel
{
public:
    DitheringTab(wxNotebook* parent);
};


class CameraControlTab : public wxPanel
{
public:
    CameraControlTab(wxNotebook* parent);
};


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
    std::vector<wxStaticText*> statusLabels{ 6 };

    wxStaticBoxSizer* toolSizer;
    wxButton* connectButton;
    wxComboBox* serialComboBox;

    wxStaticBoxSizer* utilitiesSizer;
    wxNotebook* utilitiesNotebook;
    PolarAlignmentTab* polarAlignmentTab;
    MosaicTab* mosaicTab;
    DitheringTab* ditheringTab;
    CameraControlTab* cameraControlTab;

    void UpdateStatus();
    void UpdateJoystick();
    void SerialHeartbeat();

    void OnUpdateTimer(wxTimerEvent& event);
    void OnJoystick(wxJoystickEvent& event);
    void OnExit(wxCloseEvent& event);
    void OnSerialTimeout(serial::Serial& serial);

    void OnConnect(wxCommandEvent& event);

    void OnSettings(wxCommandEvent& event);
    void OnFirnmwareUpdate(wxCommandEvent& event);
    void OnSerialComboBox(wxCommandEvent& event);
    void OnSerialComboBoxDrop(wxCommandEvent& event);

    void Build_Strip(wxMenuBar* menuBar);
};