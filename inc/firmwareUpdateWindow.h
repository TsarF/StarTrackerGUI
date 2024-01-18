#pragma once
#include <wx/wx.h>

class FirmwareUpdateFrame : public wxDialog
{
public:
    FirmwareUpdateFrame();

private:

    void OnUpload(wxCommandEvent& event);
};