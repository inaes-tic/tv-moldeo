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

	moDefineParamIndex( DEBUG_ALPHA, moText("alpha") );
	moDefineParamIndex( DEBUG_COLOR, moText("color") );
	moDefineParamIndex( DEBUG_SYNC, moText("syncro") );
	moDefineParamIndex( DEBUG_PHASE, moText("phase") );
	moDefineParamIndex( DEBUG_FONT, moText("font") );


	ticks = 0;
	fps_current = 0;
	fps_mean = 0;
	fps_count = 0;

	MODebug2->Message(moText("Debug initialized"));

	return true;
}

void moPostEffectDebug::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    MOulong timecodeticks;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

	//calculamos ticks y frames x seg
	moTextArray TextArray;

	ticksprevious = ticks;
	ticks = moGetTicksAbsolute();
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


/*
    for(int g=560;g>=80 ;g-=20) {
        //glPrint(0,g,MODebug2->Pop(),0,1.0,1.0);
        //glPrint(0,g,moText("test"),0,1.0,1.0);

    }
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


    moFont* pFont = m_Config[moR(DEBUG_FONT)].GetData()->Font();

    PreDraw( tempogral, parentstate);

    // Cambiar la proyeccion para una vista ortogonal //
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,w,0,h,-1,1);                              // Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
	glLoadIdentity();									// Reset The View


    glEnable(GL_BLEND);

    //color
    SetColor( m_Config[moR(DEBUG_COLOR)][MO_SELECTED], m_Config[moR(DEBUG_ALPHA)][MO_SELECTED], state );

    moText Texto = moText("");
    Texto+= fps_text;

    int minutes;
    int seconds;
    int frames;

    if (pFont) {
        pFont->Draw(    0.0,
                        0.0,
                        Texto,
                        m_Config[moR(DEBUG_FONT)][MO_SELECTED][2].Int(),
                        0 );

        timecodeticks = moGetTicks();
        minutes = timecodeticks / (1000*60);
        seconds = (timecodeticks - minutes*1000*60) / 1000;
        frames = (timecodeticks - minutes*1000*60 - seconds*1000 ) * 25 / 1000;

        Texto = moText("");
        /*Texto = moText(" ticks: ") + (moText)IntToStr(timecodeticks) +
                moText(" ang: ") + (moText)FloatToStr(tempogral->ang) +
                moText(" timecode: ") + (moText)IntToStr(minutes) + moText(":") + (moText)IntToStr(seconds) + moText(":") + (moText)IntToStr(frames);
*/
        //glTranslatef( 0.0, 2.0, 0.0 );
        pFont->Draw(    0.0,
                        16.0,
                        Texto);


        moText  Final;

        int cint = 0;
        //Final = moText("Debug ");
        //for( int i=0; i<MODebug2->Count(); i++ ) {
        //    cint++;
            //Final = MODebug2->GetMessages().Get(i);
        //}

        cint = MODebug2->Count();
        //Final = Final + moText("(")+IntToStr( cint ) + moText("):");
        //if (cint>0) Final = Final + (moText)MODebug2->GetLast();


        /*
        for( int i=0; i<(cint-1); i++ ) {
            Final = MODebug2->Pop();
        }
        */
        pFont->Draw( 0.0, 48.0, Final );

        //moText infod = moText("screen width:")+IntToStr(w)+moText(" screen height:")+IntToStr(h);
        //pFont->Draw( 0, 0, infod, m_Config[moR(TEXT_FONT)][MO_SELECTED][2].Int(), 0, 2.0, 2.0, 0.0);
    }

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

}

MOboolean moPostEffectDebug::Finish()
{
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


moConfigDefinition *
moPostEffectDebug::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("font"), MO_PARAM_FONT, DEBUG_FONT, moValue( "Default", "TXT", "0", "NUM", "32.0", "NUM") );
	return p_configdefinition;
}

