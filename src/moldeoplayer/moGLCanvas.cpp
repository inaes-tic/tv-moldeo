#include "moGLCanvas.h"

#define TIMER_ID 111

BEGIN_EVENT_TABLE(moGLCanvas, wxGLCanvas)
    EVT_TIMER( TIMER_ID, moGLCanvas::OnTimer)
    EVT_SIZE(moGLCanvas::OnSize)
	EVT_CLOSE(moGLCanvas::OnClose)
    EVT_PAINT(moGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(moGLCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(moGLCanvas::OnMouse)
	EVT_KEY_DOWN( moGLCanvas::OnKeyDown )
	EVT_KEY_UP( moGLCanvas::OnKeyUp )
END_EVENT_TABLE()

moGLCanvas::moGLCanvas(wxWindow *parent,
    wxWindowID id,
    int* attribList,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, id, attribList, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
{
	m_init = false;
    block = false;
    m_active = false;
	m_pHandler = NULL;

}

moGLCanvas::moGLCanvas(wxWindow *parent,
    wxGLContext*    p_pGLContext,
    wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent,(wxGLContext*) p_pGLContext, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
{
	m_init = false;
    block = false;
    m_active = false;
	m_pHandler = NULL;
}

moGLCanvas::moGLCanvas(wxWindow *parent,
    wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
{
	m_init = false;
    block = false;
    m_active = false;
	m_pHandler = NULL;
}

moGLCanvas::~moGLCanvas()
{

}

moGLCanvas*
moGLCanvas::Init( moIDirectorActions* pActionsHandler) {

    SetNextActionHandler( pActionsHandler );

	block = false;
	m_init = false;
	m_active = false;

    //m_timer.SetOwner( this, TIMER_ID);
	//m_timer.Start(10);

	return this;
}

void
moGLCanvas::Activate( wxEvtHandler* p_pHandler ) {
    Log("GL Canvas Activate");
    if (p_pHandler) {
        m_pHandler = p_pHandler;
        m_active = true;
    }

}

MOboolean
moGLCanvas::Finish() {
	m_pHandler = NULL;
	block = true;
	m_init = false;
	m_active = false;
	Log("GL Canvas Finalized");
	return true;
}

void
moGLCanvas::Reset() {
	m_init = false;
	//Refresh();
}

void moGLCanvas::OnTimer(wxTimerEvent &event) {

    //ViewSwapBuffers();

}

void moGLCanvas::ViewSwapBuffers() {
    SetCurrent();
    if( m_init && !block ) {
        SwapBuffers();
    }
}

MO_HANDLE
moGLCanvas::GetHandle() {
    #ifdef MO_WIN32
    return (MO_HANDLE)wxWindow::GetHandle();
    #else
    GdkWindow *gdkwin = ((GtkWidget*)wxWindow::GetHandle())->window;
    return (MO_HANDLE)GDK_WINDOW_XID(gdkwin);
    #endif
}


MO_DISPLAY
moGLCanvas::GetDisplay() {
    #ifdef MO_WIN32
        return NULL;
    #else
        return (MO_DISPLAY)wxGetDisplay(); //default display;
    #endif
}

void moGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    /* must always be here */
    wxPaintDC dc(this);
    int w, h;
    GetClientSize(&w, &h);

    if( GetContext() )
    if (!m_init && !block) {
        SetView(0,0,w,h);
        Log("GL Canvas Initialized");
        m_init = true;
    }

/*
    if (block) {
        return;
    }

#ifndef __WXMOTIF__
    if(!GetContext()) return;
#endif

    // Initialize OpenGL
        SetCurrent();


    GetClientSize(&w, &h);

    // Init OpenGL once, but after SetCurrent
   if(!m_init)
    {
      InitGL(w,h);

      if (m_pDirectorConsole) {
          moResourceManager* RM = m_pDirectorConsole->GetResourceManager();
          moConfig dummy;
          if (!RM->Initialized()) {
              RM->Init( moText("../../data/test"),dummy,0,800,600,800,600);
              m_3dRef.Finish();
              int result = 0;
              result = m_3dRef.Add( "objetos/iconos3d/moldeologo.3ds" , m_pDirectorConsole->GetResourceManager()->GetModelMan() );
              if (result==MO_UNDEFINED)
                m_pDirectorConsole->LogError("Couldn't load Moldeo 3d logo");
              m_init = true;
          } else {
              if ( m_3dRef.nModels==0) {
                int result = 0;
                result = m_3dRef.Add( "objetos/iconos3d/moldeologo.3ds" , m_pDirectorConsole->GetResourceManager()->GetModelMan() );
                if (result==MO_UNDEFINED)
                    m_pDirectorConsole->LogError("Couldn't load Moldeo 3d logo");
              }
          }

          m_init = RM->Initialized();
      }
    }


    // View
   	moRenderManager* Render = NULL;
	moGLManager* GLManager = NULL;

   if(m_pDirectorConsole) {
		moResourceManager* RM = m_pDirectorConsole->GetResourceManager();
		if (RM) {

			Render = RM->GetRenderMan();
			GLManager = RM->GetGLMan();

			if (Render) {
				Render->Finish();
				Render->Init( 0, w, h, w, h);
			}

			if(GLManager)
				GLManager->SetPerspectiveView( w, h );

		}

   }


	if (!GLManager)	{
		glViewport( 0, 0, w, h );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		double ratio =(double) w /(double) h;
		gluPerspective( 60.0,ratio, 0.1, 1024.0 );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	}

	if(m_pDirectorConsole!=NULL && m_pDirectorConsole->Initialized()) {
		m_pDirectorConsole->Draw();
	} else {
		MoldeoLogo(0);
	}
*/

}

void moGLCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    //wxGLCanvas::OnSize(event);

    // set GL viewport(not called by wxGLCanvas::OnSize on all platforms...)

    //wxMessageBox("GL On Size");

    int w, h;
    GetClientSize(&w, &h);

    if( GetContext() )
    if (!block) {
        SetView( 0, 0, w, h );
        //m_init = true;

    }
        /*
		if (m_init) {
   			moRenderManager* Render = NULL;
			moGLManager* GLManager = NULL;

		   if(m_pDirectorConsole) {
				moResourceManager* RM = m_pDirectorConsole->GetResourceManager();
				if (RM) {

					Render = RM->GetRenderMan();
					GLManager = RM->GetGLMan();

					if (Render) {
						Render->Finish();
						Render->Init( 0, w, h, w, h);
					}

					if(GLManager)
						GLManager->SetPerspectiveView( w, h );

				}

		   }


			if (!GLManager)	{
				glViewport( 0, 0, w, h );
				glMatrixMode( GL_PROJECTION );
				glLoadIdentity();
				double ratio =(double) w /(double) h;
				gluPerspective( 60.0,ratio, 0.1, 1024.0 );
				glMatrixMode( GL_MODELVIEW );
				glLoadIdentity();
			}
		}
		*/
}

void moGLCanvas::OnClose(wxCloseEvent& event) {

}

void moGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    /* Do nothing, to avoid flashing on MSW */
/*
    if (block) {
        wxSize sz( GetClientSize() );
        wxPaintDC dc(this);
        dc.SetClippingRegion(0,0,400,300);
        dc.SetBrush(dc.GetBackground());
        dc.DrawRectangle(0,0,400,300);
        return;
    }*/
}

void moGLCanvas::OnMouse( wxMouseEvent& event )
{

    //if( event.Dragging() || event.Moving() )
    if(m_active)
        if(m_pHandler) {
            m_pHandler->ProcessEvent(event);
        }
		event.Skip();


}

void moGLCanvas::OnKeyDown( wxKeyEvent &event ) {

if(m_active)
    if(m_pHandler)		{
        m_pHandler->ProcessEvent(event);
    }
	event.Skip();
}

void moGLCanvas::OnKeyUp( wxKeyEvent &event ) {
if(m_active)
    if(m_pHandler)		{
        m_pHandler->ProcessEvent(event);
    }
	event.Skip();
}

