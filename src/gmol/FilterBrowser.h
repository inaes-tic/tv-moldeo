#ifndef FILTERBROWSER_H
#define FILTERBROWSER_H

//(*Headers(FilterBrowser)
#include <wx/panel.h>
//*)

class FilterBrowser: public wxPanel
{
	public:

		FilterBrowser(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~FilterBrowser();

		//(*Declarations(FilterBrowser)
		//*)

	protected:

		//(*Identifiers(FilterBrowser)
		//*)

	private:

		//(*Handlers(FilterBrowser)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
