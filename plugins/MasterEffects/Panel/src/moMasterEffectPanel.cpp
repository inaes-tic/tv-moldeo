/*******************************************************************************

                        moMasterEffectPanel.cpp

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

#include "moMasterEffectPanel.h"

//========================
//  Factory
//========================

moMasterEffectPanelFactory *m_MasterEffectPanelFactory = NULL;

MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory(){
	if(m_MasterEffectPanelFactory==NULL)
		m_MasterEffectPanelFactory = new moMasterEffectPanelFactory();
	return(moMasterEffectFactory*) m_MasterEffectPanelFactory;
}

MO_PLG_API void DestroyMasterEffectFactory(){
	delete m_MasterEffectPanelFactory;
	m_MasterEffectPanelFactory = NULL;
}

moMasterEffect*  moMasterEffectPanelFactory::Create() {
	return new moMasterEffectPanel();
}

void moMasterEffectPanelFactory::Destroy(moMasterEffect* fx) {
	delete fx;
}

//========================
//  EfectoMaestro
//========================
moMasterEffectPanel::moMasterEffectPanel() {
	m_pEffectManager = NULL;
	consolestate = NULL;
	SetName("panel");
}

moMasterEffectPanel::~moMasterEffectPanel() {
	Finish();
}

MOboolean moMasterEffectPanel::Init()
{
    if (!PreInit()) return false;

/*
    color = m_Config.GetParamIndex("color");
    transx = m_Config.GetParamIndex("TranslateX");
    transy = m_Config.GetParamIndex("TranslateY");
    transz = m_Config.GetParamIndex("TranslateZ");
    rotatx = m_Config.GetParamIndex("RotateX");
    rotaty = m_Config.GetParamIndex("RotateY");
    rotatz = m_Config.GetParamIndex("RotateZ");
    scalex = m_Config.GetParamIndex("ScaleX");
    scaley = m_Config.GetParamIndex("ScaleY");
    scalez = m_Config.GetParamIndex("ScaleZ");
 */
    // Hacer la inicializacion de este efecto en particular.
    // Seteos de OpenGL.
    // Seteos de Texturas.
	return true;
}

void moMasterEffectPanel::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
/*
    glTranslatef(   movx(m_Config.GetParam(transx), tempo),
                    movy(m_Config.GetParam(transy), tempo),
                    movz(m_Config.GetParam(transz), tempo));
    glRotatef(  movx(m_Config.GetParam(rotatx), tempo), 1.0, 0.0, 0.0 );
    glRotatef(  movy(m_Config.GetParam(rotaty), tempo), 0.0, 1.0, 0.0 );
    glRotatef(  movz(m_Config.GetParam(rotatz), tempo), 0.0, 0.0, 1.0 );
    glScale(    movx(m_Config.GetParam(scalex), tempo),
                movy(m_Config.GetParam(scaley), tempo),
                movz(m_Config.GetParam(scalez), tempo));
*/
	/*
    glColor4f(  m_Config.GetParam(color).GetValueFlt(MO_RED)*state.tint,
                m_Config.GetParam(color).GetValueFlt(MO_GREEN*state.tint),
                m_Config.GetParam(color).GetValueFlt(MO_BLUE)*state.tint,
                m_Config.GetParam(color).GetValueFlt(MO_ALPHA)*state.alpha);
    */
    // Aca van los comandos OpenGL del efecto.

}

MOboolean moMasterEffectPanel::Finish()
{
	m_pEffectManager = NULL;
	consolestate = NULL;
	return PreFinish();
}

void
moMasterEffectPanel::Interaction(moIODeviceManager *IODeviceManager) {
	//aqui se parsean los codisp
	moDeviceCode *temp;
	MOint did,cid,state;

	if(devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			if(state) {
				moEffect* pEffect = NULL;
			//switch(i) {
			//	default:
					if(i<=MO_ACTION_SWITCH_EFFECT_39) {

/*
						// Sucio codigo agregado rapidamente para poder asignar los efectos a teclas arbitrarias de las 4 filas
						// del teclado
						int n = m_pEffectManager->Effects().Count();
						for(int j = 0; j < n; j++) {
							pEffect = m_pEffectManager->Effects().Get(j);
							if(pEffect!=NULL) {
								if(pEffect->keyidx == i) {
									pEffect->state.on*=-1;
									break;
								}
							}
						}
						// Fabri, despues implementa mejor esto... ;-)
*/


						// Codigo original:
						pEffect = m_pEffectManager->Effects().Get(i);
						if(pEffect!=NULL) {
							pEffect->state.on*=-1;
						}



					}
					else
					if(i<=MO_ACTION_SWITCH_PRE_EFFECT_11) {
						pEffect =m_pEffectManager->PreEffects().Get(i-MO_ACTION_SWITCH_PRE_EFFECT_0);
						if(pEffect!=NULL) {
							pEffect->state.on*=-1;
						}
					}
					else
					if(i<=MO_ACTION_SWITCH_POST_EFFECT_11) {
						pEffect = m_pEffectManager->PostEffects().Get(i-MO_ACTION_SWITCH_POST_EFFECT_0);
						if( pEffect !=NULL ) {
							pEffect->state.on*=-1;
						}
					}
			//		break;
			//}
			}
		temp = temp->next;
		}
	}
}
