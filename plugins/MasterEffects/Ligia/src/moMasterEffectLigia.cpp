/*******************************************************************************

                            moMasterEffectLigia.cpp

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
  Andrés Colubri

*******************************************************************************/

#include "moMasterEffectLigia.h"

//========================
//  Factory
//========================

moMasterEffectLigiaFactory *m_MasterEffectLigiaFactory = NULL;

MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory(){
	if(m_MasterEffectLigiaFactory==NULL)
		m_MasterEffectLigiaFactory = new moMasterEffectLigiaFactory();
	return(moMasterEffectFactory*) m_MasterEffectLigiaFactory;
}

MO_PLG_API void DestroyMasterEffectFactory(){
	delete m_MasterEffectLigiaFactory;
	m_MasterEffectLigiaFactory = NULL;
}

moMasterEffect*  moMasterEffectLigiaFactory::Create() {
	return new moMasterEffectLigia();
}

void moMasterEffectLigiaFactory::Destroy(moMasterEffect* fx) {
	delete fx;
}

//========================
//  EfectoMaestro Ligia
//========================


moMasterEffectLigia::moMasterEffectLigia() {

	SetName("ligia");

	preview = stream = -1;


	done = false;

}

moMasterEffectLigia::~moMasterEffectLigia() {
	Finish();
}

MOboolean moMasterEffectLigia::Init()
{
    if (!PreInit()) return false;

	Images.MODebug = MODebug;
	return true;
}

void moMasterEffectLigia::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Aca van los comandos OpenGL del efecto.
	//if(welcome==MO_FALSE) Welcome();

}


MOboolean moMasterEffectLigia::Finish()
{
    return PreFinish();
}

void
moMasterEffectLigia::Interaction(moIODeviceManager *IODeviceManager) {
	//aqui se parsean los codisp
	//printf("IODeviceManager:%i",IODeviceManager);

	moDeviceCode *temp;
	MOint did,cid,state,valor;

	MOuint j;

	if(devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);

			if(state)
			switch(i) {
				case MO_ACTION_SHUTDOWN_LIGIA_CONSOLE:
					consolestate->finish = MO_TRUE;
					break;
				case MO_ACTION_PAUSE_LIGIA_CONSOLE:
					consolestate->pause*=-1;
					break;
				case MO_ACTION_LIGIA_AUTOMATIC_MODE:
					consolestate->automatic*=-1;
					break;
				case MO_ACTION_REINIT_LIGIA:
					//consolestate->reinit = MO_TRUE;
					break;
				case MO_ACTION_SAVE_SCREEN:
					consolestate->savescreen*=-1;
					break;
				case MO_ACTION_SET_FPS:
					consolestate->setfps*=-1;
					break;
				case MO_ACTION_SHUTDOWN_EFFECTS:
					for( j = 0; j < consolestate->m_nEffects; j++) {
						if(m_pEffectManager->AllEffects().Get( j + m_pEffectManager->PreEffects().Count() + m_pEffectManager->PostEffects().Count() )!=NULL) {
							m_pEffectManager->AllEffects().Get( j + m_pEffectManager->PreEffects().Count() + m_pEffectManager->PostEffects().Count() )->state.on = MO_OFF;
						}
					}
					break;
				case MO_ACTION_SHUTDOWN_PREPOST_EFFECTS:
					for( j = 0; j < consolestate->m_nPreEffects; j++) {
						if(m_pEffectManager->AllEffects().Get(j)!=NULL) {
							m_pEffectManager->AllEffects().Get(j)->state.on = MO_OFF;
						}
					}
					for( j = 0; j < consolestate->m_nPostEffects; j++) {
						if(m_pEffectManager->AllEffects().Get( j + m_pEffectManager->PreEffects().Count() )!=NULL) {
							m_pEffectManager->AllEffects().Get( j + m_pEffectManager->PreEffects().Count() )->state.on = MO_OFF;
						}
					}
					break;
				case MO_ACTION_SHUTDOWN_MASTER_EFFECTS:
					for( j = 0; j < consolestate->m_nMasterEffects;j++) {
					    moEffect* pMEF = m_pEffectManager->AllEffects().Get( m_pEffectManager->AllEffects().Count() - m_pEffectManager->MasterEffects().Count() + j );
					    if (pMEF!=NULL) {
                            if((stricmp( pMEF->GetName(),"ligia")!=0) &&
                                (stricmp( pMEF->GetName(),"masterchannel")!=0) &&
                                (stricmp( pMEF->GetName(),"presetpanel")!=0)) {
                                pMEF->state.on*=-1;
                            }
					    }

					}
					break;
				default:
					break;
			}
		temp = temp->next;
		}
	}

}

moConfigDefinition *
moMasterEffectLigia::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("codes"), MO_PARAM_TEXTURE );
	return p_configdefinition;
}
