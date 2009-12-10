/*******************************************************************************

                      moMasterEffectPresetPanel.cpp

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

#include "moMasterEffectPresetPanel.h"

//========================
//  Factory
//========================

moMasterEffectPresetPanelFactory *m_MasterEffectPresetPanelFactory = NULL;

MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory(){
	if(m_MasterEffectPresetPanelFactory==NULL)
		m_MasterEffectPresetPanelFactory = new moMasterEffectPresetPanelFactory();
	return(moMasterEffectFactory*) m_MasterEffectPresetPanelFactory;
}

MO_PLG_API void DestroyMasterEffectFactory(){
	delete m_MasterEffectPresetPanelFactory;
	m_MasterEffectPresetPanelFactory = NULL;
}

moMasterEffect*  moMasterEffectPresetPanelFactory::Create() {
	return new moMasterEffectPresetPanel();
}

void moMasterEffectPresetPanelFactory::Destroy(moMasterEffect* fx) {
	delete fx;
}

//========================
//  EfectoMaestro
//========================
moMasterEffectPresetPanel::moMasterEffectPresetPanel() {
	m_pEffectManager = NULL;
	consolestate = NULL;
	presets = NULL;
	SetName("presetpanel");
}

moMasterEffectPresetPanel::~moMasterEffectPresetPanel() {
	Finish();
}

MOboolean
moMasterEffectPresetPanel::Init() {
	MOuint i;

    if (!PreInit()) return false;


	presets = new moPresets* [m_pEffectManager->AllEffects().Count()];
	for( i=0; i<MO_MAX_PRESETS; i++) {
		states[i].Init();
	}
	for( i=0; i<m_pEffectManager->AllEffects().Count(); i++) {
		if(m_pEffectManager->AllEffects().Get(i)!=NULL)
			presets[i] = new moPresets(MO_MAX_PRESETS,m_pEffectManager->AllEffects().Get(i)->GetConfig()->GetParamsCount());
		else
			presets[i] = NULL;

	}

    //color = m_Config.GetParamIndex("color");
    //transx = m_Config.GetParamIndex("TranslateX");
    //transy = m_Config.GetParamIndex("TranslateY");
    //transz = m_Config.GetParamIndex("TranslateZ");
    //rotatx = m_Config.GetParamIndex("RotateX");
    //rotaty = m_Config.GetParamIndex("RotateY");
    //rotatz = m_Config.GetParamIndex("RotateZ");
    //scalex = m_Config.GetParamIndex("ScaleX");
    //scaley = m_Config.GetParamIndex("ScaleY");
    //scalez = m_Config.GetParamIndex("ScaleZ");

    // Hacer la inicializacion de este efecto en particular.
    // Seteos de OpenGL.
    // Seteos de Texturas.
	return true;
}

void moMasterEffectPresetPanel::Draw( moTempo* tempogral,moEffectState* parentstate)
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

void
moMasterEffectPresetPanel::SavePreset(MOint presetid) {
    MOuint i;
    //Consola.SalvarEstado(preset);
    //for(i=0;i<nm_pEffectManager;i++) m_pEffectManager[i]->SalvarEstado(preset);
    states[presetid]  = *consolestate;
	for(i=0; i < m_pEffectManager->AllEffects().Count(); i++) {
		if(presets[i]!=NULL)
		presets[i]->Save(presetid,m_pEffectManager->AllEffects().Get(i)->GetConfig(),&m_pEffectManager->AllEffects().Get(i)->state);
	}

}

void
moMasterEffectPresetPanel::LoadPreset(MOint presetid) {
	MOuint i;

	for(i=0; i<m_pEffectManager->AllEffects().Count(); i++) {
		if(presets[i]!=NULL) {
				if(presets[i]->setting[presetid]) *consolestate = states[presetid];
				presets[i]->Load(presetid,m_pEffectManager->AllEffects().Get(i)->GetConfig(),&m_pEffectManager->AllEffects().Get(i)->state);
		}
	}
}


MOboolean moMasterEffectPresetPanel::Finish()
{
	MOuint i;
	if(presets!=NULL) {
		for(i=0;i<m_pEffectManager->AllEffects().Count();i++) {
			delete presets[i];
			presets[i] = NULL;
		}
		delete [] presets;
		presets = NULL;
	}
    return PreFinish();
}

void
moMasterEffectPresetPanel::Interaction(moIODeviceManager *IODeviceManager) {
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
			//switch(i) {
			//	default:
					if(i<=MO_ACTION_PRESET_8_SAVE) {
						SavePreset(i);
					}
					else
						if(i<=MO_ACTION_PRESET_8_LOAD) {
						LoadPreset(i-MO_ACTION_PRESET_0_LOAD);
					}
			//		break;
			//}
			}
		temp = temp->next;
		}
	}
}
