#include "firmwareUpdateWindow.h"
#include "common.h"
#include "protocol.h"
#include "serial/serial.h"
#include "utils.h"

#include <istream>
#include <fstream>
#include <thread>

FirmwareUpdateFrame::FirmwareUpdateFrame() : wxDialog(nullptr, wxID_ANY, "Firmware Update")
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxButton* uploadButton = new wxButton(this, wxID_ANY, "Upload");
    uploadButton->Bind(wxEVT_BUTTON, &FirmwareUpdateFrame::OnUpload, this, wxID_ANY);

    currentProgress = new wxStaticText(this, wxID_ANY, "Status: ");

    progressGauge = new wxGauge(this, wxID_ANY, 100);

    mainSizer->Add(uploadButton, 0, wxALL, 3);
    mainSizer->Add(currentProgress, 0, wxALL | wxEXPAND, 3);
    mainSizer->Add(progressGauge, 0, wxALL | wxEXPAND, 3);
    
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
    if(g_serialPort.isOpen())
    {
        Message_t msg = Packets::EnterBootloader();
        uint8_t msg_id = QueuePacket(msg);
        GetPacket(g_serialPort, msg_id);
        g_serialPort.close();

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    serial::Serial dfuSerial;
    dfuSerial.setPort(g_settings.serialPort);
    dfuSerial.setBaudrate(921600);
    dfuSerial.setTimeout(1000, 1000, 50, 1000, 50);
    dfuSerial.open();
    if (!dfuSerial.isOpen())
    {
        wxMessageBox("Unable to open COM for firmware update", "Error", wxICON_ERROR | wxOK);
        dfuSerial.close();
        return;
    }

    std::ifstream fileStream;
    fileStream.open(filePath.c_str(), std::ifstream::binary | std::ifstream::in);
    fileStream.ignore( std::numeric_limits<std::streamsize>::max() );
    uint32_t fileSize = fileStream.gcount();
    fileStream.clear();   //  Since ignore will have set eof.
    fileStream.seekg( 0, std::ios_base::beg );
    uint32_t totalChunks = std::ceilf((((float)fileSize)/56.0f))-1;
    uint8_t buffer[64];
    uint8_t retries = 0;
    uint16_t progress = 0;
    uint8_t bytesRead = 0;
    do
    {
        currentProgress->SetLabelText("Status: Erasing Flash...");
        Message_t msg = Packets::EraseFlash();
        SendPacket(dfuSerial, msg);
        unsigned long long startTime = time_now;
        while (dfuSerial.available() < 64 && startTime + 20000 > time_now) { std::this_thread::yield(); }
        bytesRead = dfuSerial.read(buffer, 64);
        retries++;
    } while (bytesRead < 64 && retries < 20);
    if(bytesRead < 64)
    {
        wxMessageBox("Unable to erase flash", "Error", wxICON_ERROR | wxOK);
        dfuSerial.close();
        fileStream.close();
        return;
    }
    retries = 0;
    bytesRead = 0;
    currentProgress->SetLabelText("Status: Flash Erased");
    progress += 200;
    progressGauge->SetValue((progress) * (100.0f/((float)200.0f)));
    uint32_t chunkNum = 0;
    do
    {
        char buf[56];
        memset(buf, 0, 56);
        fileStream.read(buf, 56);
        do
        {
            currentProgress->SetLabelText(string_format("Status: Writing Chunk %i of %i...", chunkNum, totalChunks));
            Message_t msg = Packets::FirmwareChunk(chunkNum, (uint8_t*)buf, 56);
            SendPacket(dfuSerial, msg);
            unsigned long long startTime = time_now;
            while(dfuSerial.available() < 64 && startTime + 1000 > time_now) {std::this_thread::yield();}
            bytesRead = dfuSerial.read(buffer, 64);
            retries++;
        } while (bytesRead < 64 && retries < 5);
        if(bytesRead < 64)
        {
            wxMessageBox(string_format("Unable to write chunk %i", chunkNum), "Error", wxICON_ERROR | wxOK);
            dfuSerial.close();
            fileStream.close();
            return;
        }
        retries = 0;
        bytesRead = 0;
        chunkNum++;
        progress++;
        progressGauge->SetValue((progress) * (100.0f/((float)totalChunks+200.0f)));
        Update();
    } while (!fileStream.eof());
    fileStream.close();

    Message_t msg = Packets::SystemReset();
    SendPacket(dfuSerial, msg);

    dfuSerial.close();

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
