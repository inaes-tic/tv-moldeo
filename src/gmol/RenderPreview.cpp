#include "RenderPreview.h"

//(*InternalHeaders(RenderPreview)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(RenderPreview)
const long RenderPreview::ID_BUTTONPLAY = wxNewId();
const long RenderPreview::ID_BUTTONPAUSE = wxNewId();
const long RenderPreview::ID_BUTTONSTOP = wxNewId();
//*)

BEGIN_EVENT_TABLE(RenderPreview,wxPanel)
	//(*EventTable(RenderPreview)
	//*)
END_EVENT_TABLE()

RenderPreview::RenderPreview(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(RenderPreview)
	Create(parent, id, wxDefaultPosition, wxSize(270,192), wxTAB_TRAVERSAL, _T("id"));
	SetMinSize(wxSize(200,200));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
	ButtonPlay = new wxButton(this, ID_BUTTONPLAY, _("Play"), wxPoint(0,112), wxSize(32,23), 0, wxDefaultValidator, _T("ID_BUTTONPLAY"));
	ButtonPause = new wxButton(this, ID_BUTTONPAUSE, _("Pause"), wxPoint(32,0), wxSize(40,23), 0, wxDefaultValidator, _T("ID_BUTTONPAUSE"));
	ButtonStop = new wxButton(this, ID_BUTTONSTOP, _("Stop"), wxPoint(72,0), wxSize(40,23), 0, wxDefaultValidator, _T("ID_BUTTONSTOP"));
	
	Connect(ID_BUTTONPLAY,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RenderPreview::OnButtonPlayClick);
	Connect(ID_BUTTONPAUSE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RenderPreview::OnButtonPauseClick);
	Connect(ID_BUTTONSTOP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&RenderPreview::OnButtonStopClick);
	//*)
	m_pGLCanvas = new wxGLCanvas( (wxWindow*)this, (wxWindowID)wxID_ANY, wxPoint(0,0),wxSize(360,288));
	/*if (m_pGLContext)
        m_pGLContext = new wxGLContext(m_pGLCanvas);*/

}

RenderPreview::~RenderPreview()
{
	//(*Destroy(RenderPreview)
	//*)
}

bool RenderPreview::Init() {

    m_pGLCanvas->SetCurrent();
    glClearColor( 5.0, 0.0, 0.0, 1.0);
    glClear( GL_COLOR_BUFFER_BIT);
    m_pGLCanvas->SwapBuffers();

}


void RenderPreview::OnButtonPlayClick(wxCommandEvent& event)
{
    Init();
}

void RenderPreview::OnButtonPauseClick(wxCommandEvent& event)
{
    m_pGLCanvas->SetCurrent();
    glClearColor( 0.0, 0.5, 0.0, 1.0);
    glClear( GL_COLOR_BUFFER_BIT);
    m_pGLCanvas->SwapBuffers();
}

void RenderPreview::OnButtonStopClick(wxCommandEvent& event)
{
        m_pGLCanvas->SetCurrent();
    glClearColor( 0.5, 0.5, 0.0, 1.0);
    glClear( GL_COLOR_BUFFER_BIT);
    m_pGLCanvas->SwapBuffers();
}
