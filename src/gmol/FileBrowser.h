#ifndef FILEBROWSER_H
#define FILEBROWSER_H

//(*Headers(FileBrowser)
#include <wx/sizer.h>
#include <wx/dirctrl.h>
#include <wx/panel.h>
//*)

#include "VideoBrowser.h"

class FileBrowser: public wxPanel
{
	public:

		FileBrowser(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~FileBrowser();
		void SetVideoBrowser( VideoBrowser* pvideobrowser );

		//(*Declarations(FileBrowser)
		wxGenericDirCtrl* GenericDirCtrl1;
		//*)

	protected:

		//(*Identifiers(FileBrowser)
		static const long ID_GENERICDIRCTRL1;
		//*)

        VideoBrowser*       m_pVideoBrowser;

	private:

		//(*Handlers(FileBrowser)
        void OnItemActivated(wxTreeEvent& event);
        //*)

		DECLARE_EVENT_TABLE()
};

#endif
