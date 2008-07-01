/*******************************************************************************

                              moEffectScene3D.cpp

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

#include "moEffectScene3D.h"

//========================
//  Factory
//========================

moEffectScene3DFactory *m_EffectScene3DFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){ 
	if(m_EffectScene3DFactory==NULL)
		m_EffectScene3DFactory = new moEffectScene3DFactory();
	return(moEffectFactory*) m_EffectScene3DFactory;
} 

MO_PLG_API void DestroyEffectFactory(){ 
	delete m_EffectScene3DFactory;
	m_EffectScene3DFactory = NULL;
} 

moEffect*  moEffectScene3DFactory::Create() {
	return new moEffectScene3D();
}

void moEffectScene3DFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectScene3D::moEffectScene3D() {

	devicecode = NULL;
	ncodes = 0;
	m_Name = "scene3d";

}

moEffectScene3D::~moEffectScene3D() {
	Finish();
}

MOboolean moEffectScene3D::Init() {

    if (!PreInit()) return false;

    // Hacer la inicializacion de este efecto en particular.
	object = m_Config.GetParamIndex("object");
	transx = m_Config.GetParamIndex("translatex");
    transy = m_Config.GetParamIndex("translatey");
    transz = m_Config.GetParamIndex("translatez");
    rotatx = m_Config.GetParamIndex("rotatex");
    rotaty = m_Config.GetParamIndex("rotatey");
    rotatz = m_Config.GetParamIndex("rotatez");
    scalex = m_Config.GetParamIndex("scalex");
    scaley = m_Config.GetParamIndex("scaley");
    scalez = m_Config.GetParamIndex("scalez");

    eyex = m_Config.GetParamIndex("eyex");
    eyey = m_Config.GetParamIndex("eyey");
    eyez = m_Config.GetParamIndex("eyez");
    viewx = m_Config.GetParamIndex("viewx");
    viewy = m_Config.GetParamIndex("viewy");
    viewz = m_Config.GetParamIndex("viewz");


	g_ViewMode = GL_TRIANGLES;

  
	//inicializacion de los objetos y materiales
	Models3d.MODebug = MODebug;
	Models3d.Init(GetConfig(),object,MOModels);

    // Seteos de OpenGL.

    // moText(by Andres)
    moText cargando_str("Cargando preconfigs"); 
    moText finalizado_str("Inicializacion finalizada");

	MODebug->Push(cargando_str);
    if(preconfig.GetPreConfNum() > 0)
        preconfig.PreConfFirst( GetConfig());
	MODebug->Push(finalizado_str);
	return true;
}

void moEffectScene3D::Draw( moTempo* tempogral,moEffectState* parentstate) 
{
	
    PreDraw( tempogral, parentstate);

    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View

    // Cambiar la proyeccion para una vista ortogonal //
	glEnable(GL_DEPTH_TEST);							// Disables Depth Testing
	glDisable(GL_BLEND);
	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	//glOrtho(0,800,0,600,-1,1);							// Set Up An Ortho Screen

// Give OpenGL our position,	then view,		then up vector
	gluPerspective(45.0f,(GLfloat)800/(GLfloat)600, 4.0f ,4000.0f);
	gluLookAt(		mov.movx(m_Config.GetParam(eyex), state.tempo),
					mov.movy(m_Config.GetParam(eyey), state.tempo),
					mov.movz(m_Config.GetParam(eyez), state.tempo),
					mov.movx(m_Config.GetParam(viewx), state.tempo),
					mov.movy(m_Config.GetParam(viewy), state.tempo),
					mov.movz(m_Config.GetParam(viewz), state.tempo),					
					0, 1, 0);
	
    // Draw //
	//BEGIN
	//glBindTexture(GL_TEXTURE_2D, Images.Get(Image,&state.tempo));
	glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
	glTranslatef(   mov.movx(m_Config.GetParam(transx), state.tempo),
                    mov.movy(m_Config.GetParam(transy), state.tempo),
                    mov.movz(m_Config.GetParam(transz), state.tempo) );
	glRotatef(  mov.movx(m_Config.GetParam(rotatx), state.tempo), 1.0, 0.0, 0.0 );
    glRotatef(  mov.movx(m_Config.GetParam(rotaty), state.tempo), 0.0, 1.0, 0.0 );
    glRotatef(  mov.movx(m_Config.GetParam(rotatz), state.tempo), 0.0, 0.0, 1.0 );

	glScalef(   mov.movx(m_Config.GetParam(scalex), state.tempo),
                mov.movx(m_Config.GetParam(scaley), state.tempo),
                mov.movx(m_Config.GetParam(scalez), state.tempo) );   

	

	Models3d.Draw(m_Config.GetCurrentValue(object), &state, GL_TRIANGLES);

	//END
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);						// Allow color
	glDisable(GL_LIGHTING);								// Turn on lighting
	glDisable(GL_LIGHT0);								// Turn on a light with defaults set




    // Dejamos todo como lo encontramos //
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

}

MOboolean moEffectScene3D::Finish() 
{
	Models3d.Finish();

    return PreFinish();
}

