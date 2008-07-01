/*******************************************************************************

                              moEffectLights.cpp

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

#include "moEffectLights.h"

//========================
//  Factory
//========================

moEffectLightsFactory *m_EffectLightsFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){ 
	if(m_EffectLightsFactory==NULL)
		m_EffectLightsFactory = new moEffectLightsFactory();
	return(moEffectFactory*) m_EffectLightsFactory;
} 

MO_PLG_API void DestroyEffectFactory(){ 
	delete m_EffectLightsFactory;
	m_EffectLightsFactory = NULL;
} 

moEffect*  moEffectLightsFactory::Create() {
	return new moEffectLights();
}

void moEffectLightsFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectLights::moEffectLights() {
	devicecode = NULL;
	ncodes = 0;

	Images = NULL;
	m_Name = "lights";

}

moEffectLights::~moEffectLights() {
	Finish();
}

MOboolean
moEffectLights::Init() {

	MOuint i;

    if (!PreInit()) return false;

    // Hacer la inicializacion de este efecto en particular.
    color = m_Config.GetParamIndex("color");
	configuration = m_Config.GetParamIndex("configuration");
    images = m_Config.GetParamIndex("textura");
	divisorf = m_Config.GetParamIndex("divisorf");
	factorx = m_Config.GetParamIndex("factorx");
	factory = m_Config.GetParamIndex("factory");
	factorz = m_Config.GetParamIndex("factorz");



    //Seteos de Texturas.
	m_Config.SetCurrentParamIndex(images);
	m_Config.FirstValue();
	nimages = m_Config.GetParam(images).GetValuesCount();
	Images = new  MOtexture [nimages];
	for(i = 0; i < nimages; i++) {
		Images[i] = m_pResourceManager->GetTextureMan()->GetTexture(m_Config.GetParam().GetValue().GetSubValue(0).Text());
		m_Config.NextValue();
	}

	radius = 2.0f;
	MOfloat angulo = 0.0f;

	nlights = 60;

	for(i=0;i<nlights;i++) {
		angulo+=5.0f*pi/10.0f;
		LIGHTS[i].X = cos(angulo)*radius;
		LIGHTS[i].Y = sin(angulo)*radius;
		LIGHTS[i].Z = -10.0f;
	}

	//Image = Images[0];
    if(preconfig.GetPreConfNum() > 0)
        preconfig.PreConfFirst( GetConfig());
	return true;
}

void moEffectLights::Draw( moTempo* tempogral,moEffectState* parentstate)
{
	MOfloat divf,facx,facy,facz;
	MOuint i;
	//MOfloat r;//random
	float angulo;

    
	PreDraw( tempogral, parentstate);

    glMatrixMode( GL_PROJECTION );
	glPushMatrix();
    glMatrixMode( GL_MODELVIEW );
	glPushMatrix();


	glLoadIdentity();

    glColor4f(  m_Config.GetParam(color).GetValue().GetSubValue(MO_RED).Float()*state.tintr,
				m_Config.GetParam(color).GetValue().GetSubValue(MO_GREEN).Float()*state.tintg,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_BLUE).Float()*state.tintb,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_ALPHA).Float()*state.alpha);
			
/*
    glTranslatef(   mov.movx(m_Config.GetParam(transx), state.tempo),
                    mov.movy(m_Config.GetParam(transy), state.tempo),
                    mov.movz(m_Config.GetParam(transz), state.tempo));
*/
/*
	glRotatef(  mov.movx(m_Config.GetParam(rotatx), state.tempo), 1.0, 0.0, 0.0 );
    glRotatef(  mov.movy(m_Config.GetParam(rotaty), state.tempo), 0.0, 1.0, 0.0 );
    glRotatef(  mov.movz(m_Config.GetParam(rotatz), state.tempo), 0.0, 0.0, 1.0 );
	glScalef(   mov.movx(m_Config.GetParam(scalex), state.tempo),
                mov.movy(m_Config.GetParam(scaley), state.tempo),
                mov.movz(m_Config.GetParam(scalez), state.tempo));
*/
	divf = m_Config.GetParam(divisorf).GetValue().GetSubValue(0).Float();
	facx = mov.movx(m_Config.GetParam(factorx), state.tempo);
	facy = mov.movy(m_Config.GetParam(factory), state.tempo);
	facz = mov.movz(m_Config.GetParam(factorz), state.tempo);
	
	Image = m_Config.GetCurrentValueIndex(images);

	glDisable(GL_DEPTH_TEST);	
	glDisable(GL_ALPHA);
	glDisable(GL_CULL_FACE);	
	glEnable(GL_BLEND);		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glBindTexture(GL_TEXTURE_2D, Images[Image]);
  
	angulo = 0.0f;
	radius = 2.0f;
	
	for(i=0;i<nlights;i++) {
		//angulo+=5.0f*pi/divf;
		angulo+=5.0f*pi/10.0f;
		LIGHTS[i].X = cos(angulo)*radius;
		LIGHTS[i].Y = sin(angulo)*radius;
		LIGHTS[i].Z = -10.0f;
	}

	for(i=0;i<nlights;i++) {
		//angulo+=pi/(nlights)/(divf/5.0);
		angulo+=pi/(nlights/ divf);		
		LIGHTS[i].XN = LIGHTS[i].X+cos(angulo)*facx; 
		LIGHTS[i].YN = LIGHTS[i].Y+sin(angulo)*facy;
		LIGHTS[i].ZN = LIGHTS[i].Z;

		glLoadIdentity();
		glTranslatef(0.0,0.0,0.0);
		glScalef(1.0*state.magnitude*cos(state.tempo.ang),1.0*state.magnitude*cos(state.tempo.ang),1.0*state.magnitude);
		glRotatef(60*state.tempo.ang, 0.0, 0.0, 1.0 );
		
		glTranslatef(LIGHTS[i].XN,LIGHTS[i].YN,LIGHTS[i].ZN);
		
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0);
			glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, 0);
			glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, 0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, 0);
		glEnd();
	}
	
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
}

MOboolean moEffectLights::Finish()
{
	if(Images!=NULL)	delete [] Images;
	Images = NULL;
    return PreFinish();
}
