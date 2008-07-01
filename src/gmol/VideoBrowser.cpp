#include "VideoBrowser.h"

//(*InternalHeaders(VideoBrowser)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/filename.h>
#include <wx/msgdlg.h>

//(*IdInit(VideoBrowser)
const long VideoBrowser::ID_LISTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(VideoBrowser,wxPanel)
	//(*EventTable(VideoBrowser)
	//*)
	EVT_LIST_ITEM_ACTIVATED( wxID_ANY, VideoBrowser::OnVideoActivated )
END_EVENT_TABLE()

VideoBrowser::VideoBrowser(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(VideoBrowser)
	wxFlexGridSizer* FlexGridSizer1;
	
	Create(parent, id, wxDefaultPosition, wxSize(321,381), wxTAB_TRAVERSAL|wxCLIP_CHILDREN, _T("id"));
	SetMinSize(wxSize(200,400));
	SetMaxSize(wxSize(-1,-1));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
	FlexGridSizer1 = new wxFlexGridSizer(1, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	ListCtrl1 = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxSize(198,202), 0, wxDefaultValidator, _T("ID_LISTCTRL1"));
	ListCtrl1->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	ListCtrl1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
	FlexGridSizer1->Add(ListCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->SetSizeHints(this);
	//*)

    m_Config.LoadConfig( moText( "../../data/gmol/gmol.mol" ) );
    m_pResourceManager = new moResourceManager();


}

VideoBrowser::~VideoBrowser()
{
	//(*Destroy(VideoBrowser)
	//*)
}

void VideoBrowser::OnVideoActivated( wxListEvent& levent ) {

    //try to open the video...
    //in moldeo console??!!??!
    long itemindex = levent.GetIndex();

    wxMessageBox(m_FileDataObject.GetFilenames()[itemindex]);

    if (!m_pResourceManager->Initialized()) {
        m_pResourceManager->Init( moText("../../data/gmol/"),
                                  m_Config,
                                    0,
                                    800,
                                    600,
                                    800,
                                    600);
    }

    m_pRenderPreview->Init();



}

void VideoBrowser::AddFile( const wxString& filename ) {

    wxFileName FileName(filename);

    if (wxFileName::IsFileReadable(filename)) {
        FileName.Assign(filename);
        wxString extension = FileName.GetExt();
        if ( extension==_T("mpg") ||  extension==_T("mpeg") ||  extension==_T("mov") ||  extension==_T("avi")) {
            m_FileDataObject.AddFile( FileName.GetFullPath() );
            ListCtrl1->InsertItem( m_FileDataObject.GetFilenames().Count(), FileName.GetFullName());
            //ListCtrl1->SetItem(m_FileDataObject.GetFilenames().Count(),0,FileName.GetFullName());
            wxMessageBox(_T("Video added, checking type"));
        } else {
            wxMessageBox(_T("Movie format unknown"));
        }
    }
}

