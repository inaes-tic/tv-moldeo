/*******************************************************************************

                        moPostEffectDebug.cpp

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

#include "moPostEffectDebug.h"

//========================
//  Factory
//========================

moPostEffectDebugFactory *m_PostEffectDebugFactory = NULL;

MO_PLG_API moPostEffectFactory* CreatePostEffectFactory(){
	if(m_PostEffectDebugFactory==NULL)
		m_PostEffectDebugFactory = new moPostEffectDebugFactory();
	return(moPostEffectFactory*) m_PostEffectDebugFactory;
}

MO_PLG_API void DestroyPostEffectFactory(){
	delete m_PostEffectDebugFactory;
	m_PostEffectDebugFactory = NULL;
}

moPostEffect*  moPostEffectDebugFactory::Create() {
	return new moPostEffectDebug();
}

void moPostEffectDebugFactory::Destroy(moPostEffect* fx) {
	delete fx;
}

//========================
//  PostEfecto
//========================


moPostEffectDebug::moPostEffectDebug() {
	SetName("debug");
	SetType(MO_OBJECT_POSTEFFECT);
}

moPostEffectDebug::~moPostEffectDebug() {
	Finish();
}

MOboolean
moPostEffectDebug::Init()
{
    if (!PreInit()) return false;

    color = m_Config.GetParamIndex("color");
    font = m_Config.GetParamIndex("font");

    // Hacer la inicializacion de este efecto en particular.
    // Seteos de OpenGL.
    // Seteos de Texturas.
	Fonts.MODebug = MODebug;
	Fonts.Init(GetConfig(),font,m_pResourceManager->GetTextureMan());

	Font = 0;
	BuildFont();

	if(m_Config.GetPreConfCount() > 0)
        m_Config.PreConfFirst();

	ticks = 0;
	fps_current = 0;
	fps_mean = 0;
	fps_count = 0;

	MODebug2->Message(moText("debug inited"));

	return true;
}

void moPostEffectDebug::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

	int canvasWidth = m_pResourceManager->GetRenderMan()->RenderWidth();
    int canvasHeight = m_pResourceManager->GetRenderMan()->RenderHeight();
    m_pResourceManager->GetGLMan()->SetOrthographicView(canvasWidth, canvasHeight);

    // Aca van los comandos OpenGL del efecto.

	//actualizamos la fuente
	MOuint PrevFont;

	PrevFont = Font;
	Font = m_Config.GetCurrentValueIndex(font);

	if(Font!=PrevFont) {
		KillFont();
		BuildFont();
	}

	//calculamos ticks y frames x seg
	moTextArray TextArray;

	ticksprevious = ticks;
	ticks = m_pResourceManager->GetTimeMan()->GetTicks();
	tickselapsed = ticks - ticksprevious;

	fps_current = 1000.0 / tickselapsed;
	fps_mean += fps_current;

	fps_count++;
	if (fps_count % 10 == 0)
	{
		fps_mean /= 10;
		fps_text = moText("FPS = ") + (moText)FloatToStr(fps_mean);
		fps_mean = 0;
		fps_count = 0;
	}
	TextArray.Add(fps_text);


	//DIBUJAR
	glColor4f(  m_Config.GetParam(color).GetValue().GetSubValue(MO_RED).Float()*state.tintr,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_GREEN).Float()*state.tintg,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_BLUE).Float()*state.tintb,
                m_Config.GetParam(color).GetValue().GetSubValue(MO_ALPHA).Float()*state.alpha);

	//glMatrixMode( GL_MODELVIEW );
	//glLoadIdentity();									// Reset The View
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_ONE);
	//glDisable(GL_ALPHA);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


    for(int g=560;g>=80 /*&& MODebug2->Count()>0*/;g-=20) {
        //glPrint(0,g,MODebug2->Pop(),0,1.0,1.0);
        glPrint(0,g,moText("test"),0,1.0,1.0);

    }


	/*
	m_pResourceManager->GetGLMan()->SetOrthographicView( m_pResourceManager->GetRenderMan()->ScreenWidth(), m_pResourceManager->GetRenderMan()->ScreenHeight() );
	m_pResourceManager->GetGuiMan()->DisplayInfoWindow( 20 , 20, 200, 20, TextArray );
	TextArray.Finish();
	m_pResourceManager->GetGuiMan()->DisplayInfoWindow( 400 , 300, 200, 200, textevents );
	*/



	/*
	moText state_datos;
	moText state_luminosidad;
	moText state_colorido;
	moText state_magnitude;

	state_datos+= "SINC: ";
	state_datos+= IntToStr(state.synchronized);
	state_datos+= " FAC: ";
	state_datos+= FloatToStr(state.tempo.factor,4);
	state_datos+= " SYNCRO: ";
	state_datos+= FloatToStr(state.tempo.syncro,4);

	state_luminosidad+="ALPHA:";
	state_luminosidad+= FloatToStr(state.alpha,4);
	state_luminosidad+="LUM:";
	state_luminosidad+= FloatToStr(state.tint,4);

	state_colorido+="ALPHA:";
	state_colorido+= FloatToStr(state.tintc,4);
	state_colorido+="SAT:";
	state_colorido+= FloatToStr(state.tints,4);

	state_magnitude+="DELTA:";
	state_magnitude+= FloatToStr(state.tempo.delta,4);
	state_magnitude+="AMP:";
	state_magnitude+= FloatToStr(state.amplitude,4);


	MODebug->Push(state_datos);
	MODebug->Push(state_luminosidad);
	MODebug->Push(state_colorido);
	MODebug->Push(state_magnitude);
*/



}

