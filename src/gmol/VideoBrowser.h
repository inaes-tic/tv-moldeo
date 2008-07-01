#ifndef VIDEOBROWSER_H
#define VIDEOBROWSER_H

#include "moTypes.h"

#include "RenderPreview.h"

//(*Headers(VideoBrowser)
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
//*)

#include <wx/dataobj.h>

#include "moConsole.h"

class VideoBrowser: public wxPanel
{
	public:

		VideoBrowser(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~VideoBrowser();
        void AddFile( const wxString& filename );
        wxArrayString& GetFiles();
        void SetRenderPreview( RenderPreview *prenderpreview) { m_pRenderPreview = prenderpreview; }

		//(*Declarations(VideoBrowser)
		wxListCtrl* ListCtrl1;
		//*)

	protected:

        wxFileDataObject    m_FileDataObject;
        moConfig            m_Config;
        moResourceManager*  m_pResourceManager;
        RenderPreview*      m_pRenderPreview;

		//(*Identifiers(VideoBrowser)
		static const long ID_LISTCTRL1;
		//*)

	private:


		//(*Handlers(VideoBrowser)
		//*)
		void OnVideoActivated( wxListEvent& levent );

		DECLARE_EVENT_TABLE()
};

#endif
