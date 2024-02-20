#pragma once
#include <wx/wx.h>

class FirmwareUpdateFrame : public wxDialog
{
public:
    FirmwareUpdateFrame();

    wxTextCtrl* currentProgress;
private:

    void OnUpload(wxCommandEvent& event);
};