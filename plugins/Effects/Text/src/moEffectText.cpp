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
	moDefineParamIndex( TEXT_TEXTURE, moText("font") );
	moDefineParamIndex( TEXT_TEXT, moText("text") );
	moDefineParamIndex( TEXT_BLENDING, moText("blending") );
	moDefineParamIndex( TEXT_WIDTH, moText("width") );
	moDefineParamIndex( TEXT_HEIGHT, moText("height") );
	moDefineParamIndex( TEXT_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( TEXT_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( TEXT_SCALEX, moText("scalex") );
	moDefineParamIndex( TEXT_SCALEY, moText("scaley") );
	moDefineParamIndex( TEXT_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( TEXT_INLET, moText("inlet") );
	moDefineParamIndex( TEXT_OUTLET, moText("outlet") );

    Tx = Ty = Rz = 0.0;
	Sx = Sy = 1.0;
    //m_pFont = (moTexture*) m_Config[moR(TEXT_TEXTURE)].GetData()->Pointer();
    FontId = m_Config[moR(TEXT_TEXTURE)].GetData()->GetGLId(&state.tempo, 1, NULL);
	BuildFont();
	return true;

}

void moEffectText::Draw( moTempo* tempogral, moEffectState* parentstate)
{

    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    PreDraw( tempogral, parentstate);


    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
	glLoadIdentity();									// Reset The View
/*
    // Cambiar la proyeccion para una vista ortogonal //
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,w,0,h,-1,1);                              // Set Up An Ortho Screen


    // Funcion de blending y de alpha channel //
    glEnable(GL_BLEND);

	glDisable(GL_ALPHA);

    // Draw //
	glTranslatef(  ( m_Config[moR(TEXT_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang)+Tx )*w,
                   ( m_Config[moR(TEXT_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang)+Ty )*h,
					0.0);

    //rotation
    glRotatef(  m_Config[moR(TEXT_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );

	//scale
	glScalef(   m_Config[moR(TEXT_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang)*Sx,
                m_Config[moR(TEXT_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang)*Sy,
               1.0);

    //color
    SetColor( m_Config[moR(TEXT_COLOR)][MO_SELECTED], m_Config[moR(TEXT_ALPHA)][MO_SELECTED] );

    //blending
    SetBlending( (moBlendingModes) m_Config[moR(TEXT_BLENDING)][MO_SELECTED][0].Int() );

    //set image
    moTexture* pImage = (moTexture*) m_Config[moR(TEXT_TEXTURE)].GetData()->Pointer();


//====
	moTexture* prevFont;

	prevFont = m_pFont;

	m_pFont = pImage;

	if ( m_pFont != prevFont ) {

		KillFont();

		BuildFont();

	}*/


    //color
    SetColor( m_Config[moR(TEXT_COLOR)][MO_SELECTED], m_Config[moR(TEXT_ALPHA)][MO_SELECTED], state );


	moText Texto = m_Config[moR(TEXT_TEXT)][MO_SELECTED][0].Text();

	float r1;
	r1 = 2.0 *((double)rand() /(double)(RAND_MAX+1));

	//ancho = (int)m_Config[ moR(TEXT_WIDTH) ].GetData()->Fun()->Eval(state.tempo.ang)* (float)(w/800.0);
	//alto = (int)m_Config[ moR(TEXT_HEIGHT) ].GetData()->Fun()->Eval(state.tempo.ang)* (float)(h/600.0);

	glPrint( m_Config[moR(TEXT_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
            m_Config[moR(TEXT_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
            Texto,
            0,
            m_Config[moR(TEXT_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
            m_Config[moR(TEXT_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
            m_Config[moR(TEXT_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang) );
//====




}

MOboolean moEffectText::Finish()
{
    return PreFinish();
}

//====================
//
//		CUSTOM
//
//===================


GLvoid
moEffectText::BuildFont(void)								// Build Our Font Display List
{
	float	cx;											// Holds Our X Character Coord
	float	cy;											// Holds Our Y Character Coord
    GLint loop;
	m_GLBaseList = glGenLists(256);								// Creating 256 Display Lists
    if (m_pFont)
        glBindTexture( GL_TEXTURE_2D, FontId );
    else
        glBindTexture( GL_TEXTURE_2D, 0 );
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	for(loop=0; loop<256; loop++)						// Loop Through All 256 Lists
	{
		cx=float((int)(loop%16))/16.0f;						// X Position Of Current Character
		cy=float((int)(loop/16))/16.0f;						// Y Position Of Current Character

		glNewList(m_GLBaseList+loop,GL_COMPILE);				// Start Building A List
			glBegin(GL_QUADS);							// Use A Quad For Each Character
				glTexCoord2f(cx,1.0f-cy-0.0625f);			// Texture Coord(Bottom Left)
				glVertex2i(0,0);						// Vertex Coord(Bottom Left)
				glTexCoord2f(cx+0.0625f,1.0f-cy-0.0625f);	// Texture Coord(Bottom Right)
				glVertex2i(16,0);						// Vertex Coord(Bottom Right)
				glTexCoord2f(cx+0.0625f,1.0f-cy);			// Texture Coord(Top Right)
				glVertex2i(16,16);						// Vertex Coord(Top Right)
				glTexCoord2f(cx,1.0f-cy);					// Texture Coord(Top Left)
				glVertex2i(0,16);						// Vertex Coord(Top Left)
			glEnd();									// Done Building Our Quad(Character)
			glTranslated(10,0,0);						// Move To The Right Of The Character
		glEndList();									// Done Building The Display List
	}													// Loop Until All 256 Are Built
}

GLvoid
moEffectText::glPrint( GLint x, GLint y, char *string, int set, float scx, float scy, float rt )	// Where The Printing Happens
{
	if(set>1)
	{
		set=1;
	}
    glBindTexture( GL_TEXTURE_2D, FontId );
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,800,0,600,-1,1);							// Set Up An Ortho Screen

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	glRotated(rt,0.0,0.0,1.0);
	glTranslated(x,y,0);								// Position The Text(0,0 - Bottom Left)
	glScalef(scx,scy,1.0f);
	glListBase( m_GLBaseList -32+(128*set));						// Choose The Font Set(0 or 1)
	glCallLists(strlen(string),GL_BYTE,string);			// Write The Text To The Screen
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	//glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
}

GLvoid
moEffectText::KillFont()									// Delete The Font From Memory
{
	glDeleteLists( m_GLBaseList, 256);							// Delete All 256 Display Lists

}



void moEffectText::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moEffect::Interaction( IODeviceManager );

	if (devicecode!=NULL)
	for(int i=0; i<ncodes; i++) {

		temp = devicecode[i].First;

		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);
			if (state)
			switch(i) {
				case MO_TEXT_TRANSLATE_X:
					Tx = valor;
					MODebug->Push(IntToStr(valor));
					break;
				case MO_TEXT_TRANSLATE_Y:
					Ty = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
					MODebug->Push(IntToStr(valor));
					break;
				case MO_TEXT_SCALE_X:
					Sx+=((float) valor / (float) 256.0);
					MODebug->Push(IntToStr(valor));
					break;
				case MO_TEXT_SCALE_Y:
					Sy+=((float) valor / (float) 256.0);
					MODebug->Push(IntToStr(valor));
					break;
			}
		temp = temp->next;
		}
	}

}


moConfigDefinition *
moEffectText::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("font"), MO_PARAM_TEXTURE, TEXT_TEXTURE, moValue( "fuentes/texto00.png", "TXT") );
	p_configdefinition->Add( moText("text"), MO_PARAM_TEXT, TEXT_TEXT, moValue( "Hola, aquí puedes ingresar el texto que quieras", "TXT") );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, TEXT_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, TEXT_WIDTH, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, TEXT_HEIGHT, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, TEXT_TRANSLATEX, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, TEXT_TRANSLATEY, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, TEXT_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, TEXT_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, TEXT_SCALEY );
	return p_configdefinition;
}
