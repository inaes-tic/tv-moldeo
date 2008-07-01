/***************************************************************
 * Name:      gmolApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Fabricio Costa Alisedo (fcosta@computaciongrafica.com)
 * Created:   2008-03-02
 * Copyright: Fabricio Costa Alisedo (http://moldeo.computaciongrafica.com)
 * License:
 **************************************************************/

#include "gmolApp.h"

//(*AppHeaders
#include "gmolMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(gmolApp);

bool gmolApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	gmolFrame* Frame = new gmolFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
