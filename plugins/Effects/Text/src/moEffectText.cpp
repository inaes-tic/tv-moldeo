/*******************************************************************************

                              moEffectText.cpp

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

#include "moEffectText.h"

//========================
//  Factory
//========================

moEffectTextFactory *m_EffectTextFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectTextFactory==NULL)
		m_EffectTextFactory = new moEffectTextFactory();
	return(moEffectFactory*) m_EffectTextFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectTextFactory;
	m_EffectTextFactory = NULL;
}

moEffect*  moEffectTextFactory::Create() {
	return new moEffectText();
}

void moEffectTextFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectText::moEffectText() {
	SetName("text");
}

moEffectText::~moEffectText() {
	Finish();
}

MOboolean
moEffectText::Init()
{
    if (!PreInit()) return false;

	moDefineParamIndex( TEXT_ALPHA, moText("alpha") );
	moDefineParamIndex( TEXT_COLOR, moText("color") );
	moDefineParamIndex( TEXT_SYNC, moText("syncro") );
	moDefineParamIndex( TEXT_PHASE, moText("phase") );
	moDefineParamIndex( TEXT_FONT, moText("font") );
	moDefineParamIndex( TEXT_TEXT, moText("text") );
	moDefineParamIndex( TEXT_BLENDING, moText("blending") );
	moDefineParamIndex( TEXT_WIDTH, moText("width") );
	moDefineParamIndex( TEXT_HEIGHT, moText("height") );
	moDefineParamIndex( TEXT_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( TEXT_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( TEXT_SCALEX, moText("scalex") );
	moDefineParamIndex( TEXT_SCALEY, moText("scaley") );
	moDefineParamIndex( TEXT_ROTATE, moText("rotate") );
	moDefineParamIndex( TEXT_INLET, moText("inlet") );
	moDefineParamIndex( TEXT_OUTLET, moText("outlet") );

	return true;

}

void moEffectText::Draw( moTempo* tempogral, moEffectState* parentstate)
{

    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    moFont* pFont = m_Config[moR(TEXT_FONT)].GetData()->Font();

    PreDraw( tempogral, parentstate);

    // Cambiar la proyeccion para una vista ortogonal //
    glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
    glOrtho( -0.5, 0.5, -0.5*(h/w), 0.5*(h/w), -1, 1);          // Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glLoadIdentity();									// Reset The View


    glEnable(GL_BLEND);


    /// Draw //
    glTranslatef(  m_Config.Eval( moR(TEXT_TRANSLATEX), state.tempo.ang),
                   m_Config.Eval( moR(TEXT_TRANSLATEY), state.tempo.ang),
                   0.0);

    ///solo rotamos en el eje Z (0,0,1) ya que siempre estaremos perpedicular al plano (X,Y)
    glRotatef(  m_Config.Eval( moR(TEXT_ROTATE), state.tempo.ang), 0.0, 0.0, 1.0 );

    glScalef(   m_Config.Eval( moR(TEXT_SCALEX), state.tempo.ang ),
                m_Config.Eval( moR(TEXT_SCALEY), state.tempo.ang ),
                  1.0);

    SetColor( m_Config[moR(TEXT_COLOR)][MO_SELECTED], m_Config[moR(TEXT_ALPHA)][MO_SELECTED], state );

    SetBlending( (moBlendingModes) m_Config.Int( moR(TEXT_BLENDING) ) );

    moText Texto = m_Config.Text( moR(TEXT_TEXT) );

    float r1;
    r1 = 2.0 *((double)rand() /(double)(RAND_MAX+1));


    if (pFont) {
        pFont->Draw(    0.0,
                        0.0,
                        Texto,
                        m_Config[moR(TEXT_FONT)][MO_SELECTED][2].Int(), ///inner font size
                        0,
                        m_Config.Eval( moR(TEXT_SCALEX), state.tempo.ang ),
                        m_Config.Eval( moR(TEXT_SCALEY), state.tempo.ang ),
                        m_Config.Eval( moR(TEXT_ROTATE), state.tempo.ang ) );

        //moText infod = moText("screen width:")+IntToStr(w)+moText(" screen height:")+IntToStr(h);
        //pFont->Draw( 0, 0, infod, m_Config[moR(TEXT_FONT)][MO_SELECTED][2].Int(), 0, 2.0, 2.0, 0.0);
    }

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}

MOboolean moEffectText::Finish()
{
    return PreFinish();
}


void moEffectText::Interaction( moIODeviceManager *IODeviceManager ) {

}


moConfigDefinition *
moEffectText::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("font"), MO_PARAM_FONT, TEXT_FONT, moValue( "Default", "TXT", "0", "NUM", "32.0", "NUM") );
	p_configdefinition->Add( moText("text"), MO_PARAM_TEXT, TEXT_TEXT, moValue( "Insert text in text parameter", MO_VALUE_TXT ) );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, TEXT_BLENDING, moValue( "0", MO_VALUE_NUM ).Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, TEXT_WIDTH, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, TEXT_HEIGHT, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, TEXT_TRANSLATEX, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, TEXT_TRANSLATEY, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("rotate"), MO_PARAM_ROTATEZ, TEXT_ROTATE, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, TEXT_SCALEX, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, TEXT_SCALEY, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	return p_configdefinition;
}
