/***************************************************************
 * Name:      gmolMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Fabricio Costa Alisedo (fcosta@computaciongrafica.com)
 * Created:   2008-03-02
 * Copyright: Fabricio Costa Alisedo (http://moldeo.computaciongrafica.com)
 * License:
 **************************************************************/

#include "gmolMain.h"

#include "FileBrowser.h"
#include "FilterBrowser.h"
#include "RenderPreview.h"
#include "VideoBrowser.h"

#include <wx/msgdlg.h>

//(*InternalHeaders(gmolFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(gmolFrame)
const long gmolFrame::idMenuQuit = wxNewId();
const long gmolFrame::idMenuAbout = wxNewId();
const long gmolFrame::ID_STATUSBAR1 = wxNewId();
const long gmolFrame::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(gmolFrame,wxFrame)
    //(*EventTable(gmolFrame)
    //*)
END_EVENT_TABLE()

gmolFrame::gmolFrame(wxWindow* parent,wxWindowID id)
{
    FrameManager.SetManagedWindow(this);

    //(*Initialize(gmolFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;
    
    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(894,547));
    SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    Timer1.SetOwner(this, ID_TIMER1);
    
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&gmolFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&gmolFrame::OnAbout);
    //*)

    RenderPreview   *mRenderPreview = new RenderPreview( this, wxID_ANY, wxDefaultPosition, wxDefaultSize );
    FileBrowser   *mFileBrowser = new FileBrowser( this, wxID_ANY, wxDefaultPosition, wxDefaultSize );
    VideoBrowser   *mVideoBrowser = new VideoBrowser( this, wxID_ANY, wxDefaultPosition, wxDefaultSize );
    FilterBrowser   *mFilterBrowser = new FilterBrowser( this, wxID_ANY, wxDefaultPosition, wxDefaultSize );

    mFileBrowser->SetVideoBrowser(mVideoBrowser);
    mVideoBrowser->SetRenderPreview(mRenderPreview);

  	FrameManager.AddPane( mFileBrowser, wxAuiPaneInfo().Name(wxT("File Browser")).Caption(wxT("File Browser")).CenterPane().Hide().CaptionVisible().Dockable());
  	FrameManager.AddPane( mVideoBrowser, wxAuiPaneInfo().Name(wxT("Video Browser")).Caption(wxT("Video Browser")).CenterPane().Hide().CaptionVisible().Dockable());
  	FrameManager.AddPane( mRenderPreview, wxAuiPaneInfo().Name(wxT("Render Preview")).Caption(wxT("Render Preview")).CenterPane().Hide().CaptionVisible().Dockable());
  	FrameManager.AddPane( mFilterBrowser, wxAuiPaneInfo().Name(wxT("Filter Browser")).Caption(wxT("Filter Browser")).CenterPane().Hide().CaptionVisible().Dockable());

	//LEFT PANE
	FrameManager.GetPane(wxT("File Browser")).Show().Left().Layer(1).Row(0).Position(0);
	FrameManager.GetPane(wxT("Video Browser")).Show().Left().Layer(1).Row(0).Position(1);
	//FrameManager.GetPane(wxT("Log")).Show().Left().Layer(1).Row(0).Position(1);

	//CENTER PANE
	FrameManager.GetPane(wxT("Render Preview")).Show().Center().Top().Layer(0).Row(0).Position(0);
	//FrameManager.GetPane(wxT("filesbook")).Show().Center().Bottom().Layer(0).Row(0).Position(0);

	//RIGHT PANE
	FrameManager.GetPane(wxT("Filter Browser")).Show().Right().Layer(1).Position(0);
	//FrameManager.GetPane(wxT("Inspector")).Show().Right().Layer(1).Row(0).Position(1);

	FrameManager.Update();
}

gmolFrame::~gmolFrame()
{
    //(*Destroy(gmolFrame)
    //*)
}

void gmolFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void gmolFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}
