/////////////////////////////////////////////////////////////////////////////
// Name:        moldeodirector.cpp
// Author:      Fabricio Costa Alisedo
// Modified by:
// Created:     02/03/2006
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "moldeodirector.h"



// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution(it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type(i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(moDirectorApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool moDirectorApp::OnInit()
{

	moDirectorCore*			m_pDirectorCore = NULL;
	moDirectorFrame*		m_pDirectorFrame = NULL;

	SetAppName("Moldeo Director");

    // created initially)
   cout << "Image Handlers..." << endl;
#if wxUSE_SYSTEM_OPTIONS
    //wxSystemOptions::SetOption(wxT("no-maskblt"), 1);
#endif

    wxInitAllImageHandlers();
#if wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS
    //wxFileSystem::AddHandler(new wxInternetFSHandler);
#endif

   wxImageHandler* hndPNG = wxImage::FindHandler((long)wxBITMAP_TYPE_PNG);
   if (!hndPNG)
    cout << "Warning: PNG Image handler not loaded..." << endl;

   wxImageHandler* hndJPEG = wxImage::FindHandler((long)wxBITMAP_TYPE_JPEG);
   if (!hndJPEG)
    cout << "Warning: JPEG Image handler not loaded..." << endl;

   wxImageHandler* hndTGA = wxImage::FindHandler((long)wxBITMAP_TYPE_TGA);
   if (!hndTGA)
    cout << "Warning: TGA Image handler not loaded..." << endl;

    // create the main application window

    cout << "Director Frame..." << endl;
    m_pDirectorFrame = new moDirectorFrame(_T("Moldeo Director"));
    m_pDirectorFrame->SetIcon( wxIcon( wxIconLocation(wxT("../../art/icons/Moldeo32.ico")) ) );
    m_pDirectorFrame->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
    m_pDirectorFrame->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
    cout << "m_pDirectorFrame:" << (m_pDirectorFrame!=NULL) << endl;

    cout << "Director Core..." << endl;
	m_pDirectorCore = new moDirectorCore();
	cout << "m_pDirectorCore:" << (m_pDirectorCore!=NULL) << endl;

	cout << "Director Frame UI to Core..." << endl;
	m_pDirectorCore->SetUserInterface( m_pDirectorFrame );

	wxStandardPaths StdPaths;

	wxFileName exename(StdPaths.GetExecutablePath());
	exename.MakeAbsolute();

	//wxFileName userdatadir( StdPaths.GetUserDataDir() );

	m_pDirectorCore->SetPaths( moWx2Text(exename.GetPath()) );
	m_pDirectorCore->Init();



   //wxList   ImageHandlerList = wxImage::GetHandlers();


   cout << "Showing Frame..." << endl;
   m_pDirectorFrame->Show(true);
   cout << "Setting Top Window..." << endl;
   SetTopWindow(m_pDirectorFrame);

	m_pDirectorFrame->Init();
    cout << "Success!!! rock and roll!!" << endl;


    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

