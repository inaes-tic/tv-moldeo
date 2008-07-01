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

#define MO_ACTIONHANDLER(F)  m_pNextActionHandler == NULL ? MO_DIRECTOR_STATUS_NO_HANDLER : m_pNextActionHandler->F;
#define MO_ACTIONHANDLER_PROJECT(F)  m_pNextActionHandler == NULL ? moProjectDescriptor() : m_pNextActionHandler->F;
#define MO_ACTIONHANDLER_MOB(F)  m_pNextActionHandler == NULL ? moMobDescriptor() : m_pNextActionHandler->F;
#define MO_ACTIONHANDLER_PARAM(F)  m_pNextActionHandler == NULL ? moParameterDescriptor() : m_pNextActionHandler->F;
#define MO_ACTIONHANDLER_VALUE(F)  m_pNextActionHandler == NULL ? moValueDescriptor() : m_pNextActionHandler->F;

#define MO_ACTIONHANDLER_POINTER(F)  m_pNextActionHandler == NULL ? NULL : m_pNextActionHandler->F;

class moIDirectorActions {

public:

	moIDirectorActions() { m_pNextActionHandler = NULL; }
	virtual ~moIDirectorActions() {}

	void SetNextActionHandler( moIDirectorActions* p_pNextActionHandler ) { m_pNextActionHandler = p_pNextActionHandler; }

public:



private:

	moIDirectorActions*		m_pNextActionHandler;


};


#endif // GMOLAPP_H
