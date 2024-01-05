#include "mainWindow.h"
#include "platform.h"

#include "settingsWindow.h"
#include "common.h"

#include <vector>
#include <exception>
#include <string>

std::vector<std::string> serialPorts{};

SettingsFrame *settingsFrame;

void MainFrame::Build_Strip(wxMenuBar *menuBar)
{
    wxMenu *menuSettings = new wxMenu;

    menuSettings->Append(0, "&Connection");
    Bind(wxEVT_MENU, &MainFrame::OnSettings, this, 0);



    menuBar->Append(menuSettings, "&Settings");
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Star Tracker Utility")
{
    //Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnExit, this, wxID_ANY);

    wxMenuBar *menuBar = new wxMenuBar;

    Build_Strip(menuBar);

    SetMenuBar(menuBar);

    SetClientSize(1280, 720);

    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 2, wxSize(0,0));
    
    statusSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Status");
    statusSizer->Add(statusText);

    SetSizer(gridSizer);
    Show();
}

void MainFrame::UpdateStatus()
{
    std::string statusStr;
    if(g_serialPort->isOpen())
    {
        statusStr.append("Connected\n");
    }
    else
    {
        statusStr.append("Disconnected\n");
    }

    
}

void MainFrame::OnExit(wxCloseEvent& event)
{
    settingsFrame->Destroy();
    Destroy();
    exit(0);
}

void MainFrame::OnSettings(wxCommandEvent &event)
{
    settingsFrame = new SettingsFrame();
    settingsFrame->ShowModal();
    settingsFrame->Close();
    settingsFrame->Destroy();
}
