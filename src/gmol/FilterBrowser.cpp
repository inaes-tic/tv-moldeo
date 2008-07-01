#include "FilterBrowser.h"

//(*InternalHeaders(FilterBrowser)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(FilterBrowser)
//*)

BEGIN_EVENT_TABLE(FilterBrowser,wxPanel)
	//(*EventTable(FilterBrowser)
	//*)
END_EVENT_TABLE()

FilterBrowser::FilterBrowser(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(FilterBrowser)
	Create(parent, id, wxDefaultPosition, wxSize(151,152), wxTAB_TRAVERSAL, _T("id"));
	SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
	//*)
}

FilterBrowser::~FilterBrowser()
{
	//(*Destroy(FilterBrowser)
	//*)
}

