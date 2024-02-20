#include "firmwareUpdateWindow.h"
#include "common.h"
#include "protocol.h"
#include "serial/serial.h"

#include <istream>
#include <fstream>

FirmwareUpdateFrame::FirmwareUpdateFrame() : wxDialog(nullptr, wxID_ANY, "Firmware Update")
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxButton* uploadButton = new wxButton(this, wxID_ANY, "Upload");
    uploadButton->Bind(wxEVT_BUTTON, &FirmwareUpdateFrame::OnUpload, this, wxID_ANY);

    currentProgress = new wxTextCtrl(this, wxID_ANY, "Status\n");

    mainSizer->Add(uploadButton, 0, wxALL, 3);
    mainSizer->Add(currentProgress, 1, wxALL | wxEXPAND, 3);
    
    SetSizer(mainSizer);
}

void FirmwareUpdateFrame::OnUpload(wxCommandEvent& event)
{
    wxString filePath;
    wxFileDialog openFileDialog(this, _("Open File"), "", "", "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    // Show the file dialog
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        // If the user cancelled the dialog, just return an empty string
        filePath = "";
        return;
    }
    else
    {
        // Get the selected file path
        filePath = openFileDialog.GetPath();
    }

    //QueuePacket(Packets::EnterBootloader());
    //g_serialPort.close();

    //Sleep(3000);

    serial::Serial dfuSerial;
    dfuSerial.setPort(g_settings.serialPort);
    dfuSerial.setBaudrate(921600);
    dfuSerial.open();
    if (!dfuSerial.isOpen())
    {
        wxMessageBox("Unable to open COM for Firmware Update", "Error", wxICON_ERROR | wxOK);
        return;
    }

    std::ifstream fileStream;
    fileStream.open(filePath.c_str().AsWChar(), std::ifstream::binary | std::ifstream::in);
    SendPacket(dfuSerial, Packets::EraseFlash());
    currentProgress->WriteText("Erasing Flash\r\n");
    dfuSerial.read(64);
    currentProgress->WriteText("Erased Flash\r\n");
    int i = 0;
    do
    {
        char buf[60];
        memset(buf, 0, 60);
        fileStream.read(buf, 60);
        SendPacket(dfuSerial, Packets::FirmwareChunk((uint8_t*)buf, 60));
        currentProgress->WriteText("Writing chunk \r\n");
        dfuSerial.read(64);
        currentProgress->WriteText("Wrote chunk \r\n");
    } while (!fileStream.eof());
    fileStream.close();

    /* CUBE PROGRAMMER API (DID NOT WORK IN TESTING)
    dfuDeviceInfo* dfuDeviceList;
    int numDevices = getDfuDeviceList(&dfuDeviceList, 0xdf11, 0x0483);
    if (numDevices == 0)
    {
        wxMessageBox("No DFU devices found", "Error", wxICON_ERROR | wxOK);
        return;
    }
    int usbConnectFlag = connectDfuBootloader(dfuDeviceList[0].usbIndex);
    if (usbConnectFlag)
    {
        wxMessageBox("Unable to connect to DFU target device", "Error", wxICON_ERROR | wxOK);
        disconnect();
        return;
    }
    int downloadFileFlag = downloadFile(filePath.c_str(), 0x08000000, 0, 1, L"");
    if (downloadFileFlag != 0)
    {
        wxMessageBox("Unable to download firmware file", "Error", wxICON_ERROR | wxOK);
        disconnect();
        return;
    }
    disconnect();
    deleteInterfaceList();
*/
}
