#pragma once
#include <wx/wx.h>

class MainFrame : public wxFrame
{
    public:
    MainFrame();

    void UpdateStatus();

    private:
    wxStaticBoxSizer* statusSizer;
    wxStaticText* statusText;

    void OnExit(wxCloseEvent& event);
    void OnSettings(wxCommandEvent &event);

    void Build_Strip(wxMenuBar* menuBar);
};