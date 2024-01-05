#include "settingsWindow.h"
#include "common.h"
#include "utils.h"

#ifndef max
#define max(A, B) {A > B ? A : B} 
#endif

ConnectionTab::ConnectionTab(wxNotebook* parent) : wxPanel(parent, wxID_ANY)
{
    serialComboBox = new wxComboBox(this, wxID_ANY, "Serial Port", wxDefaultPosition, wxDefaultSize);// wxSize(40, 10));
    QuerySerialPorts(g_availableSerialPorts);
    for(int i = 0; i<g_availableSerialPorts.size(); i++)
    {
        serialComboBox->Append(g_availableSerialPorts[i].c_str());
    }
    serialComboBox->Bind(wxEVT_COMBOBOX, &ConnectionTab::OnSerialComboBox, this);

    wxButton* refreshSerialConnectionButton = new wxButton(this, wxID_ANY, "Refresh", wxDefaultPosition, wxDefaultSize);
    refreshSerialConnectionButton->Bind(wxEVT_BUTTON, &ConnectionTab::OnRefreshSerialListButton, this);

    wxButton* testSerialConnectionButton = new wxButton(this, wxID_ANY, "Test Connection", wxDefaultPosition, wxDefaultSize);// wxSize(40, 10));
    testSerialConnectionButton->Bind(wxEVT_BUTTON, &ConnectionTab::OnTestSerialConnectionButton, this);

    wxGridSizer* sizer = new wxGridSizer(2, 2, 3, 3);
    wxStaticBoxSizer* serialSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Serial");
    wxBoxSizer* serialComboBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* testSerialConnectionButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* refreshSerialConnectionButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(serialSizer, 0, wxALL | wxEXPAND, 0);
    serialSizer->Add(serialComboBoxSizer, 0, wxALL | wxEXPAND, 0);
    serialSizer->Add(refreshSerialConnectionButtonSizer, 0, wxALL | wxEXPAND, 0);
    serialSizer->Add(testSerialConnectionButtonSizer, 0, wxALL | wxEXPAND, 0);

    serialComboBoxSizer->Add(serialComboBox, 1, wxALL, 3);
    refreshSerialConnectionButtonSizer->Add(refreshSerialConnectionButton, 1, wxALL, 3);
    testSerialConnectionButtonSizer->Add(testSerialConnectionButton, 1, wxALL, 3);
    SetSizer(sizer);
}

void ConnectionTab::OnSerialComboBox(wxCommandEvent& event)
{
    int selectedIndex = serialComboBox->GetSelection();
    g_settings.serialPort = g_availableSerialPorts[selectedIndex];
    SaveSettings(g_settings);
}

void ConnectionTab::OnTestSerialConnectionButton(wxCommandEvent& event)
{
    printf("test connection");
}

void ConnectionTab::OnRefreshSerialListButton(wxCommandEvent& event)
{
    QuerySerialPorts(g_availableSerialPorts);
    serialComboBox->Clear();
    for (int i = 0; i < g_availableSerialPorts.size(); i++)
    {
        serialComboBox->Append(g_availableSerialPorts[i].c_str());
    }
    serialComboBox->Refresh();
}

AppTab::AppTab(wxNotebook* parent) : wxPanel(parent, wxID_ANY)
{
    wxStaticText* label = new wxStaticText(this, wxID_ANY, "Content of Tab Two", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(label, 1, wxALIGN_CENTRE | wxALL, 5);
    SetSizer(sizer);
}

SettingsFrame::SettingsFrame(int page) : wxDialog(nullptr, wxID_ANY, "Settings")
{
    //Bind(wxEVT_CLOSE_WINDOW, &SettingsFrame::OnExit, this, wxID_ANY);
    if(!FileExists("settings.json"))
    {
        SaveSettings(g_defaultSettings); // save default settings file if the settings file does not exist
    }
    LoadSettings(g_settings);

    notebook = new wxNotebook(this, wxID_ANY);

    ConnectionTab* connectionTab = new ConnectionTab(notebook);
    AppTab* appTab = new AppTab(notebook);

    notebook->AddPage(connectionTab, "Connection", page == 0);
    notebook->AddPage(appTab, "App", page == 1);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* nextButton = new wxButton(this, wxID_ANY, "Next >");
    nextButton->Bind(wxEVT_BUTTON, &SettingsFrame::OnNext, this);
    wxBoxSizer* nextButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* backButton = new wxButton(this, wxID_ANY, "< Back");
    backButton->Bind(wxEVT_BUTTON, &SettingsFrame::OnBack, this);
    wxBoxSizer* backButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    nextButtonSizer->Add(nextButton, 1, wxALL | wxEXPAND);
    backButtonSizer->Add(backButton, 1, wxALL | wxEXPAND);

    buttonSizer->Add(backButtonSizer, 1, wxALL | wxEXPAND);
    buttonSizer->AddStretchSpacer(3);
    buttonSizer->Add(nextButtonSizer, 1, wxALL | wxEXPAND);
    sizer->Add(notebook, 1, wxEXPAND);
    sizer->Add(buttonSizer, 0, wxALL | wxEXPAND, 3);
    SetSizer(sizer);

    SetClientSize(640, 480);
}

void SettingsFrame::OnNext(wxCommandEvent& event)
{
    notebook->SetSelection((notebook->GetSelection() + 1) % 2);
}

void SettingsFrame::OnBack(wxCommandEvent& event)
{
    notebook->SetSelection((notebook->GetSelection() - 1) % notebook->GetRowCount());
}

void SettingsFrame::OnExit(wxCloseEvent &event)
{
    //this->Hide();
    //Close(true);
}
