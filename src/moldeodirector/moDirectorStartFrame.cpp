#include "moDirectorStartFrame.h"
#include "HtmlPanel.h"
#include "moDirectorFrame.h"

BEGIN_EVENT_TABLE(moDirectorStartFrame, wxScrolledWindow )
    EVT_HTML_LINK_CLICKED(-1, moDirectorStartFrame::OnLinkClicked)
    EVT_SIZE( moDirectorStartFrame::OnSize )
END_EVENT_TABLE()

moDirectorStartFrame::moDirectorStartFrame(wxWindow *parent)
       : wxScrolledWindow ( (wxWindow*)parent, wxID_ANY  )
{

    //wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	m_pHtmlWindow = new wxHtmlWindow( this, wxID_ANY );

	//topsizer->Add(m_pHtmlWindow);

	//SetSizer( topsizer );

	//topsizer->SetSizeHints( this );

	//HtmlPanel* pHtmlPanel = new HtmlPanel(this,-1);

    //m_Html = new MyHtmlWindow(this);
    //m_Html->SetRelatedFrame(this, _("HTML : %s"));
#if wxUSE_STATUSBAR
    //m_Html->SetRelatedStatusBar(0);
#endif // wxUSE_STATUSBAR
    //m_Html->ReadCustomization(wxConfig::Get());
    //m_Html->LoadFile(wxFileName(wxT("start.html")));
   // m_Html->SetPage("Hello world");

}

moDirectorStartFrame::~moDirectorStartFrame() {

}

MOboolean
moDirectorStartFrame::Init( moIDirectorActions* pActions ) {

   SetNextActionHandler( pActions );

   wxString pagesrc;

   m_pHtmlWindow->LoadFile(wxFileName("../../doc/start.html"));

   //m_pHtmlWindow->ReadCustomization(wxConfig::Get());

   //pagesrc = _T("<a href=\"http://moldeo.computaciongrafica.com/\">Moldeo Homepage</a>");
   //m_pHtmlWindow->SetPage( pagesrc );

   return true;
}

MOboolean
moDirectorStartFrame::Finish() {

    return true;

}


void moDirectorStartFrame::OnLinkClicked(wxHtmlLinkEvent& event) {

    wxHtmlLinkInfo link( event.GetLinkInfo());

    wxString href = link.GetHref();

    if (href.StartsWith(_T("MO_CMD_")))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, MODIRECTOR_OPENPROJECT );
        wxString pathproject = href.SubString(  7, href.Length()-1);
        evt.SetString( pathproject );
        wxPostEvent( this->GetParent(), evt);
        //return true;
        return;
    }

    if(href.IsSameAs(_T("http://moldeo.computaciongrafica.com/"))
    || href.StartsWith(_T("http")))
    {
        //wxTextDataObject *data = new wxTextDataObject(revInfo);
        //wxTheClipboard->SetData(data);
        wxLaunchDefaultBrowser(link.GetHref());
    }

    if(href.IsSameAs(_T("rev")))
    {
        //wxTextDataObject *data = new wxTextDataObject(revInfo);
        //wxTheClipboard->SetData(data);
        //return true;
        return;
    }

    //return false;
}


void moDirectorStartFrame::OnSize( wxSizeEvent& event ) {

    //wxMessageBox("resize");
    int w,h;
    GetParent()->GetClientSize( &w, &h );
    //wxMessageBox( wxString("parent size:") + moText2Wx(IntToStr(w)) );
    m_pHtmlWindow->SetSize( w, h );
    event.Skip();
}


/*
wxHtmlOpeningStatus MyHtmlWindow::OnOpeningURL(wxHtmlURLType WXUNUSED(type),
                                               const wxString& url,
                                               wxString *WXUNUSED(redirect)) const
{
    GetRelatedFrame()->SetStatusText(url + _T(" lately opened"),1);
    return wxHTML_OPEN;
}

#if wxUSE_CLIPBOARD
BEGIN_EVENT_TABLE(MyHtmlWindow, wxHtmlWindow)
    EVT_TEXT_COPY(wxID_ANY, MyHtmlWindow::OnClipboardEvent)
END_EVENT_TABLE()

void MyHtmlWindow::OnClipboardEvent(wxClipboardTextEvent& WXUNUSED(event))
{
    // explicitly call wxHtmlWindow::CopySelection() method
    // and show the first 100 characters of the text copied in the status bar
    if ( CopySelection() )
    {
        wxTextDataObject data;
        if ( wxTheClipboard && wxTheClipboard->GetData(data) )
        {
            const wxString text = data.GetText();
            const size_t maxTextLength = 100;

            wxLogStatus(wxString::Format(_T("Clipboard: '%s%s'"),
                        wxString(text, maxTextLength).c_str(),
                        (text.length() > maxTextLength) ? _T("...")
                                                        : _T("")));
            return;
        }
    }

    wxLogStatus(_T("Clipboard: nothing"));
}
#endif // wxUSE_CLIPBOARD
*/

