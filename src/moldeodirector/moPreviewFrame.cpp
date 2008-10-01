#include "moPreviewFrame.h"

//(*InternalHeaders(moPreviewFrame)
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*IdInit(moPreviewFrame)
const long moPreviewFrame::TOOLPLAY = wxNewId();
const long moPreviewFrame::TOOLPAUSE = wxNewId();
const long moPreviewFrame::TOOLFULLSCREEN = wxNewId();
const long moPreviewFrame::ID_TOOLBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(moPreviewFrame,wxFrame)
	//(*EventTable(moPreviewFrame)
	//*)
	EVT_TOOL( wxID_ANY, moPreviewFrame::OnToolClick)
	//EVT_SIZE( moPreviewFrame::OnSize )
END_EVENT_TABLE()

moPreviewFrame::moPreviewFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(moPreviewFrame)
	Create(parent, wxID_ANY, _("Moldeo output"), wxDefaultPosition, wxDefaultSize, wxSTAY_ON_TOP|wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(800,600));
	SetForegroundColour(wxColour(255,255,255));
	SetBackgroundColour(wxColour(0,0,0));
	ToolBar1 = new wxToolBar(this, ID_TOOLBAR1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER, _T("ID_TOOLBAR1"));
	ToolBar1->SetToolBitmapSize(wxSize(32,32));
	ToolBarItem1 = ToolBar1->AddTool(TOOLPLAY, _("Play"), wxBitmap(wxImage(_T("../../doc/icons/play32.png"))), wxBitmap(wxImage(_T("../../doc/icons/play32dis.bmp"))), wxITEM_NORMAL, _("Play syncro"), _("Help text"));
	ToolBarItem2 = ToolBar1->AddTool(TOOLPAUSE, _("Pause"), wxBitmap(wxImage(_T("../../doc/icons/pause32.png"))), wxBitmap(wxImage(_T("../../doc/icons/pause32.bmp"))), wxITEM_NORMAL, _("Pause"), _("Pause"));
	ToolBarItem3 = ToolBar1->AddTool(TOOLFULLSCREEN, _("Fullscreen"), wxBitmap(wxImage(_T("../../doc/icons/fullscreen32.png"))), wxNullBitmap, wxITEM_NORMAL, _("Fullscreen"), _("Fullscreen"));
	ToolBar1->Realize();
	SetToolBar(ToolBar1);

	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&moPreviewFrame::OnClose);
	//*)
	m_pGLCanvas = NULL;
}

moPreviewFrame::~moPreviewFrame()
{
	//(*Destroy(moPreviewFrame)
	//*)
}

void moPreviewFrame::Init( moIDirectorActions* pActionsHandler, wxGLContext* p_pGLContext ) {

    SetNextActionHandler( pActionsHandler);

    if (!m_pGLCanvas)
        m_pGLCanvas = new moGLCanvas( this, p_pGLContext, -1, wxPoint(0,0), wxSize(800,600), 0, wxT("moGLCanvas"));

    if (m_pGLCanvas)
        m_pGLCanvas->Init( this );

    Log("Preview frame initialized");
}

void
moPreviewFrame::OnToolClick( wxCommandEvent &event) {

    if ( event.GetId() == TOOLPLAY) {
        ShowMessage( moText("Play") );
    } else if (event.GetId() == TOOLPAUSE) {
        ShowMessage( moText("Pause") );
    } else if (event.GetId() == TOOLFULLSCREEN) {
        ShowMessage( moText("Fullscreen") );
        FullScreen();
    }


}


void moPreviewFrame::OnClose(wxCloseEvent& event)
{
    ProjectPreview();
}


moDirectorStatus
moPreviewFrame::FullScreen( bool force) {

    if (!IsFullScreen() || force ) {
        ShowFullScreen( true, wxFULLSCREEN_NOMENUBAR | wxFULLSCREEN_NOSTATUSBAR | wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION | wxFULLSCREEN_NOTOOLBAR );
    } else {
        ShowFullScreen( false);
    }

    return MO_DIRECTOR_STATUS_OK;

}

/*
void moPreviewFrame::OnSize(wxSizeEvent& event)
{
    wxMessageBox("Frame On Size");
}
*/
