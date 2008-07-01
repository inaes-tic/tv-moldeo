#include "PlayPreview.h"

//(*InternalHeaders(PlayPreview)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(PlayPreview)
//*)

BEGIN_EVENT_TABLE(PlayPreview,wxPanel)
	//(*EventTable(PlayPreview)
	//*)
END_EVENT_TABLE()

PlayPreview::PlayPreview(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(PlayPreview)
	Create(parent, id, wxDefaultPosition, wxSize(320,240), wxTAB_TRAVERSAL, _T("id"));
	//*)
}

PlayPreview::~PlayPreview()
{
	//(*Destroy(PlayPreview)
	//*)
}

