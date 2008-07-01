/***************************************************************
 * Name:      gmolApp.h
 * Purpose:   Defines Application Class
 * Author:    Fabricio Costa Alisedo (fcosta@computaciongrafica.com)
 * Created:   2008-03-02
 * Copyright: Fabricio Costa Alisedo (http://moldeo.computaciongrafica.com)
 * License:
 **************************************************************/

#ifndef GMOLAPP_H
#define GMOLAPP_H

#include "moTypes.h"

#include <wx/app.h>

#include "moConsole.h"

class gmolApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // GMOLAPP_H
