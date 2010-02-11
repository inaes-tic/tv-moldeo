/*******************************************************************************

                               moEffectSound.cpp

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

#include "moEffectSound.h"

//========================
//  Factory
//========================

moEffectSoundFactory *m_EffectSoundoFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if (m_EffectSoundoFactory==NULL)
		m_EffectSoundoFactory = new moEffectSoundFactory();
	return (moEffectFactory*) m_EffectSoundoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectSoundoFactory;
	m_EffectSoundoFactory = NULL;
}

moEffect*  moEffectSoundFactory::Create() {
	return new moEffectSound();
}

void moEffectSoundFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectSound::moEffectSound() {

	SetName("sound");

}

moEffectSound::~moEffectSound() {
	Finish();
}

MOboolean moEffectSound::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( SOUND_ALPHA, moText("alpha") );
	moDefineParamIndex( SOUND_COLOR, moText("color") );
	moDefineParamIndex( SOUND_SYNC, moText("syncro") );
	moDefineParamIndex( SOUND_PHASE, moText("phase") );
	moDefineParamIndex( SOUND_SOUND, moText("sound") );
	moDefineParamIndex( SOUND_TEXTURE, moText("texture") );
	moDefineParamIndex( SOUND_BLENDING, moText("blending") );
	moDefineParamIndex( SOUND_WIDTH, moText("width") );
	moDefineParamIndex( SOUND_HEIGHT, moText("height") );
	moDefineParamIndex( SOUND_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( SOUND_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( SOUND_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( SOUND_ROTATEX, moText("rotatex") );
	moDefineParamIndex( SOUND_ROTATEY, moText("rotatey") );
	moDefineParamIndex( SOUND_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( SOUND_SCALEX, moText("scalex") );
	moDefineParamIndex( SOUND_SCALEY, moText("scaley") );
	moDefineParamIndex( SOUND_SCALEZ, moText("scalez") );
	moDefineParamIndex( SOUND_INLET, moText("inlet") );
	moDefineParamIndex( SOUND_OUTLET, moText("outlet") );

    Tx = Ty = Tz = Rx = Ry = Rz = 0.0;
	Sx = Sy = Sz = 1.0;

    m_SoundFilename = moText("");

    m_Audio.InitGraph();
    m_bAudioStarted = false;

    if (m_SoundFilename!=m_Config[moParamReference(SOUND_SOUND)][MO_SELECTED][0].Text() ) {

        m_SoundFilename = m_Config[moParamReference(SOUND_SOUND)][MO_SELECTED][0].Text();
        m_SoundFilenameFull = (moText)m_pResourceManager->GetDataMan()->GetDataPath() + (moText)moSlash + (moText)m_SoundFilename;

        m_Audio.BuildLiveSound( m_SoundFilenameFull );

    }

    last_ticks = 0;

	return true;
}

#define KLT_TRACKED           0
#define KLT_NOT_FOUND        -1
#define KLT_SMALL_DET        -2
#define KLT_MAX_ITERATIONS   -3
#define KLT_OOB              -4
#define KLT_LARGE_RESIDUE    -5

void moEffectSound::Draw( moTempo* tempogral, moEffectState* parentstate )
{
    MOint indeximage;
	MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    PreDraw( tempogral, parentstate);

    if (m_SoundFilename!=m_Config[moParamReference(SOUND_SOUND)][MO_SELECTED][0].Text() ) {

        m_SoundFilename = m_Config[moParamReference(SOUND_SOUND)][MO_SELECTED][0].Text();
        m_SoundFilenameFull = (moText)m_pResourceManager->GetDataMan()->GetDataPath() + (moText)moSlash + (moText)m_SoundFilename;

        m_Audio.BuildLiveSound( m_SoundFilenameFull );

    }



    /*
    if ( tempogral->ticks>1 && !moTimeManager::MoldeoTimer->Paused() && !m_bAudioStarted ) {
        m_Audio.Seek( tempogral->ticks );
        m_Audio.Play();
        m_bAudioStarted = true;
    } else if ( tempogral->ticks==0 && !moTimeManager::MoldeoTimer->Started() ) {
        m_Audio.Stop();
        m_bAudioStarted = false;
    }

    if (  last_ticks == tempogral->ticks ) {
        m_Audio.Pause();
        m_Audio.Seek( tempogral->ticks );
        m_bAudioStarted = false;
    }
    */
    moStreamState audiostate = m_Audio.GetState();
    moText  textstate = m_Audio.StateToText( audiostate );
    long    audioposition;

    last_ticks = moGetTicks();

    audioposition = m_Audio.GetPosition();



    if ( (long) abs(audioposition - last_ticks) > 8000 ) {
       if (audiostate!=MO_STREAMSTATE_STOPPED)
           m_Audio.Seek( last_ticks );
           audioposition = last_ticks;

    }

    moSetDuration( audioposition );

    if (moTimeManager::MoldeoTimer->Paused()) {

        if (audiostate!=MO_STREAMSTATE_PAUSED)
            m_Audio.Pause();

    } else {

        if ( moTimeManager::MoldeoTimer->Started() ) {
            if (audiostate!=MO_STREAMSTATE_PLAYING)
                m_Audio.Play();
        } else {
            if (audiostate!=MO_STREAMSTATE_STOPPED)
                m_Audio.Stop();
        }

    }

    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
	glLoadIdentity();									// Reset The View

    // Cambiar la proyeccion para una vista ortogonal //
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,w,0,h,-1,1);                              // Set Up An Ortho Screen


    // Funcion de blending y de alpha channel //
    glEnable(GL_BLEND);

	glDisable(GL_ALPHA);

    // Draw //
	glTranslatef(  ( m_Config[moR(SOUND_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang)+Tx )*w,
                   ( m_Config[moR(SOUND_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang)+Ty )*h,
					m_Config[moR(SOUND_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang)+Tz);

	glRotatef(  m_Config[moR(SOUND_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(SOUND_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(SOUND_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
	glScalef(   m_Config[moR(SOUND_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang)*Sx,
                m_Config[moR(SOUND_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang)*Sy,
                m_Config[moR(SOUND_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang)*Sz);

    SetColor( m_Config[moR(SOUND_COLOR)][MO_SELECTED], m_Config[moR(SOUND_ALPHA)][MO_SELECTED], state );

    SetBlending( (moBlendingModes) m_Config[moR(SOUND_BLENDING)][MO_SELECTED][0].Int() );

    moTexture* pImage = (moTexture*) m_Config[moR(SOUND_TEXTURE)].GetData()->Pointer();

    glBindTexture( GL_TEXTURE_2D, m_Config[moR(SOUND_TEXTURE)].GetData()->GetGLId(&state.tempo) );

    PosTextX0 = 0.0;
	PosTextX1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordS() :  1.0 );
    PosTextY0 = 0.0;
    PosTextY1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordT() :  1.0 );

	//ancho = (int)m_Config[ moR(SOUND_WIDTH) ].GetData()->Fun()->Eval(state.tempo.ang)* (float)(w/800.0);
	//alto = (int)m_Config[ moR(SOUND_HEIGHT) ].GetData()->Fun()->Eval(state.tempo.ang)* (float)(h/600.0);

	/*
	glBegin(GL_QUADS);
		glTexCoord2f( PosTextX0, PosTextY1);
		glVertex2f( -0.5*w, -0.5*h);

		glTexCoord2f( PosTextX1, PosTextY1);
		glVertex2f(  0.5*w, -0.5*h);

		glTexCoord2f( PosTextX1, PosTextY0);
		glVertex2f(  0.5*w,  0.5*h);

		glTexCoord2f( PosTextX0, PosTextY0);
		glVertex2f( -0.5*w,  0.5*h);
	glEnd();
*/
    moFont* pFont = m_pResourceManager->GetFontMan()->GetFont(0);

    textstate+= moText(" position") + IntToStr(audioposition);
    textstate+= moText(" diff:") + (moText)IntToStr( abs(audioposition - last_ticks) );

    if (pFont) {
        pFont->Draw(    0.0,
                        0.0,
                        textstate );
    }

}

MOboolean moEffectSound::Finish()
{
    return PreFinish();
}

void moEffectSound::Interaction( moIODeviceManager *IODeviceManager ) {

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
				case MO_SOUND_TRANSLATE_X:
					Tx = valor;
					MODebug->Push(IntToStr(valor));
					break;
				case MO_SOUND_TRANSLATE_Y:
					Ty = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
					MODebug->Push(IntToStr(valor));
					break;
				case MO_SOUND_SCALE_X:
					Sx+=((float) valor / (float) 256.0);
					MODebug->Push(IntToStr(valor));
					break;
				case MO_SOUND_SCALE_Y:
					Sy+=((float) valor / (float) 256.0);
					MODebug->Push(IntToStr(valor));
					break;
			}
		temp = temp->next;
		}
	}

}

moConfigDefinition *
moEffectSound::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("sound"), MO_PARAM_TEXT, SOUND_SOUND, moValue( "costa.mp3", "TXT") );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, SOUND_TEXTURE, moValue( "soundos/estrellas/star.tga", "TXT") );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, SOUND_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, SOUND_WIDTH, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, SOUND_HEIGHT, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, SOUND_TRANSLATEX, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, SOUND_TRANSLATEY, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, SOUND_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, SOUND_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, SOUND_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, SOUND_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, SOUND_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, SOUND_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, SOUND_SCALEZ );
	return p_configdefinition;
}



void
moEffectSound::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);


}
