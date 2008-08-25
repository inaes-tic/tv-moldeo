#include "moDirectorFrame.h"


// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions(event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(moDirectorFrame, wxFrame)

    EVT_MENU( MODIRECTOR_OPENPROJECT, moDirectorFrame::OnOpenProject )
	EVT_MENU( MODIRECTOR_CLOSEPROJECT, moDirectorFrame::OnCloseProject )
	EVT_MENU( MODIRECTOR_SAVEPROJECT, moDirectorFrame::OnSaveProject )

	EVT_MENU( MODIRECTOR_QUIT,  moDirectorFrame::OnQuit )
    EVT_MENU( MODIRECTOR_ABOUT, moDirectorFrame::OnAbout )

	EVT_MENU( MODIRECTOR_PROJECT_PREVIEW, moDirectorFrame::OnProjectPreview )
	EVT_MENU( MODIRECTOR_PROJECT_PREVIEW_FULLSCREEN, moDirectorFrame::OnProjectPreviewFullscreen )
	EVT_MENU( MODIRECTOR_FULLSCREEN, moDirectorFrame::OnFullscreen )

	EVT_KEY_DOWN( moDirectorFrame::OnKeyDown )
	EVT_COMMAND( -1, MODIRECTOR_COMMAND, moDirectorFrame::OnCommand)
	EVT_CLOSE( moDirectorFrame::OnClose )
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
moDirectorFrame::moDirectorFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxPoint(0,0), wxSize(1024,768))
{

    m_cForeground = wxColour(255,255,255);
    m_cBackground = wxColour(80,80,80);

    // set the frame icon
    //SetIcon(wxICON(sample));

	FrameManager.SetManagedWindow(this);

#if wxUSE_MENUS
    // create a menu bar

	//FILE
    wxMenu *fileMenu = new wxMenu;

	wxMenuItem *preeffectsMenuItem = NULL;
	wxMenu *preeffectsMenu = new wxMenu;

	wxMenu *effectsMenu = new wxMenu;
	wxMenu *posteffectsMenu = new wxMenu;
	wxMenu *iodevicesMenu = new wxMenu;
	wxMenu *parameterMenu = new wxMenu;

	fileMenu->Append( MODIRECTOR_OPENPROJECT, _T("&Open Project\tAlt-O"), _T("Open an existing project"));
    fileMenu->Append( MODIRECTOR_CLOSEPROJECT, _T("&Close Project"), _T("Close the current project"));

    fileMenu->AppendSeparator();
    fileMenu->Append( MODIRECTOR_SAVEPROJECT, _T("Save Project\tCtrl-S"), _T("Save the current project"));

	fileMenu->Append(MODIRECTOR_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));
	//EDIT
	wxMenu *editMenu = new wxMenu;

	//VIEW
	wxMenu *viewMenu = new wxMenu;
	viewMenu->Append( MODIRECTOR_FULLSCREEN, _T("&Interface Fullscreen\tAlt-F11"), _T("Interface Fullscreen"));
	viewMenu->Append( MODIRECTOR_PROJECT_PREVIEW, _T("&Preview\tAlt-P"), _T("View frame preview"));
	viewMenu->Append( MODIRECTOR_PROJECT_PREVIEW_FULLSCREEN, _T("&Toggle Preview Fullscreen\tAlt-Return"), _T("Frame preview fullscreen"));

	//OPTIONS
	wxMenu *optionsMenu = new wxMenu;

    //HELP
    wxMenu *helpMenu = new wxMenu;

    helpMenu->Append(MODIRECTOR_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));


    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
	menuBar->Append(editMenu, _T("&Edit"));
	menuBar->Append(viewMenu, _T("&View"));
	menuBar->Append(optionsMenu, _T("&Options"));

    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun(by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR

    CreateGLWindows();

	m_pLogTextCtrl = CreateTextCtrl();
	m_pLogTextCtrl->SetForegroundColour(m_cForeground);
    m_pLogTextCtrl->SetBackgroundColour(m_cBackground);

    m_pStartFrame = new moDirectorStartFrame( this );

	FrameManager.AddPane( m_pLogTextCtrl,       wxAuiPaneInfo().Name(wxT("log")).CenterPane().Hide().Caption(wxT("Log")).CaptionVisible().Floatable().Movable().Dockable());
	FrameManager.AddPane( m_pPreviewWindow, wxAuiPaneInfo().Name(wxT("preview")).Caption(wxT("Console")).CaptionVisible().Floatable().Movable().Dockable().MaximizeButton().MinimizeButton().Resizable(true));
	FrameManager.AddPane( m_pStartFrame, wxAuiPaneInfo().Name(wxT("start")).Caption(wxT("Welcome")).CaptionVisible().Floatable().Movable().Dockable().MaximizeButton().MinimizeButton().Resizable(true));

	FrameManager.GetPane(wxT("preview")).Show().Center();
	FrameManager.GetPane(wxT("start")).Show().Right();
	FrameManager.GetPane(wxT("log")).Show().Center().Bottom();

    wxAuiDockArt* pFrameArt;

    pFrameArt = FrameManager.GetArtProvider();
    pFrameArt->SetColor( wxAUI_DOCKART_SASH_COLOUR, wxColour(80,80,80) );
    pFrameArt->SetColor( wxAUI_DOCKART_BORDER_COLOUR, wxColour(255,255,255) );
    pFrameArt->SetColor( wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxColour(10,40,120) );
    pFrameArt->SetColor( wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(0,0,0) );
    pFrameArt->SetColor( wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, wxColour(255,255,255) );

	FrameManager.Update();

}

