/***************************************************************
 * Name:      gmolMain.h
 * Purpose:   Defines Application Frame
 * Author:    Fabricio Costa Alisedo (fcosta@computaciongrafica.com)
 * Created:   2008-03-02
 * Copyright: Fabricio Costa Alisedo (http://moldeo.computaciongrafica.com)
 * License:
 **************************************************************/

#ifndef GMOLMAIN_H
#define GMOLMAIN_H

#include "moTypes.h"

//(*Headers(gmolFrame)
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)

#include <wx/aui/aui.h>

#include "moConsole.h"

class gmolFrame: public wxFrame
{
    public:

        gmolFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~gmolFrame();

    private:

        //(*Handlers(gmolFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        //*)

        //(*Identifiers(gmolFrame)
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        static const long ID_TIMER1;
        //*)

        //(*Declarations(gmolFrame)
        wxStatusBar* StatusBar1;
        wxTimer Timer1;
        //*)

        wxAuiManager FrameManager;

        DECLARE_EVENT_TABLE()
};

#endif // GMOLMAIN_H
