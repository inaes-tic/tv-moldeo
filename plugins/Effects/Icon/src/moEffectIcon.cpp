/*******************************************************************************

                               moEffectIcon.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *   (at your option) any later version.                                    *
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

  Copyright (C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andrés Colubri

*******************************************************************************/

#include "moEffectIcon.h"

//========================
//  Factory
//========================

moEffectIconFactory *m_EffectIconoFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if (m_EffectIconoFactory==NULL)
		m_EffectIconoFactory = new moEffectIconFactory();
	return (moEffectFactory*) m_EffectIconoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectIconoFactory;
	m_EffectIconoFactory = NULL;
}

moEffect*  moEffectIconFactory::Create() {
	return new moEffectIcon();
}

void moEffectIconFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectIcon::moEffectIcon() {

	SetName("icon");

}

moEffectIcon::~moEffectIcon() {
	Finish();
}

moConfigDefinition *
moEffectIcon::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, ICON_TEXTURE, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, ICON_BLENDING, moValue( "0", MO_VALUE_NUM ).Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, ICON_WIDTH, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, ICON_HEIGHT, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, ICON_TRANSLATEX, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, ICON_TRANSLATEY, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("rotate"), MO_PARAM_ROTATEZ, ICON_ROTATE );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, ICON_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, ICON_SCALEY );
	return p_configdefinition;
}

MOboolean moEffectIcon::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( ICON_ALPHA, moText("alpha") );
	moDefineParamIndex( ICON_COLOR, moText("color") );
	moDefineParamIndex( ICON_SYNC, moText("syncro") );
	moDefineParamIndex( ICON_PHASE, moText("phase") );
	moDefineParamIndex( ICON_TEXTURE, moText("texture") );
	moDefineParamIndex( ICON_BLENDING, moText("blending") );
	moDefineParamIndex( ICON_WIDTH, moText("width") );
	moDefineParamIndex( ICON_HEIGHT, moText("height") );
	moDefineParamIndex( ICON_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( ICON_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( ICON_ROTATE, moText("rotate") );
	moDefineParamIndex( ICON_SCALEX, moText("scalex") );
	moDefineParamIndex( ICON_SCALEY, moText("scaley") );
	moDefineParamIndex( ICON_INLET, moText("inlet") );
	moDefineParamIndex( ICON_OUTLET, moText("outlet") );

	return true;
}


void moEffectIcon::Draw( moTempo* tempogral, moEffectState* parentstate )
{

    float ancho, alto;
    float prop;

    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();
    if ( w == 0 || h == 0 ) { w  = 1; h = 1; prop = 1.0; }
    else {
      prop = (float) h / (float) w;
    }

    PreDraw( tempogral, parentstate);


    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glLoadIdentity();									// Reset The View

    // Cambiar la proyeccion para una vista ortogonal //
    glDisable(GL_DEPTH_TEST);       // Disables Depth Testing
    glMatrixMode(GL_PROJECTION);    // Select The Projection Matrix
    glLoadIdentity();               // Reset The Projection Matrix
    glOrtho( -0.5, 0.5, -0.5*prop, 0.5*prop, -1, 1);          // Set Up An Ortho Screen


    glEnable(GL_ALPHA);

    ancho = m_Config.Eval( moR(ICON_WIDTH));
    alto = m_Config.Eval( moR(ICON_HEIGHT));

    /// Draw //
    glTranslatef(  m_Config.Eval( moR(ICON_TRANSLATEX)),
                   m_Config.Eval( moR(ICON_TRANSLATEY)),
                   0.0);

    ///solo rotamos en el eje Z (0,0,1) ya que siempre estaremos perpedicular al plano (X,Y)
    glRotatef(  m_Config.Eval( moR(ICON_ROTATE)), 0.0, 0.0, 1.0 );

    glScalef(   m_Config.Eval( moR(ICON_SCALEX)),
                m_Config.Eval( moR(ICON_SCALEY)),
                  1.0);

    SetColor( m_Config[moR(ICON_COLOR)][MO_SELECTED], m_Config[moR(ICON_ALPHA)][MO_SELECTED], state );

    SetBlending( (moBlendingModes) m_Config.Int( moR(ICON_BLENDING) ) );

    glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(ICON_TEXTURE), &state.tempo) );


    glBegin(GL_QUADS);
      glTexCoord2f( 0.0, 1.0 );
      glVertex2f( -0.5*ancho, -0.5*alto);

      glTexCoord2f( 1.0, 1.0 );
      glVertex2f(  0.5*ancho, -0.5*alto);

      glTexCoord2f( 1.0, 0.0 );
      glVertex2f(  0.5*ancho,  0.5*alto);

      glTexCoord2f( 0.0, 0.0 );
      glVertex2f( -0.5*ancho,  0.5*alto);
    glEnd();

    int Tracker = GetInletIndex("TRACKERKLT" );
    if (Tracker > -1) {
      moInlet* pInlet = GetInlets()->Get(Tracker);
      if (pInlet ) {
          if (pInlet->Updated()) {

            moTrackerSystemData* pData = (moTrackerSystemData*) pInlet->GetData()->Pointer();
            if (pData) {
                //MODebug2->Message("Icon > Draw > Drawing Features");
                pData->DrawFeatures( 1.0, -1.0, 0.5, 0.5 );
            }
          }
      }
    }

}

MOboolean moEffectIcon::Finish()
{
    return PreFinish();
}


void
moEffectIcon::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);


}