moDirectorFrame::~moDirectorFrame() {

    m_pGLCanvas = NULL;
	FrameManager.UnInit();

}

void
moDirectorFrame::CreateGLWindows() {

   int attribList[2];
    attribList[0] = WX_GL_RGBA; //true color
    attribList[1] = 0;

    m_pGLContext = NULL;
    //create base wxGLCanvas to create context...
    //retreive context...
    //m_pBaseGLCanvas = (wxGLCanvas*)new moGLCanvas( this, wxID_ANY, &attribList[0], wxPoint(0,0), wxSize(800,600) );
    //m_pGLContext = m_pBaseGLCanvas->GetContext();
    //m_pBaseGLCanvas->Destroy();

    //create preview window, dockable...

	m_pPreviewWindow = NULL;
	m_pPreviewWindow = new moPreviewWindow( this, wxID_ANY );

	if (m_pPreviewWindow) {

        m_pPreviewWindow->Init( this, m_pGLContext );
        m_pGLCanvas = m_pPreviewWindow->m_pGLCanvas;
        if (m_pGLCanvas) {
            m_pGLContext = m_pGLCanvas->GetContext();
            m_pGLCanvas->SetCurrent();
        }

	}

	m_pPreviewFrame = NULL;

}

void moDirectorFrame::OnKeyDown( wxKeyEvent &event ) {

	event.Skip();

}

void moDirectorFrame::OnCommand( wxCommandEvent& event ) {

	// do something
	wxString text = event.GetString();

}

void
moDirectorFrame::SetDirectorCore( moDirectorCore* p_pDirectorCore ) {

	m_pDirectorCore = p_pDirectorCore;
	this->SetNextActionHandler( (moIDirectorActions*)m_pDirectorCore );

}

void
moDirectorFrame::SetIODeviceManager( wxEvtHandler*  m_pEventHandler ) {

    if (m_pPreviewFrame)
        if (m_pPreviewFrame->m_pGLCanvas)
         m_pPreviewFrame->m_pGLCanvas->Init( this);
    if (m_pPreviewWindow)
        if (m_pPreviewWindow->m_pGLCanvas)
            m_pPreviewWindow->m_pGLCanvas->Init( this);
}

moGLCanvas*  moDirectorFrame::GetGLCanvas() {
	return m_pGLCanvas;
}

MOboolean
moDirectorFrame::Init() {

    if (m_pStartFrame) {
        //m_pStartFrame->Activate();
        //( (wxWindow*) m_pStartFrame, "Welcome to Moldeo Director");

        m_pStartFrame->Init(this);
    }
	return true;
}

MOboolean moDirectorFrame::Finish() {

    m_pGLCanvas = NULL;

	return true;
}


