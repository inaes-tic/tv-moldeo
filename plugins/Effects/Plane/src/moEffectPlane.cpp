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

void moEffectPlane::Draw( moTempo* tempogral,moEffectState* parentstate)
{
	MOint indeximage;
    MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    MOdouble alto, ancho;

    PreDraw( tempogral, parentstate);


    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View

    // Cambiar la proyeccion para una vista ortogonal //
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)800/(GLfloat)600, 4.0f ,4000.0f);

    // Funcion de blending y de alpha channel //
    glEnable(GL_BLEND);

	glDisable(GL_ALPHA);

	glTranslatef(   m_Config[moR(PLANE_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(PLANE_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(PLANE_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));

	glRotatef(  m_Config[moR(PLANE_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(PLANE_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(PLANE_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
	glScalef(   m_Config[moR(PLANE_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(PLANE_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(PLANE_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang));

	glColor4f(  m_Config[moR(PLANE_COLOR)][MO_SELECTED][MO_RED].Fun()->Eval(state.tempo.ang) * state.tintr,
                m_Config[moR(PLANE_COLOR)][MO_SELECTED][MO_GREEN].Fun()->Eval(state.tempo.ang) * state.tintg,
                m_Config[moR(PLANE_COLOR)][MO_SELECTED][MO_BLUE].Fun()->Eval(state.tempo.ang) * state.tintb,
				m_Config[moR(PLANE_COLOR)][MO_SELECTED][MO_ALPHA].Fun()->Eval(state.tempo.ang) *
				m_Config[moR(PLANE_ALPHA)].GetData()->Fun()->Eval(state.tempo.ang) * state.alpha);

    //glBindTexture( GL_TEXTURE_2D, Images.GetGLId(indeximage,&state.tempo) );
    moTexture* pImage = (moTexture*) m_Config[moR(PLANE_TEXTURE)].GetData()->Pointer();
    if (pImage!=NULL) {
        if (pImage->GetType()==MO_TYPE_MOVIE) {
            glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
        } else {
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        }
    }

    glBindTexture( GL_TEXTURE_2D, m_Config[moR(PLANE_TEXTURE)].GetData()->GetGLId(&state.tempo, 1, NULL ) );


    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    PosTextX0 = 0.0;
	PosTextX1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordS() :  1.0 );
    PosTextY0 = 0.0;
    PosTextY1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordT() :  1.0 );

	ancho = (int)m_Config[ moR(PLANE_WIDTH) ].GetData()->Fun()->Eval(state.tempo.ang);
	alto = (int)m_Config[ moR(PLANE_HEIGHT) ].GetData()->Fun()->Eval(state.tempo.ang);

	glBegin(GL_QUADS);
		glTexCoord2f( PosTextX0, PosTextY1);
		glVertex2i( -ancho, -alto);

		glTexCoord2f( PosTextX1, PosTextY1);
		glVertex2i(  ancho, -alto);

		glTexCoord2f( PosTextX1, PosTextY0);
		glVertex2i(  ancho,  alto);

		glTexCoord2f( PosTextX0, PosTextY0);
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

moConfigDefinition *
moEffectPlane::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, PLANE_TEXTURE );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, PLANE_WIDTH );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, PLANE_HEIGHT );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, PLANE_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, PLANE_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, PLANE_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, PLANE_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, PLANE_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, PLANE_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, PLANE_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, PLANE_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, PLANE_SCALEZ );
	return p_configdefinition;
}
