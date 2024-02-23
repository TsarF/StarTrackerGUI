#pragma once
#include <wx/wx.h>

class FirmwareUpdateFrame : public wxDialog
{
public:
    FirmwareUpdateFrame();

    wxStaticText* currentProgress;
    wxGauge* progressGauge;
private:

    void OnUpload(wxCommandEvent& event);
};