void moDirectorFrame::OnEraseBackground(wxEraseEvent& event)
{
    event.Skip();
}

void moDirectorFrame::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

wxPanel* moDirectorFrame::CreatePanel() {

	return new wxPanel(this, -1,
		wxPoint(0,0), wxSize(160,250),wxTAB_TRAVERSAL, _T("panel"));

}


wxTextCtrl* moDirectorFrame::CreateTextCtrl()
{

    return new wxTextCtrl(this,-1, wxT(""),
                          wxPoint(0,0), wxSize(150,90),
                          wxNO_BORDER | wxTE_MULTILINE | wxTE_RICH);
}


void
moDirectorFrame::CreateGLContext() {

	if (m_pGLCanvas)
        m_pGLContext = m_pGLCanvas->GetContext();

}

void moDirectorFrame::OnClose(wxCloseEvent& event) {
	if( wxMessageBox(_T("Really close Moldeo Player?"), _T("Please confirm"),
                          wxICON_QUESTION | wxYES_NO) != wxYES )
    {
        event.Veto();

        return;
    } else {
		if(this->CloseProject()==MO_DIRECTOR_STATUS_OK) {
		    if (this->CloseApp()==MO_DIRECTOR_STATUS_OK) {
                event.Skip();
                return;
		    }
		}
		event.Veto();
	}
}

void moDirectorFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void moDirectorFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the minimal sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About Minimal"), wxOK | wxICON_INFORMATION, this);
}


//INTERFACE METHODS
void
moDirectorFrame::OnOpenProject(wxCommandEvent& event) {

	wxFileDialog* pFileDialog = NULL;
	moProjectDescriptor ProjectDescriptor;
	moDirectorStatus	mStatus;

	//open browser window
	if ( event.GetString()=="" ) {

        pFileDialog = new wxFileDialog( this );

        if(pFileDialog) {

            pFileDialog->SetWildcard(wxT("MOL files (*.mol)|*.mol|All files (*.*)|*.*"));

            if( pFileDialog->ShowModal() == wxID_OK ) {

                wxFileName	FileName( pFileDialog->GetPath() );

                wxString path = FileName.GetPath();
                #ifdef MO_WIN32
                    path+= "\\";
                #else
                    path+= "/";
                #endif
                wxString name = FileName.GetFullName();
                const char *cfilepath = (char*)path.c_str();
                const char *cfilename = (char*)name.c_str();

                ProjectDescriptor.Set( moText((char*)cfilepath), moText((char*)cfilename) );

                mStatus = OpenProject( ProjectDescriptor );

            }

        }

	} else {

	    Log( moText("Direct opening..") + moWx2Text(event.GetString()));

        wxFileName	FileName( event.GetString() );

        wxString path = FileName.GetPath();
        #ifdef MO_WIN32
            path+= "\\";
        #else
            path+= "/";
        #endif
        wxString name = FileName.GetFullName();
        const char *cfilepath = (char*)path.c_str();
        const char *cfilename = (char*)name.c_str();

        ProjectDescriptor.Set( moText((char*)cfilepath), moText((char*)cfilename) );

        mStatus = OpenProject( ProjectDescriptor );

    }

}

void
moDirectorFrame::OnSaveProject(wxCommandEvent& event) {
    SaveProject();
}

void
moDirectorFrame::OnCloseProject(wxCommandEvent& event) {

    this->CloseProject();
}


moDirectorStatus
moDirectorFrame::CloseProject() {

	moDirectorChildFrame* pChild = NULL;
	moDirectorChildConsole* pDirectorChildConsole = NULL;

    if (this->CloseAll()==MO_DIRECTOR_STATUS_OK) {
        if ( m_pDirectorCore->CloseProject() == MO_DIRECTOR_STATUS_OK ) {

            return MO_DIRECTOR_STATUS_OK;
        }
    }
    return MO_DIRECTOR_STATUS_ERROR;

}

void
moDirectorFrame::OnProjectPreview( wxCommandEvent& event ) {

    ProjectPreview();

}

void
moDirectorFrame::OnProjectPreviewFullscreen( wxCommandEvent& event ) {

    FullScreen(false);

}

