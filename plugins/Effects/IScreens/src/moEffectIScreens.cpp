/*******************************************************************************

                                moEffectIScreens.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
  *                                                                          *
  *   This code is distributed in the hope that it will be useful, but       *
  *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
  *   General Public License for more details.                               *
  *                                                                          *
  *   A copy of the GNU General Public License is available on the World     *
  *   Wide Web at <http://www.gnu.org/copyleft/gpl.html>. You can also       *
  *   obtain it by writing to the Free Software Foundation,                  *
  *   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.         *
  *                                                                          *
  ****************************************************************************

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andr� Colubri

*******************************************************************************/

#include "moEffectIScreens.h"

//========================
//  Factory
//========================

moIScreensFactory *m_EffectIScreensFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){ 
	if(m_EffectIScreensFactory==NULL)
		m_EffectIScreensFactory = new moIScreensFactory();
	return(moEffectFactory*) m_EffectIScreensFactory;
} 

MO_PLG_API void DestroyEffectFactory(){ 
	delete m_EffectIScreensFactory;
	m_EffectIScreensFactory = NULL;
} 

moEffect*  moIScreensFactory::Create() {
	return new moEffectIScreens();
}

void moIScreensFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectIScreens::moEffectIScreens() {

	devicecode = NULL;
	ncodes = 0;
	m_Name = "iscreens";

}

moEffectIScreens::~moEffectIScreens() {
	Finish();
}

MOboolean moEffectIScreens::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( ISCREENS_ALPHA, moText("alpha") );
	moDefineParamIndex( ISCREENS_COLOR, moText("color") );
	moDefineParamIndex( ISCREENS_SYNC, moText("syncro") );
	moDefineParamIndex( ISCREENS_PHASE, moText("phase") );
	moDefineParamIndex( ISCREENS_INLET, moText("inlet") );
	moDefineParamIndex( ISCREENS_OUTLET, moText("outlet") );	


	g_ViewMode = GL_TRIANGLES;

	//
    if(m_Config.GetPreConfCount() > 0)
        m_Config.PreConfFirst();

	return true;
}

void moEffectIScreens::Draw( moTempo* tempogral,moEffectState* parentstate) 
{
	
    PreDraw( tempogral, parentstate);

    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View
    // Dejamos todo como lo encontramos //
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

}

MOboolean moEffectIScreens::Finish() 
{
    return PreFinish();
}

void moEffectIScreens::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp,*aux;
	MOint did,cid,state,e,valor,estaux,elaux;
	moText txt;

	if (devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);
			if (state)
			switch(i) {
				default:
					break;
			}
		temp = temp->next;
		}
	}

}


moConfigDefinition *
moEffectIScreens::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("param1"), MO_PARAM_FUNCTION, ISCREENS_PARAM1 );
	p_configdefinition->Add( moText("param2"), MO_PARAM_FUNCTION, ISCREENS_PARAM2 );
	return p_configdefinition;
}