/*******************************************************************************

                                moEffectPlane.cpp

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

#include "moEffectPlane.h"

//========================
//  Factory
//========================

moEffectPlaneFactory *m_EffectPlaneFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectPlaneFactory==NULL)
		m_EffectPlaneFactory = new moEffectPlaneFactory();
	return(moEffectFactory*) m_EffectPlaneFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectPlaneFactory;
	m_EffectPlaneFactory = NULL;
}

moEffect*  moEffectPlaneFactory::Create() {
	return new moEffectPlane();
}

void moEffectPlaneFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectPlane::moEffectPlane() {
	SetName("plane");
}

moEffectPlane::~moEffectPlane() {
	Finish();
}

MOboolean moEffectPlane::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( PLANE_ALPHA, moText("alpha") );
	moDefineParamIndex( PLANE_COLOR, moText("color") );
	moDefineParamIndex( PLANE_SYNC, moText("syncro") );
	moDefineParamIndex( PLANE_PHASE, moText("phase") );
	moDefineParamIndex( PLANE_TEXTURE, moText("texture") );
	moDefineParamIndex( PLANE_BLENDING, moText("blending") );
	moDefineParamIndex( PLANE_WIDTH, moText("width") );
	moDefineParamIndex( PLANE_HEIGHT, moText("height") );
	moDefineParamIndex( PLANE_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( PLANE_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( PLANE_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( PLANE_ROTATEX, moText("rotatex") );
	moDefineParamIndex( PLANE_ROTATEY, moText("rotatey") );
	moDefineParamIndex( PLANE_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( PLANE_SCALEX, moText("scalex") );
	moDefineParamIndex( PLANE_SCALEY, moText("scaley") );
	moDefineParamIndex( PLANE_SCALEZ, moText("scalez") );
	moDefineParamIndex( PLANE_INLET, moText("inlet") );
	moDefineParamIndex( PLANE_OUTLET, moText("outlet") );

	return true;
}

moConfigDefinition *
moEffectPlane::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, PLANE_TEXTURE, moValue("Default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, PLANE_BLENDING, moValue( "0", MO_VALUE_NUM ).Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, PLANE_WIDTH, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, PLANE_HEIGHT, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, PLANE_TRANSLATEX, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, PLANE_TRANSLATEY, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, PLANE_TRANSLATEZ, moValue("-2.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, PLANE_ROTATEX, moValue("-45", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, PLANE_ROTATEY, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, PLANE_ROTATEZ, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, PLANE_SCALEX, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, PLANE_SCALEY, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, PLANE_SCALEZ, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	return p_configdefinition;
}

void moEffectPlane::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    MOint indeximage;
    MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    MOdouble alto, ancho;
    float prop;

    PreDraw( tempogral, parentstate);

    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();
    if ( w == 0 || h == 0 ) { w  = 1; h = 1; prop = 1.0; }
    else {
      prop = (float) h / (float) w;
    }

    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View

    // Cambiar la proyeccion para una vista ortogonal //
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluPerspective(45.0f, 1/prop, 0.1f ,4000.0f);

    // Funcion de blending y de alpha channel //
    glEnable(GL_BLEND);

	glDisable(GL_ALPHA);

	glTranslatef(   m_Config.Eval( moR(PLANE_TRANSLATEX) ),
                  m_Config.Eval( moR(PLANE_TRANSLATEY)),
                  m_Config.Eval( moR(PLANE_TRANSLATEZ))
              );

	glRotatef(  m_Config.Eval( moR(PLANE_ROTATEX) ), 1.0, 0.0, 0.0 );
  glRotatef(  m_Config.Eval( moR(PLANE_ROTATEY) ), 0.0, 1.0, 0.0 );
  glRotatef(  m_Config.Eval( moR(PLANE_ROTATEZ) ), 0.0, 0.0, 1.0 );

	glScalef(   m_Config.Eval( moR(PLANE_SCALEX)),
              m_Config.Eval( moR(PLANE_SCALEX)),
              m_Config.Eval( moR(PLANE_SCALEZ))
            );

  moVector4d color = m_Config.EvalColor(moR(PLANE_COLOR));

	glColor4f(  color.X() * state.tintr,
              color.Y() * state.tintg,
              color.Z() * state.tintb,
              color.W() *
              m_Config.Eval( moR(PLANE_ALPHA) ) * state.alpha );

  glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(PLANE_TEXTURE), &state.tempo ) );

  SetBlending( (moBlendingModes) m_Config.Int( moR(PLANE_BLENDING) ) );

	ancho = (int)m_Config.Eval( moR(PLANE_WIDTH) );
	alto = (int)m_Config.Eval( moR(PLANE_HEIGHT) );

	glBegin(GL_QUADS);
		glTexCoord2f( 0.0, 0.0);
		glVertex2i( -ancho, -alto);

		glTexCoord2f( 1.0, 0.0);
		glVertex2i(  ancho, -alto);

		glTexCoord2f( 1.0, 1.0);
		glVertex2i(  ancho,  alto);

		glTexCoord2f( 0.0, 1.0);
		glVertex2i( -ancho,  alto);
	glEnd();

    // Dejamos todo como lo encontramos //
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

}

MOboolean moEffectPlane::Finish()
{
    return PreFinish();
}


