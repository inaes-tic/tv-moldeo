#ifndef MOPREVIEWWINDOW_H
#define MOPREVIEWWINDOW_H

#include "moIDirectorActions.h"


//(*Headers(moPreviewWindow)
#include <wx/panel.h>
#include <wx/bmpbuttn.h>
//*)

class moGLCanvas;

class moPreviewWindow: public wxPanel, public moIDirectorActions
{
	public:

		moPreviewWindow(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~moPreviewWindow();

		//(*Declarations(moPreviewWindow)
		wxBitmapButton* BitmapButtonSaveSession;
		wxBitmapButton* ButtonFullSCreen;
		wxBitmapButton* BitmapButtonFocusOutput;
		wxBitmapButton* BitmapButtonPlay;
		wxBitmapButton* BitmapButtonPause;
		wxBitmapButton* BitmapButtonSaveVideo;
		wxBitmapButton* BitmapButtonScreenshot;
		//*)
		moGLCanvas* m_pGLCanvas;

		void Init( moIDirectorActions* pActionsHandler,wxGLContext* p_pGLContext );

	protected:

		//(*Identifiers(moPreviewWindow)
		static const long ID_BITMAPBUTTONPLAY;
		static const long ID_BITMAPBUTTONPAUSE;
		static const long ID_BITMAPBUTTONFULLSCREEN;
		static const long ID_BITMAPBUTTONSCREENSHOT;
		static const long ID_BITMAPBUTTONSAVEVIDEO;
		static const long ID_BITMAPBUTTONSAVESESSION;
		static const long ID_BITMAPBUTTONFOCUSOUTPUT;
		//*)

	private:

		//(*Handlers(moPreviewWindow)
		void OnButtonFullSCreenClick(wxCommandEvent& event);
		void OnBitmapButtonFocusAndActivate(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#include "moGLCanvas.h"

#endif