void
moDirectorFrame::OnFullscreen( wxCommandEvent& event ) {

    if (!IsFullScreen() ) {
        ShowFullScreen( true, wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION );
    } else {
        ShowFullScreen( false);
    }

}


/*================================================

  IDirectorActions Implemented by FrameDirector:

================================================*/

moDirectorStatus
moDirectorFrame::FocusOutput() {


    if(m_pGLCanvas) {

        m_pGLCanvas->Activate( m_pDirectorCore->GetDirectorIODeviceManager() );
    }

}


moDirectorStatus
moDirectorFrame::FullScreen( bool force ) {

    if (m_pPreviewFrame==NULL) {
        ProjectPreview();
        if (m_pPreviewFrame)
            m_pPreviewFrame->FullScreen();
    } else
    if ( m_pPreviewFrame!=NULL && !m_pPreviewFrame->IsShown() ) {
        ProjectPreview();
        m_pPreviewFrame->FullScreen(true);
    } else
    if (m_pPreviewFrame!=NULL && m_pPreviewFrame->IsShown()) {
        m_pPreviewFrame->FullScreen();
    }


}

moDirectorStatus
moDirectorFrame::ProjectPreview() {

    if (m_pPreviewWindow)
        if (m_pGLCanvas)
            if (m_pGLCanvas==m_pPreviewWindow->m_pGLCanvas) {

                m_pPreviewWindow->m_pGLCanvas->Finish();

                if (!m_pPreviewFrame) {
                    m_pPreviewFrame = new moPreviewFrame( this, wxID_ANY );
                    //if (!m_pGLContext) {
                        m_pGLContext = m_pPreviewWindow->m_pGLCanvas->GetContext();
                    //}
                    m_pPreviewFrame->Init( this, m_pGLContext );
                }
                m_pPreviewFrame->Init( this, m_pGLContext );
                m_pPreviewFrame->m_pGLCanvas->SetCurrent();
                m_pPreviewFrame->Show();

                m_pGLCanvas = m_pPreviewFrame->m_pGLCanvas;

            } else {
                m_pGLCanvas = NULL;
                m_pPreviewFrame->m_pGLCanvas->Finish();
                m_pPreviewFrame->Hide();

                m_pPreviewWindow->Init( this, m_pGLContext );
                m_pPreviewWindow->m_pGLCanvas->SetCurrent();
                m_pPreviewWindow->Refresh();

                m_pGLCanvas = m_pPreviewWindow->m_pGLCanvas;

            }
        else return MO_DIRECTOR_STATUS_OK;
    else return MO_DIRECTOR_STATUS_OK;
    return MO_DIRECTOR_STATUS_OK;

}

void
moDirectorFrame::ViewSwapBuffers() {
    if (m_pGLCanvas!=NULL) {
        m_pGLCanvas->ViewSwapBuffers();
    }
}

MO_HANDLE
moDirectorFrame::GetHandle() {
    if (m_pGLCanvas)
        return m_pGLCanvas->GetHandle();
    return NULL;

}



MO_DISPLAY
moDirectorFrame::GetDisplay() {
    if (m_pGLCanvas)
        return m_pGLCanvas->GetDisplay();
    return NULL;

}

moDirectorStatus
moDirectorFrame::ProjectUpdated( moProjectDescriptor p_ProjectDescriptor ) {

	return MO_DIRECTOR_STATUS_OK;
}


void
moDirectorFrame::Log( moText p_message ) {

	wxString  w =(wxChar*)(char*)p_message;

	m_pLogTextCtrl->SetDefaultStyle( wxTextAttr( wxColour( 0, 200, 200 )) );

	m_pLogTextCtrl->AppendText(w + wxT("\n"));

}

void
moDirectorFrame::LogError( moText p_message ) {
	//seria bueno en rojo...

	wxString  w =(wxChar*)(char*)p_message;

	m_pLogTextCtrl->SetDefaultStyle( wxTextAttr( wxColour(255,0,0) ) );

	m_pLogTextCtrl->AppendText(w + wxT("\n"));

	wxMessageBox(w);

}
