#ifndef PLAYPREVIEW_H
#define PLAYPREVIEW_H

//(*Headers(PlayPreview)
#include <wx/panel.h>
//*)

class PlayPreview: public wxPanel
{
	public:

		PlayPreview(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~PlayPreview();

		//(*Declarations(PlayPreview)
		//*)

	protected:

		//(*Identifiers(PlayPreview)
		//*)

	private:

		//(*Handlers(PlayPreview)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
