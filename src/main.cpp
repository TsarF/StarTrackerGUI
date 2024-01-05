#include <stdio.h>
#include <wx/wx.h>

#include "common.h"
#include "mainWindow.h"

class App : public wxApp
{
    public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    MainFrame *frame = new MainFrame();
    frame->Show();

    return true;
}