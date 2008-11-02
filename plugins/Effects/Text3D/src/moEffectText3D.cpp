/*******************************************************************************

                              moEffectText3D.cpp

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

#include "moEffectText3D.h"

//========================
//  Factory
//========================

moEffectText3DFactory *m_EffectTextFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectTextFactory==NULL)
		m_EffectTextFactory = new moEffectText3DFactory();
	return(moEffectFactory*) m_EffectTextFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectTextFactory;
	m_EffectTextFactory = NULL;
}

moEffect*  moEffectText3DFactory::Create() {
	return new moEffectText3D();
}

void moEffectText3DFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectText3D::moEffectText3D() {
	SetName("text3d");
}

moEffectText3D::~moEffectText3D() {
	Finish();
}

MOboolean
moEffectText3D::Init()
{
    if (!PreInit()) return false;

	moDefineParamIndex( TEXT3D_ALPHA, moText("alpha") );
	moDefineParamIndex( TEXT3D_COLOR, moText("color") );
	moDefineParamIndex( TEXT3D_SYNC, moText("syncro") );
	moDefineParamIndex( TEXT3D_PHASE, moText("phase") );
	moDefineParamIndex( TEXT3D_FONT, moText("font") );
	moDefineParamIndex( TEXT3D_TEXT, moText("text") );
	moDefineParamIndex( TEXT3D_BLENDING, moText("blending") );
	moDefineParamIndex( TEXT3D_WIDTH, moText("width") );
	moDefineParamIndex( TEXT3D_HEIGHT, moText("height") );
	moDefineParamIndex( TEXT3D_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( TEXT3D_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( TEXT3D_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( TEXT3D_SCALEX, moText("scalex") );
	moDefineParamIndex( TEXT3D_SCALEY, moText("scaley") );
	moDefineParamIndex( TEXT3D_SCALEZ, moText("scalez") );
	moDefineParamIndex( TEXT3D_ROTATEX, moText("rotatex") );
	moDefineParamIndex( TEXT3D_ROTATEY, moText("rotatey") );
	moDefineParamIndex( TEXT3D_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( TEXT3D_EYEX, moText("eyex") );
	moDefineParamIndex( TEXT3D_EYEY, moText("eyey") );
	moDefineParamIndex( TEXT3D_EYEZ, moText("eyez") );
	moDefineParamIndex( TEXT3D_VIEWX, moText("viewx") );
	moDefineParamIndex( TEXT3D_VIEWY, moText("viewy") );
	moDefineParamIndex( TEXT3D_VIEWZ, moText("viewz") );
	moDefineParamIndex( TEXT3D_INLET, moText("inlet") );
	moDefineParamIndex( TEXT3D_OUTLET, moText("outlet") );

	return true;

}

void moEffectText3D::Draw( moTempo* tempogral, moEffectState* parentstate)
{

    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    moFont* pFont = m_Config[moR(TEXT3D_FONT)].GetData()->Font();

    PreDraw( tempogral, parentstate);

    // Cambiar la proyeccion para una vista ortogonal //
//	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
//	glOrtho(0,w,0,h,-1,1);                              // Set Up An Ortho Screen
    m_pResourceManager->GetGLMan()->SetPerspectiveView( w, h );

    glMatrixMode(GL_PROJECTION);
	gluLookAt(		m_Config[moR(TEXT3D_EYEX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(TEXT3D_EYEY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(TEXT3D_EYEZ)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(TEXT3D_VIEWX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(TEXT3D_VIEWY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(TEXT3D_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang),
					0, 1, 0);

    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
	glLoadIdentity();									// Reset The View


    glEnable( GL_CULL_FACE);
	glFrontFace( GL_CCW);

	glEnable( GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    setUpLighting();


    glEnable(GL_BLEND);

	glTranslatef(   m_Config[moR(TEXT3D_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
                    m_Config[moR(TEXT3D_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
                    m_Config[moR(TEXT3D_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));

    //rotation
    glRotatef(  m_Config[moR(TEXT3D_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
    glRotatef(  m_Config[moR(TEXT3D_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(TEXT3D_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );

	//scale
	glScalef(   m_Config[moR(TEXT3D_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(TEXT3D_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(TEXT3D_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang));


    //blending
    SetBlending( (moBlendingModes) m_Config[moR(TEXT3D_BLENDING)][MO_SELECTED][0].Int() );
/*
    //set image
    moTexture* pImage = (moTexture*) m_Config[moR(TEXT3D_TEXTURE)].GetData()->Pointer();
*/
    //color
    SetColor( m_Config[moR(TEXT3D_COLOR)][MO_SELECTED], m_Config[moR(TEXT3D_ALPHA)][MO_SELECTED], state );

	moText Texto = m_Config[moR(TEXT3D_TEXT)][MO_SELECTED][0].Text();

	float r1;
	r1 = 2.0 *((double)rand() /(double)(RAND_MAX+1));


    if (pFont) {
        pFont->Draw(    m_Config[moR(TEXT3D_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
                        m_Config[moR(TEXT3D_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
                        Texto,
                        m_Config[moR(TEXT3D_FONT)][MO_SELECTED][2].Int(),
                        0,
                        m_Config[moR(TEXT3D_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                        m_Config[moR(TEXT3D_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                        m_Config[moR(TEXT3D_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang) );

        //moText infod = moText("screen width:")+IntToStr(w)+moText(" screen height:")+IntToStr(h);
        //pFont->Draw( 0, 0, infod, m_Config[moR(TEXT3D_FONT)][MO_SELECTED][2].Int(), 0, 2.0, 2.0, 0.0);
    }

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}

void moEffectText3D::setUpLighting()
{
   // Set up lighting.
   float light1_ambient[4]  = { 1.0, 1.0, 1.0, 1.0 };
   float light1_diffuse[4]  = { 1.0, 0.9, 0.9, 1.0 };
   float light1_specular[4] = { 1.0, 0.7, 0.7, 1.0 };
   float light1_position[4] = { -1.0, 1.0, 1.0, 0.0 };
   glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
   glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
   glEnable(GL_LIGHT1);

   float light2_ambient[4]  = { 0.2, 0.2, 0.2, 1.0 };
   float light2_diffuse[4]  = { 0.9, 0.9, 0.9, 1.0 };
   float light2_specular[4] = { 0.7, 0.7, 0.7, 1.0 };
   float light2_position[4] = { 1.0, -1.0, -1.0, 0.0 };
   glLightfv(GL_LIGHT2, GL_AMBIENT,  light2_ambient);
   glLightfv(GL_LIGHT2, GL_DIFFUSE,  light2_diffuse);
   glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
   glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
//   glEnable(GL_LIGHT2);

   float front_emission[4] = { 0.3, 0.2, 0.1, 0.0 };
   float front_ambient[4]  = { 0.2, 0.2, 0.2, 0.0 };
   float front_diffuse[4]  = { 0.95, 0.95, 0.8, 0.0 };
   float front_specular[4] = { 0.6, 0.6, 0.6, 0.0 };
   glMaterialfv(GL_FRONT, GL_EMISSION, front_emission);
   glMaterialfv(GL_FRONT, GL_AMBIENT, front_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, front_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, front_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, 16.0);
   glColor4fv(front_diffuse);

   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   glEnable(GL_LIGHTING);
}


MOboolean moEffectText3D::Finish()
{
    return PreFinish();
}

//====================
//
//		CUSTOM
//
//===================


void moEffectText3D::Interaction( moIODeviceManager *IODeviceManager ) {

}


moConfigDefinition *
moEffectText3D::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("font"), MO_PARAM_FONT, TEXT3D_FONT, moValue( "arial.ttf", "TXT", "0", "NUM", "32.0", "NUM") );
	p_configdefinition->Add( moText("text"), MO_PARAM_TEXT, TEXT3D_TEXT, moValue( "Insert text in text parameter", "TXT") );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, TEXT3D_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, TEXT3D_WIDTH, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, TEXT3D_HEIGHT, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, TEXT3D_TRANSLATEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, TEXT3D_TRANSLATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, TEXT3D_TRANSLATEZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, TEXT3D_ROTATEX, moValue( "0.0", "FUNCTION").Ref() );
    p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, TEXT3D_ROTATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, TEXT3D_ROTATEZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, TEXT3D_SCALEX, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, TEXT3D_SCALEY, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, TEXT3D_SCALEZ, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyex"), MO_PARAM_FUNCTION, TEXT3D_EYEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyey"), MO_PARAM_FUNCTION, TEXT3D_EYEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyez"), MO_PARAM_FUNCTION, TEXT3D_EYEZ, moValue( "100.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewx"), MO_PARAM_FUNCTION, TEXT3D_VIEWX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewy"), MO_PARAM_FUNCTION, TEXT3D_VIEWY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewz"), MO_PARAM_FUNCTION, TEXT3D_VIEWZ, moValue( "0.0", "FUNCTION").Ref() );
	return p_configdefinition;
}
