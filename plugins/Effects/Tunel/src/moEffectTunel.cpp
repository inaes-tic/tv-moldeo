/*******************************************************************************

                              moEffectTunel.cpp

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

#include "moEffectTunel.h"

//========================
//  Factory
//========================

moEffectTunelFactory *m_EffectTunelFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectTunelFactory==NULL)
		m_EffectTunelFactory = new moEffectTunelFactory();
	return(moEffectFactory*) m_EffectTunelFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectTunelFactory;
	m_EffectTunelFactory = NULL;
}

moEffect*  moEffectTunelFactory::Create() {
	return new moEffectTunel();
}

void moEffectTunelFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectTunel::moEffectTunel()
{
	SetName("tunel");
}

moEffectTunel::~moEffectTunel()
{
	Finish();
}


MOboolean moEffectTunel::Init()
{
    if (!PreInit()) return false;

    moDefineParamIndex( TUNEL_ALPHA, moText("alpha") );
	moDefineParamIndex( TUNEL_COLOR, moText("color") );
	moDefineParamIndex( TUNEL_SYNC, moText("syncro") );
	moDefineParamIndex( TUNEL_PHASE, moText("phase") );
	moDefineParamIndex( TUNEL_TEXTURE, moText("texture") );
	moDefineParamIndex( TUNEL_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( TUNEL_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( TUNEL_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( TUNEL_INLET, moText("inlet") );
	moDefineParamIndex( TUNEL_OUTLET, moText("outlet") );

	return true;
}

void moEffectTunel::Draw( moTempo* tempogral,moEffectState* parentstate)
{
	int I, J, texture_a;
	GLdouble C, J1, J2;
	GLdouble Angle, Speed;

    PreDraw( tempogral, parentstate);

	Angle =(state.tempo.getTempo()/2.0);
	Speed =(state.tempo.getTempo()/2.0)*TEXTURE_SPEED;

    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View

    // Funcion de blending y de alpha channel //
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_ALPHA);

    // Cambiar la proyeccion para usar el z-buffer //
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT);


    // Draw
	glTranslatef(   m_Config[moR(TUNEL_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(TUNEL_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(TUNEL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));

    moTexture* pImage = (moTexture*) m_Config[moR(TUNEL_TEXTURE)].GetData()->Pointer();
    if (pImage!=NULL) {
        if (pImage->GetType()==MO_TYPE_MOVIE) {
            glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
        } else {
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        }
    }
    glBindTexture( GL_TEXTURE_2D, m_Config[moR(TUNEL_TEXTURE)].GetData()->GetGLId(&state.tempo, 1, NULL ) );

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	// setup TUNEL coordinates
	for( I=0; I<=12; I++)
		for( J=0; J<=32; J++)
		{
			Tunnels[I][J].X =(3.0 - J/12.0)*cos(2*MO_PI/12.0*I) + 2*sin((Angle+2.0*J)/29) + cos((Angle+2*J)/13) - 2*sin(Angle/29) - cos(Angle/13);
			Tunnels[I][J].Y =(3.0 - J/12.0)*sin(2*MO_PI/12.0*I) + 2*cos((Angle+2.0*J)/33) + sin((Angle+2*J)/17) - 2*cos(Angle/33) - sin(Angle/17);
			Tunnels[I][J].Z = -J;
		}

	// draw TUNEL
	for( J=30; J>=0; J--)
	{
		// precalculate texture v coords for speed
		J1 = J/32.0 + Speed;
		J2 =(J+1.0)/32.0 + Speed;

		// near the end of the TUNEL, fade the effect away
		if(J > 20.0)
			C = 1.0-(J-20.0)/10.0;
		else
			C = 1.0;

	glColor4f(  m_Config[moR(TUNEL_COLOR)][MO_SELECTED][MO_RED].Fun()->Eval(state.tempo.ang) * state.tintr,
                m_Config[moR(TUNEL_COLOR)][MO_SELECTED][MO_GREEN].Fun()->Eval(state.tempo.ang) * state.tintg,
                m_Config[moR(TUNEL_COLOR)][MO_SELECTED][MO_BLUE].Fun()->Eval(state.tempo.ang) * state.tintb,
				m_Config[moR(TUNEL_COLOR)][MO_SELECTED][MO_ALPHA].Fun()->Eval(state.tempo.ang) *
				m_Config[moR(TUNEL_ALPHA)].GetData()->Fun()->Eval(state.tempo.ang) * state.alpha);

		glBegin(GL_QUADS);
		for( I=0; I<=11; I++)
		{
			glTexCoord2f((I-3.0)/12.0, J1); glVertex3f(Tunnels[  I][   J].X, Tunnels[  I][   J].Y, Tunnels[  I][   J].Z);
			glTexCoord2f((I-2.0)/12.0, J1); glVertex3f(Tunnels[I+1][   J].X, Tunnels[I+1][   J].Y, Tunnels[I+1][   J].Z);
			glTexCoord2f((I-2.0)/12.0, J2); glVertex3f(Tunnels[I+1][ J+1].X, Tunnels[I+1][ J+1].Y, Tunnels[I+1][ J+1].Z);
			glTexCoord2f((I-3.0)/12.0, J2); glVertex3f(Tunnels[  I][ J+1].X, Tunnels[  I][ J+1].Y, Tunnels[  I][ J+1].Z);
		}
		glEnd();
	}

    // Dejamos todo como lo encontramos //
    glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

}

MOboolean moEffectTunel::Finish()
{
    return PreFinish();
}


moConfigDefinition *
moEffectTunel::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, TUNEL_TEXTURE, moValue( "default", "TXT")  );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, TUNEL_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, TUNEL_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, TUNEL_TRANSLATEZ );
	return p_configdefinition;
}
