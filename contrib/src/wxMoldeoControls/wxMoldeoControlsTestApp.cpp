/***************************************************************
 * Name:      wxMoldeoControlsTestApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Fabricio Costa Alisedo (fcosta@computaciongrafica.com)
 * Created:   2009-03-17
 * Copyright: Fabricio Costa Alisedo (http://ww.moldeo.org)
 * License:
 **************************************************************/

#include "wxMoldeoControlsTestApp.h"

//(*AppHeaders
#include "wxMoldeoControlsTestDialog.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(wxMoldeoControlsTestApp);

bool wxMoldeoControlsTestApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	wxMoldeoControlsTestDialog Dlg(0);
    	SetTopWindow(&Dlg);
    	Dlg.ShowModal();
    	wxsOK = false;
    }
    //*)
    return wxsOK;

}
