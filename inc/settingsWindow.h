#pragma once
#include "wx/wx.h"
#include "wx/notebook.h"

class ConnectionTab : public wxPanel
{
    public:
    ConnectionTab(wxNotebook* parent);

    private:
    wxComboBox* serialComboBox;

    void OnSerialComboBox(wxCommandEvent& event);
    void OnTestSerialConnectionButton(wxCommandEvent& event);
    void OnRefreshSerialListButton(wxCommandEvent& event);

    void RefreshSerial();
};

class AppTab : public wxPanel
{
    public:
    AppTab(wxNotebook* parent);
};

class SettingsFrame : public wxDialog
{
    public:
    SettingsFrame(int page = 0);

    private:
    wxNotebook* notebook;

    void OnExit(wxCloseEvent& event);

    void OnNext(wxCommandEvent& event);
    void OnBack(wxCommandEvent& event);
};