MOboolean moPostEffectDebug::Finish()
{
	Fonts.Finish();
    return PreFinish();
}

void
moPostEffectDebug::Update( moEventList* p_EventList ) {

	moEvent *actual,*tmp;
	moMessage *pmessage;
	MOint evtscount = 0;

	actual = p_EventList->First;
	textevents.Empty();
	textevents.Add(moText("Events List:"));

	//Procesamos los eventos recibidos de los MoldeoObject Outlets
	while(actual!=NULL) {
		tmp = actual->next;

		if (actual->reservedvalue3 == MO_MESSAGE) {

			//pSample = (moVideoSample*)actual->pointer;
			pmessage = (moMessage*)actual;
			textevents.Add( moText("Did:") + IntToStr( pmessage->m_MoldeoIdDest) + moText("SrcId:") + IntToStr( pmessage->m_MoldeoIdSrc) );
		} else {
			textevents.Add( moText("Did:") + IntToStr( actual->deviceid) );
		}
		evtscount++;
		actual = tmp;
	}
	textevents.Set(0,moText("Events List:")+IntToStr(evtscount));
}


//====================
//
//		CUSTOM
//
//===================


GLvoid
moPostEffectDebug::BuildFont()								// Build Our Font Display List
{
	float	cx;											// Holds Our X Character Coord
	float	cy;											// Holds Our Y Character Coord

	base=glGenLists(256);								// Creating 256 Display Lists
	glBindTexture(GL_TEXTURE_2D, Fonts.GetGLId(Font));			// Select Our Font Texture
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	for(loop=0; loop<256; loop++)						// Loop Through All 256 Lists
	{
		cx=float((int)(loop%16))/16.0f;						// X Position Of Current Character
		cy=float((int)(loop/16))/16.0f;						// Y Position Of Current Character

		glNewList(base+loop,GL_COMPILE);				// Start Building A List
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
			glTranslated(16,0,0);						// Move To The Right Of The Character
		glEndList();									// Done Building The Display List
	}													// Loop Until All 256 Are Built
}

GLvoid
moPostEffectDebug::glPrint(GLint x, GLint y, char *string, int set, float scx, float scy)	// Where The Printing Happens
{
	if(set>1)
	{
		set=1;
	}
	glBindTexture(GL_TEXTURE_2D, Fonts.GetGLId(Font));			// Select Our Font Texture
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,800,0,600,-1,1);							// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	glTranslated(x,y,0);								// Position The Text(0,0 - Bottom Left)
	glScalef(scx,scy,1.0f);
	glListBase(base-32+(128*set));						// Choose The Font Set(0 or 1)
	glCallLists(strlen(string),GL_BYTE,string);			// Write The Text To The Screen
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	//glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
}

GLvoid
moPostEffectDebug::KillFont()									// Delete The Font From Memory
{
	glDeleteLists(base,256);							// Delete All 256 Display Lists
}
