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
	SetName("lights");

}

moEffectLights::~moEffectLights() {
	Finish();
}

MOboolean
moEffectLights::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( LIGHTS_ALPHA, moText("alpha") );
	moDefineParamIndex( LIGHTS_COLOR, moText("color") );
	moDefineParamIndex( LIGHTS_SYNC, moText("syncro") );
	moDefineParamIndex( LIGHTS_PHASE, moText("phase") );
	moDefineParamIndex( LIGHTS_TEXTURE, moText("texture") );
	moDefineParamIndex( LIGHTS_NUMBER, moText("number") );
	moDefineParamIndex( LIGHTS_BLENDING, moText("blending") );
	moDefineParamIndex( LIGHTS_DIVISOR, moText("divisor") );
	moDefineParamIndex( LIGHTS_FACTORX, moText("factorx") );
	moDefineParamIndex( LIGHTS_FACTORY, moText("factory") );
	moDefineParamIndex( LIGHTS_FACTORZ, moText("factorz") );
	moDefineParamIndex( LIGHTS_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( LIGHTS_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( LIGHTS_SCALEX, moText("scalex") );
	moDefineParamIndex( LIGHTS_SCALEY, moText("scaley") );
	moDefineParamIndex( LIGHTS_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( LIGHTS_INLET, moText("inlet") );
	moDefineParamIndex( LIGHTS_OUTLET, moText("outlet") );

    m_Config.PreConfFirst();

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

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);


    // Draw //
	glTranslatef(  m_Config[moR(LIGHTS_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
                   m_Config[moR(LIGHTS_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
					0.0);

    glRotatef(  m_Config[moR(LIGHTS_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
	glScalef(   m_Config[moR(LIGHTS_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(LIGHTS_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                1.0);

    SetColor( m_Config[moR(LIGHTS_COLOR)][MO_SELECTED], m_Config[moR(LIGHTS_ALPHA)][MO_SELECTED], state );

    SetBlending( (moBlendingModes) m_Config[moR(LIGHTS_BLENDING)][MO_SELECTED][0].Int() );

    moTexture* pImage = (moTexture*) m_Config[moR(LIGHTS_TEXTURE)].GetData()->Pointer();

    glBindTexture( GL_TEXTURE_2D, m_Config[moR(LIGHTS_TEXTURE)].GetData()->GetGLId(&state.tempo) );


	divf = m_Config[moR(LIGHTS_DIVISOR)].GetData()->Fun()->Eval(state.tempo.ang);
	facx = m_Config[moR(LIGHTS_FACTORX)].GetData()->Fun()->Eval(state.tempo.ang);
	facy = m_Config[moR(LIGHTS_FACTORY)].GetData()->Fun()->Eval(state.tempo.ang);
	facz = m_Config[moR(LIGHTS_FACTORZ)].GetData()->Fun()->Eval(state.tempo.ang);

	angulo = 0.0f;
	radius = 2.0f;

	if (m_Config[moR(LIGHTS_NUMBER)][MO_SELECTED][0].Int()!=Lights.Count()) {
	    Lights.Init( m_Config[moR(LIGHTS_NUMBER)][MO_SELECTED][0].Int(), moCoord() );
    }

    for( i=0; i<Lights.Count(); i++ ) {
        //angulo+=5.0f*pi/divf;
        angulo+= 5.0f * moMathf::PI / 10.0f;
        Lights[i].Position = moVector3f( cos(angulo)*radius, sin(angulo)*radius, -10.0f );
    }


	for(i=0;i<Lights.Count();i++) {
		//angulo+=pi/(nlights)/(divf/5.0);
		angulo+= moMathf::PI / ( Lights.Count() / divf );
		Lights[i].Normal = moVector3f( Lights[i].Position.X() + cos(angulo)*facx, Lights[i].Position.Y() + sin(angulo)*facy, Lights[i].Position.Z() );

		glLoadIdentity();
		glTranslatef(0.0,0.0,0.0);
		glScalef(1.0*state.magnitude*cos(state.tempo.ang),1.0*state.magnitude*cos(state.tempo.ang),1.0*state.magnitude);
		glRotatef(60*state.tempo.ang, 0.0, 0.0, 1.0 );

		glTranslatef( Lights[i].Normal.X() , Lights[i].Normal.Y() , Lights[i].Normal.Z()  );

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
    return PreFinish();
}


moConfigDefinition *
moEffectLights::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("number"), MO_PARAM_NUMERIC, LIGHTS_NUMBER, moValue( "60", "NUM").Ref() );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, LIGHTS_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, LIGHTS_TEXTURE, moValue( "default", "TXT") );

	p_configdefinition->Add( moText("factorx"), MO_PARAM_FUNCTION, LIGHTS_FACTORX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("factory"), MO_PARAM_FUNCTION, LIGHTS_FACTORY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("factorz"), MO_PARAM_FUNCTION, LIGHTS_FACTORZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("divisor"), MO_PARAM_FUNCTION, LIGHTS_DIVISOR, moValue( "1.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, LIGHTS_TRANSLATEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, LIGHTS_TRANSLATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, LIGHTS_ROTATEZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, LIGHTS_SCALEX, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, LIGHTS_SCALEY, moValue( "1.0", "FUNCTION").Ref() );
	return p_configdefinition;
}
