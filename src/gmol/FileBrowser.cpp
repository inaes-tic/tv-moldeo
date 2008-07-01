#include "FileBrowser.h"

#include <wx/msgdlg.h>

//(*InternalHeaders(FileBrowser)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(FileBrowser)
const long FileBrowser::ID_GENERICDIRCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(FileBrowser,wxPanel)
	//(*EventTable(FileBrowser)
	EVT_TREE_ITEM_ACTIVATED( wxID_ANY, FileBrowser::OnItemActivated)
	//*)
END_EVENT_TABLE()


FileBrowser::FileBrowser(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	m_pVideoBrowser = NULL;

	//(*Initialize(FileBrowser)
	wxFlexGridSizer* FlexGridSizer1;
	
	Create(parent, id, wxDefaultPosition, wxSize(201,176), wxTAB_TRAVERSAL, _T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(1, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	GenericDirCtrl1 = new wxGenericDirCtrl(this, ID_GENERICDIRCTRL1, wxEmptyString, wxDefaultPosition, wxSize(177,132), 0, wxEmptyString, 0, _T("ID_GENERICDIRCTRL1"));
	GenericDirCtrl1->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	GenericDirCtrl1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
	FlexGridSizer1->Add(GenericDirCtrl1, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->SetSizeHints(this);
	//*)
}

FileBrowser::~FileBrowser()
{
	//(*Destroy(FileBrowser)
	//*)
}

void FileBrowser::OnItemActivated(wxTreeEvent& event) {

    //wxMessageBox(wxString("FileBrowser Item activado"),wxString("Mensaje de filebrowser") );
    //en caso de ser: archivo.... agregar al videobrowser
    //wxMessageBox( GenericDirCtrl1->GetPath(), "Activated" );

    if (m_pVideoBrowser) {
        m_pVideoBrowser->AddFile( GenericDirCtrl1->GetPath() );
    }
}

void FileBrowser::SetVideoBrowser( VideoBrowser* pvideobrowser ) {
    m_pVideoBrowser = pvideobrowser;
}

