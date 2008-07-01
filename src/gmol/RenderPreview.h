#ifndef RENDERPREVIEW_H
#define RENDERPREVIEW_H

//(*Headers(RenderPreview)
#include <wx/panel.h>
#include <wx/button.h>
//*)
#include <wx/glcanvas.h>

class RenderPreview: public wxPanel
{
	public:

		RenderPreview(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~RenderPreview();
        bool Init();
		//(*Declarations(RenderPreview)
		wxButton* ButtonStop;
		wxButton* ButtonPlay;
		wxButton* ButtonPause;
		//*)

	protected:

		//(*Identifiers(RenderPreview)
		static const long ID_BUTTONPLAY;
		static const long ID_BUTTONPAUSE;
		static const long ID_BUTTONSTOP;
		//*)
		wxGLContext*    m_pGLContext;
		wxGLCanvas*     m_pGLCanvas;

	private:

		//(*Handlers(RenderPreview)
		void OnButtonPlayClick(wxCommandEvent& event);
		void OnButtonPauseClick(wxCommandEvent& event);
		void OnButtonStopClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
