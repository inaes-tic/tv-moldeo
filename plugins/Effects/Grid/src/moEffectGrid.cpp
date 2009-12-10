/*******************************************************************************

                                moEffectGrid.cpp

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

#include "moEffectGrid.h"

//========================
//  Factory
//========================

moEffectGridFactory *m_EffectGridFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){ 
	if(m_EffectGridFactory==NULL)
		m_EffectGridFactory = new moEffectGridFactory();
	return(moEffectFactory*) m_EffectGridFactory;
} 

MO_PLG_API void DestroyEffectFactory(){ 
	delete m_EffectGridFactory;
	m_EffectGridFactory = NULL;
} 

moEffect*  moEffectGridFactory::Create() {
	return new moEffectGrid();
}

void moEffectGridFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectGrid::moEffectGrid() 
{
	devicecode = NULL;
	ncodes = 0;
	m_Name = "grid";
	
	glActiveTextureARB = NULL;
	glMultiTexCoord2fARB = NULL;
	//Funciones de multitexture
        // wglGetProcAddress reemplazado por glutGetProcAddress(by Andres)
	#ifdef _WIN32
	glActiveTextureARB		=(PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	=(PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
	#else
	glActiveTextureARB		=(PFNGLACTIVETEXTUREARBPROC) glutGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	=(PFNGLMULTITEXCOORD2FARBPROC) glutGetProcAddress("glMultiTexCoord2fARB");
	#endif
	Grid = new TEngine_Utility();
}

moEffectGrid::~moEffectGrid() 
{
	Finish();
}


MOboolean moEffectGrid::Init()
{
    if (!PreInit()) return true;

	MOTextures = m_pResourceManager->GetTextureMan();

    // Hacer la inicializacion de este efecto en particular.
    color = m_Config.GetParamIndex("color");
    textura = m_Config.GetParamIndex("textura");
	texturab = m_Config.GetParamIndex("texturab");
    
	// Seteos de Texturas.
	moText strimage;
	MOint id;

    m_Config.SetCurrentParamIndex(textura);
    m_Config.FirstValue();
	ntextures = m_Config.GetParam(textura).GetValuesCount();
    textures = new MOtexture[ntextures];

    for(MOuint i=0; i<ntextures; i++) 
    {
		strimage = m_Config.GetParam().GetValue().GetSubValue(0).Text();
		id = MOTextures->GetTextureMOId(strimage, true);
        textures[i] = MOTextures->GetGLId(id);
        m_Config.NextValue();
    }



	//Draw
	Grid->Start_Engine();


    // Seteos generales
    if(preconfig.GetPreConfNum() > 0)
        preconfig.PreConfFirst( GetConfig());

	return true;
}

void moEffectGrid::Draw( moTempo* tempogral,moEffectState* parentstate) 
{
	int I, J, textura_a, textura_b;
	
    PreDraw( tempogral, parentstate);

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
    // Funcion de blending y de alpha channel //

  glDisable(GL_BLEND);
  glDisable(GL_ALPHA);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glDisable(GL_DEPTH_TEST);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	textura_a = m_Config.GetCurrentValueIndex(textura);               // textura actual.
    glBindTexture(GL_TEXTURE_2D, textures[textura_a]);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	textura_b = m_Config.GetCurrentValueIndex(texturab);
	glBindTexture(GL_TEXTURE_2D, textures[textura_b]);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
    glColor4f(  m_Config.GetParam(color).GetValue().GetSubValue(MO_RED).Float()*state.tintr,
                    m_Config.GetParam(color).GetValue().GetSubValue(MO_GREEN).Float()*state.tintg,
                    m_Config.GetParam(color).GetValue().GetSubValue(MO_BLUE).Float()*state.tintb,
                    m_Config.GetParam(color).GetValue().GetSubValue(MO_ALPHA).Float()*state.alpha);
	
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	Grid->Loop_Engine();

	glActiveTextureARB(GL_TEXTURE0_ARB);
    textura_a = m_Config.GetCurrentValueIndex(textura);               // textura actual.
    glBindTexture(GL_TEXTURE_2D, textures[textura_a]);

	glDisable(GL_ACTIVE_TEXTURE_ARB);
  //glEnable(GL_BLEND);
  //glEnable(GL_ALPHA);

	// Dejamos todo como lo encontramos //
    glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}

void
moEffectGrid::Interaction(moIODeviceManager *IODeviceManager) {
	//aqui se parsean los codisp
	moDeviceCode *temp,*aux;
	MOint did,cid,state,e,valor,estaux,elaux;

	if(devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->array[did]->GetStatus(cid);
			valor = IODeviceManager->array[did]->GetValue(cid);
			if(state)
			switch(i) {
				case MO_GRID_PITCH:
					Grid->Pitch(valor);
					MODebug->Push(IntToStr(valor));
					break;
				case MO_GRID_TRIM:
					Grid->Trim(valor);
					//MODebug->Push(IntToStr(valor));
					break;
				case MO_GRID_UP:
					Grid->Check_Keyb(2);
					break;
				case MO_GRID_DOWN:
					Grid->Check_Keyb(3);
					break;
				case MO_GRID_TURN_LEFT:
					Grid->Check_Keyb(0);
					break;
				case MO_GRID_TURN_RIGHT:
					Grid->Check_Keyb(1);
					break;
				case MO_GRID_TILT_LEFT:
					Grid->Check_Keyb(5);
					break;
				case MO_GRID_TILT_RIGHT:
					Grid->Check_Keyb(4);
					break;
				case MO_GRID_FORWARD:
					Grid->Check_Keyb(6);
					break;
				case MO_GRID_BACK:
					Grid->Check_Keyb(7);
					break;
			}
		temp = temp->next;
		}
	}
}

MOboolean moEffectGrid::Finish()
{
	Grid->Stop_Engine();
	delete Grid;

    if(textures != NULL)
        delete[] textures;

    return PreFinish();
}
