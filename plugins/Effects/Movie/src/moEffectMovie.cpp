/*******************************************************************************

                                moEffectMovie.cpp

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
  Andres Colubri

*******************************************************************************/

#include "moEffectMovie.h"

//========================
//  Factory
//========================

moEffectMovieFactory *m_EffectMovieFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectMovieFactory==NULL)
		m_EffectMovieFactory = new moEffectMovieFactory();
	return(moEffectFactory*) m_EffectMovieFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectMovieFactory;
	m_EffectMovieFactory = NULL;
}

moEffect*  moEffectMovieFactory::Create() {
	return new moEffectMovie();
}

void moEffectMovieFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectMovie::moEffectMovie() {
	SetName("movie");
}

moEffectMovie::~moEffectMovie() {
	Finish();
}

MOboolean
moEffectMovie::Init() {

    if (!PreInit()) return false;

    // Hacer la inicializacion de este efecto en particular.
	moDefineParamIndex( MOVIE_ALPHA, moText("alpha") );
	moDefineParamIndex( MOVIE_COLOR, moText("color") );
	moDefineParamIndex( MOVIE_SYNC, moText("syncro") );
	moDefineParamIndex( MOVIE_MOVIES, moText("movies") );
	moDefineParamIndex( MOVIE_SOUNDS, moText("sounds") );
	moDefineParamIndex( MOVIE_SCRIPT, moText("script") );
	moDefineParamIndex( MOVIE_MODE, moText("moviemode") );
	moDefineParamIndex( MOVIE_BLENDING, moText("blending") );
	moDefineParamIndex( MOVIE_STARTPLAYING, moText("startplaying") );
	moDefineParamIndex( MOVIE_LOOP, moText("loop") );
	moDefineParamIndex( MOVIE_SHOWTRACKDATA, moText("showtrackdata") );
	moDefineParamIndex( MOVIE_SHOWMOVIEDATA, moText("showmoviedata") );
	moDefineParamIndex( MOVIE_INTERPOLATION, moText("interpolation") );
	moDefineParamIndex( MOVIE_POSTEXX, moText("pos_text_x") );
	moDefineParamIndex( MOVIE_POSTEXY, moText("pos_text_y") );
	moDefineParamIndex( MOVIE_ANCTEXX, moText("anc_text_x") );
	moDefineParamIndex( MOVIE_ALTTEXY, moText("alt_text_y") );
	moDefineParamIndex( MOVIE_POSCUADX, moText("pos_cuad_x") );
	moDefineParamIndex( MOVIE_POSCUADY, moText("pos_cuad_y") );
	moDefineParamIndex( MOVIE_ANCCUADX, moText("anc_cuad_x") );
	moDefineParamIndex( MOVIE_ALTCUADY, moText("alt_cuad_y") );
	moDefineParamIndex( MOVIE_INLET, moText("inlet") );
	moDefineParamIndex( MOVIE_OUTLET, moText("outlet") );

	m_bStartPlayingOn = m_Config[moParamReference(MOVIE_STARTPLAYING)][MO_SELECTED][0].Int();
	m_bLoop = m_Config[moParamReference(MOVIE_LOOP)][MO_SELECTED][0].Int();

	m_bInterpolation = m_Config[moParamReference(MOVIE_INTERPOLATION)][MO_SELECTED][0].Int();

	// Seteos de Texturas.
	Images.MODebug = MODebug;
	Images.Init(GetConfig(), moParamReference(MOVIE_MOVIES), m_pResourceManager->GetTextureMan());

    if (m_Config.GetPreConfCount() > 0)
        m_Config.PreConfFirst();

	m_PlaySpeed = 0.0;
	m_PlayState = MO_MOVIE_PLAYSTATE_STOPPED;//speed 0 0.0
	m_SeekState = MO_MOVIE_SEEKSTATE_REACHED;//frame 0

	m_bDisplayOn = false;
	m_bPlayingSound = false;

	m_FramesLength = 0;
	m_FramePosition = 0;
	m_FramePositionAux = 0;
	m_FramePositionF = 0.0;
	m_FramePositionFAux = 0.0;
	m_DiffFrame = 0.0;
	m_FramePositionInc = 0.0;
	m_TAnim = NULL;

	MOint imov = m_Config[moParamReference(MOVIE_MOVIES)].GetIndexValue();
	if (Images.ValidIndex(imov)) {
		if (Images[imov]->GetType() == MO_TYPE_MOVIE ||
			Images[imov]->GetType() == MO_TYPE_VIDEOBUFFER) {
			m_TAnim = (moTextureAnimated*)Images[imov];
			m_FramesLength = m_TAnim->GetNumberFrames();
			m_FramesPerSecond = m_TAnim->GetFramesPerSecond();
		}
	}
/*
	Sound = m_pResourceManager->GetSoundMan()->GetSound(m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][0].Text());
	if (Sound) {
		Sound->SetPosition( m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][1].Float(),
							m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][2].Float(),
							m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][3].Float());
	}
	 */
	 {
      moText wavfilename = m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][0].Text();
      wavfilename = (moText)m_pResourceManager->GetDataMan()->GetDataPath() + (moText)moSlash + (moText)wavfilename;

      m_SoundSystem.InitGraph();


      if (m_SoundSystem.BuildLiveSound( wavfilename )) {

      }

    }


	m_MovieScriptFN = m_Config[moParamReference(MOVIE_SCRIPT)][MO_SELECTED][0].Text();
	if ((moText)m_MovieScriptFN!=moText("")) {

	    moScript::InitScript();
        RegisterFunctions();

        m_MovieScriptFN = m_pResourceManager->GetDataMan()->GetDataPath()+ moText("/") + (moText)m_MovieScriptFN;

        if ( CompileFile(m_MovieScriptFN) ) {

            SelectScriptFunction( "Init" );
            AddFunctionParam( m_FramesPerSecond );
            AddFunctionParam( (int)m_FramesLength );
            AddFunctionParam( m_pResourceManager->GetRenderMan()->RenderWidth() );
            AddFunctionParam( m_pResourceManager->GetRenderMan()->RenderHeight() );
            RunSelectedFunction();

        } else {

        }
	}

	m_bTrackerInit = false;
	m_pTrackerData = NULL;
	//m_pTrackerGpuData = NULL;

	m_TicksAux = state.tempo.ticks;
	m_Ticks = m_TicksAux;

	return true;
}

void moEffectMovie::Draw( moTempo* tempogral, moEffectState* parentstate )
{
	MOint Image, FrameGLid;

	MOint Blending;
	MOdouble PosTextX,  AncTextX,  PosTextY,  AltTextY;
    MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    MOdouble PosCuadX,  AncCuadX,  PosCuadY,  AltCuadY;
    MOdouble PosCuadX0, PosCuadX1, PosCuadY0, PosCuadY1;

    PreDraw( tempogral, parentstate);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    /*
	if (Sound)
		Sound->Update();
*/
	glColor4f(  m_Config[moR(MOVIE_COLOR)][MO_SELECTED][MO_RED].Fun()->Eval(state.tempo.ang) * state.tintr,
                m_Config[moR(MOVIE_COLOR)][MO_SELECTED][MO_GREEN].Fun()->Eval(state.tempo.ang) * state.tintg,
                m_Config[moR(MOVIE_COLOR)][MO_SELECTED][MO_BLUE].Fun()->Eval(state.tempo.ang) * state.tintb,
				m_Config[moR(MOVIE_COLOR)][MO_SELECTED][MO_ALPHA].Fun()->Eval(state.tempo.ang) *
				m_Config[moR(MOVIE_ALPHA)].GetData()->Fun()->Eval(state.tempo.ang) * state.alpha);

	Image = m_Config[moParamReference(MOVIE_MOVIES)].GetIndexValue();

	moviemode = m_Config[moParamReference(MOVIE_MODE)].GetIndexValue();

	Blending = m_Config[moParamReference(MOVIE_BLENDING)].GetIndexValue();
	//DIBUJAR
	glEnable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing

    SetBlending( (moBlendingModes) Blending );
	//source: GL_ZERO, GL_ONE, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, and GL_SRC_ALPHA_SATURATE
	//destination: GL_ZERO, GL_ONE, GL_SCR_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR, GL_DST_ALPHA, and GL_ONE_MINUS_DST_ALPHA.

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,800,0,600,-1,1);							// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//registro de los dos ticks correspondientes a un Frame
	m_TicksAux = m_Ticks;
	m_Ticks = state.tempo.ticks;


	if (Images[Image])
	if (Images[Image]->GetType() == MO_TYPE_MOVIE || Images[Image]->GetType() == MO_TYPE_VIDEOBUFFER) {
		m_TAnim = (moTextureAnimated*)Images[Image];
		if (m_TAnim) {
			m_FramesLength = m_TAnim->GetNumberFrames();
			m_FramesPerSecond = m_TAnim->GetFramesPerSecond();
		}
	}

	//aquí debemos modificar Images.Get(Image,&state.tempo) para los distintos modos de reproduccion
	//0: PLAY NORMAL 1X [25 fps o 29.97]
	//1: FF 2X []
	//2: RW -2X []
	//3: FFF 8X []
	//4: RRW -8X []
	//5: STOP []
	//6: SEEK( position )


	switch(moviemode) {
		case MO_MOVIE_MODE_VCR:

			if (m_bStartPlayingOn) {
			    m_FramePosition = 0;
                m_FramePositionAux = 0;
                m_FramePositionF = 0.0;
                m_FramePositionFAux = 0.0;
                m_Ticks = state.tempo.ticks;
                m_TicksAux = m_Ticks;
				VCRCommand( MO_MOVIE_VCR_PLAY );
				m_bStartPlayingOn = false;//second round off if we stop
			}

			switch(m_PlayState) {
				case MO_MOVIE_PLAYSTATE_STOPPED:
					m_FramePosition = 0;
					m_FramePositionAux = 0;
					m_FramePositionF = 0.0;
					m_FramePositionFAux = 0.0;
					FrameGLid = Images.GetGLId(Image, 1);
					break;

				case MO_MOVIE_PLAYSTATE_PLAYING:
                    VCRPlaySpeed();
                    break;
				case MO_MOVIE_PLAYSTATE_REVERSE:
                    VCRPlaySpeed();
                    break;
				case MO_MOVIE_PLAYSTATE_SPEED:
					//play en funcion de play speed:
					VCRPlaySpeed();
					break;

				case MO_MOVIE_PLAYSTATE_SEEKING:
					//a definir
					break;
			}

			//Frame in funcion of m_FramePosition
			//MODebug->Push(IntToStr(m_FramePosition));
			if ( m_FramePosition <  m_FramesLength ) {
				FrameGLid = Images.GetGLId( (int)Image, (int)m_FramePosition );
			}
			if ( m_FramePosition >= (m_FramesLength-1) ) {
				if (m_bLoop) {
					m_FramePosition = 0;
					m_FramePositionAux = 0;
					m_FramePositionF = 0.0;
					FrameGLid = Images.GetGLId( (int)Image, (int)m_FramePosition );
				} else {
					VCRCommand(MO_MOVIE_VCR_STOP);
					FrameGLid = Images.GetGLId( (int)Image, (int)m_FramePosition );
				}
			}
			break;

		case MO_MOVIE_MODE_SCRIPT:

			if ( m_bTrackerInit ) {
				SelectScriptFunction("Run");

				if (m_pTrackerData) {
					AddFunctionParam(m_pTrackerData->GetFeaturesCount());
					AddFunctionParam((int)m_pTrackerData->GetVideoFormat().m_Width);
					AddFunctionParam((int)m_pTrackerData->GetVideoFormat().m_Height);
				}
                RunSelectedFunction(1);

				m_DiffFrame = (m_FramePositionFAux - m_FramePositionF);
				m_FramePosition = m_FramePositionF;
				m_FramePositionFAux = m_FramePositionF;

			} else { //NO INIT YET

				m_FramePositionF = 0.0;
				m_FramePosition = m_FramePositionF;
				m_FramePositionFAux = m_FramePositionF;

			}

			if (m_FramePosition < m_FramesLength)
			{
				FrameGLid = Images.GetGLId((int)Image, (int)m_FramePosition );
				//MODebug->Push(IntToStr(m_FramePosition));
			}
			else
			{
				MODebug->Push("Error: script generating invalid frame positions.");
			    FrameGLid = Images.GetGLId(Image, 1);
			}

			break;

		case MO_MOVIE_MODE_CYCLE:
			 FrameGLid = Images.GetGLId(Image, &state.tempo);
			break;

		default:
			break;
	}
/*
	MODebug->Push(moText("F:")+IntToStr(m_FramePosition));
	MODebug->Push(moText("FAux:")+IntToStr(m_FramePositionAux));
	MODebug->Push(moText("FF:")+IntToStr(m_FramePositionF));
	MODebug->Push(moText("FFAux:")+IntToStr(m_FramePositionFAux));

	MODebug->Push(moText("E:")+IntToStr(m_PlayState));
	MODebug->Push(moText("S:")+IntToStr(m_PlaySpeed));
*/

	PosTextX = m_Config[moR(MOVIE_POSTEXX)].GetData()->Fun()->Eval(state.tempo.ang);
    AncTextX = m_Config[moR(MOVIE_ANCTEXX)].GetData()->Fun()->Eval(state.tempo.ang);
	PosTextY = m_Config[moR(MOVIE_POSTEXY)].GetData()->Fun()->Eval(state.tempo.ang);
    AltTextY = m_Config[moR(MOVIE_ALTTEXY)].GetData()->Fun()->Eval(state.tempo.ang);

	PosCuadX = m_Config[moR(MOVIE_POSCUADX)].GetData()->Fun()->Eval(state.tempo.ang);
    AncCuadX = m_Config[moR(MOVIE_ANCCUADX)].GetData()->Fun()->Eval(state.tempo.ang);
	PosCuadY = m_Config[moR(MOVIE_POSCUADY)].GetData()->Fun()->Eval(state.tempo.ang);
    AltCuadY = m_Config[moR(MOVIE_ALTCUADY)].GetData()->Fun()->Eval(state.tempo.ang);

	PosTextX0 = PosTextX*Images[Image]->GetMaxCoordS();
	PosTextX1 =(PosTextX + AncTextX)*Images[Image]->GetMaxCoordS();
	PosTextY0 =(1 - PosTextY)*Images[Image]->GetMaxCoordT();
	PosTextY1 =(1 - PosTextY - AltTextY)*Images[Image]->GetMaxCoordT();

	PosCuadX0 = PosCuadX;
	PosCuadX1 = PosCuadX + AncCuadX;
	PosCuadY1 = PosCuadY;
	PosCuadY0 = PosCuadY + AltCuadY;

	glBindTexture(GL_TEXTURE_2D, FrameGLid);
	glBegin(GL_QUADS);
		glTexCoord2f( PosTextX0, PosTextY1);
		glVertex2i ( PosCuadX0, PosCuadY0);

		glTexCoord2f( PosTextX1, PosTextY1);
		glVertex2i ( PosCuadX1, PosCuadY0);

        glTexCoord2f( PosTextX1, PosTextY0);
		glVertex2i ( PosCuadX1, PosCuadY1);

		glTexCoord2f( PosTextX0, PosTextY0);
		glVertex2i ( PosCuadX0, PosCuadY1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

    glBegin(GL_QUADS);
	    glVertex2f(0, 0);
	    glVertex2f(0, 10.0);
	    glVertex2f(0 + 10.0, 10.0);
	    glVertex2f(0 + 10.0, 0);
    glEnd();

    //if ((MovieMode == MO_MOVIE_MODE_SCRIPT) && m_bTrackerInit && config.GetCurrentValue(showtrackdata))
	bool showfeat = m_Config[moR(MOVIE_SHOWTRACKDATA)][MO_SELECTED][0].Int();
	if (m_bTrackerInit && showfeat) DrawTrackerFeatures();

	bool showmoviedata = m_Config[moR(MOVIE_SHOWMOVIEDATA)][MO_SELECTED][0].Int();
	if (showmoviedata) {
		DrawDisplayInfo( PosCuadX0, PosCuadY0-252 );
	}

    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();
}

void
moEffectMovie::DrawDisplayInfo( MOfloat x, MOfloat y ) {

	moTextArray pInfo;



    pInfo.Add( moText("TicksAux:") + IntToStr((int)m_TicksAux) );
    pInfo.Add( moText("Ticks:") + IntToStr((int)m_Ticks) );
    pInfo.Add( moText(" "));
	pInfo.Add( moText("Mode:") + IntToStr((int)moviemode) );
	pInfo.Add( moText("Loop:") + IntToStr((int)m_bLoop) );
    pInfo.Add( moText("State:") + IntToStr((int)m_PlayState) );
	pInfo.Add( moText("Speed:") + FloatToStr(m_PlaySpeed) );
	pInfo.Add( moText(" "));
	pInfo.Add( moText("Len:") + IntToStr(m_FramesLength) );
	pInfo.Add( moText("Fps:") + FloatToStr(m_FramesPerSecond) );
	pInfo.Add( moText(" "));
	pInfo.Add( moText("Posi.:") + IntToStr(m_FramePosition));
    /*if (Sound)
		if (m_FramesLength>0 && Sound->GetBufferSize()>0 )
			pInfo.Add( moText("FP S:") + IntToStr(Sound->GetActualSample()/ ( Sound->GetBufferSize() / m_FramesLength )) );
        */
	pInfo.Add( moText("P.F.:") + FloatToStr(m_FramePositionF));
	pInfo.Add( moText("P.Inc.:") + FloatToStr(m_FramePositionInc));
	pInfo.Add( moText(" "));
	pInfo.Add( moText("P.Aux.:") + IntToStr(m_FramePositionAux));
	pInfo.Add( moText("P.Aux.F.:") + FloatToStr(m_FramePositionFAux));


	/*
	pInfo.Add( moText("shottime: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("endshottime: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("empT: ")+ m_tScriptDebug.Pop());
	pInfo.Add( moText("aftT: ")+ m_tScriptDebug.Pop());
	pInfo.Add( moText("nobs: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("ttime: ")+m_tScriptDebug.Pop());

	pInfo.Add( moText("RFIRST: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("PFIRST: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("RSHOT: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("PSHOT: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("RLAST: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("PLAST: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("GFINISH: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("LFINISH: ")+m_tScriptDebug.Pop());
*/
	m_pResourceManager->GetGuiMan()->DisplayInfoWindow( x , y, 144, 252, pInfo );

}

MOboolean moEffectMovie::Finish()
{
    SelectScriptFunction("Finish");
    RunSelectedFunction();

	m_pTrackerData = NULL;

	moScript::FinishScript();
	Images.Finish();
    PreFinish();
	return true;
}

void moEffectMovie::DrawTrackerFeatures()
{
	int i, tw, th, v;
    float x, y;
/*
	if (m_pTrackerData) {
		tw = m_pTrackerData->m_VideoFormat.m_Width;
		th = m_pTrackerData->m_VideoFormat.m_Height;

		SelectScriptFunction("Draw");
		AddFunctionParam(tw);
		AddFunctionParam(th);
		RunSelectedFunction();

		for (i = 0; i < m_pTrackerData->m_NFeatures; i++)
		{
			x = 800.0 * m_pTrackerData->m_FeatureList->feature[i]->x / tw;
			y = 600.0 * m_pTrackerData->m_FeatureList->feature[i]->y / th;
			v = m_pTrackerData->m_FeatureList->feature[i]->val;

			if (v == KLT_TRACKED) glColor4f(0.5, 1.0, 0.5, 1.0);
			else if (v == KLT_NOT_FOUND) glColor4f(0.0, 0.0, 0.0, 1.0);
			else if (v == KLT_SMALL_DET) glColor4f(1.0, 0.0, 0.0, 1.0);
			else if (v == KLT_MAX_ITERATIONS) glColor4f(0.0, 1.0, 0.0, 1.0);
			else if (v == KLT_OOB) glColor4f(0.0, 0.0, 1.0, 1.0);
			else if (v == KLT_LARGE_RESIDUE) glColor4f(1.0, 1.0, 0.0, 1.0);
			glBegin(GL_QUADS);
				glVertex2f(x - 5, y - 5);
				glVertex2f(x - 5, y + 5);
				glVertex2f(x + 5, y + 5);
				glVertex2f(x + 5, y - 5);
			glEnd();
		}
	} else if (m_pTrackerGpuData) {
		tw = m_pTrackerGpuData->m_VideoFormat.m_Width;
		th = m_pTrackerGpuData->m_VideoFormat.m_Height;

		SelectScriptFunction("Draw");
		AddFunctionParam(tw);
		AddFunctionParam(th);
		RunSelectedFunction();

		for (i = 0; i < m_pTrackerGpuData->m_NFeatures; i++)
		{
			x = 800.0 * m_pTrackerGpuData->m_FeatureList->_list[i]->x / tw;
			y = 600.0 * m_pTrackerGpuData->m_FeatureList->_list[i]->y / th;
			v = m_pTrackerGpuData->m_FeatureList->_list[i]->valid;

			//if (v == KLT_TRACKED) glColor4f(0.5, 1.0, 0.5, 1.0);
			//else if (v == KLT_NOT_FOUND) glColor4f(0.0, 0.0, 0.0, 1.0);
			//else if (v == KLT_SMALL_DET) glColor4f(1.0, 0.0, 0.0, 1.0);
			//else if (v == KLT_MAX_ITERATIONS) glColor4f(0.0, 1.0, 0.0, 1.0);
			//else if (v == KLT_OOB) glColor4f(0.0, 0.0, 1.0, 1.0);
			//else if (v == KLT_LARGE_RESIDUE) glColor4f(1.0, 1.0, 0.0, 1.0);
			if ((bool)v==true) glColor4f(0.0, 1.0, 0.0, 1.0);
			else glColor4f(0.0, 0.0, 1.0, 1.0);
			glBegin(GL_QUADS);
				glVertex2f(x - 5, y - 5);
				glVertex2f(x - 5, y + 5);
				glVertex2f(x + 5, y + 5);
				glVertex2f(x + 5, y - 5);
			glEnd();
		}

	}*/
	if (m_pTrackerData) {
		tw = m_pTrackerData->GetVideoFormat().m_Width;
		th = m_pTrackerData->GetVideoFormat().m_Height;

		SelectScriptFunction("Draw");
		AddFunctionParam(tw);
		AddFunctionParam(th);
		RunSelectedFunction();

		for (i = 0; i < m_pTrackerData->GetFeaturesCount(); i++)
		{
			x = 800.0 * m_pTrackerData->GetFeature(i)->x / tw;
			y = 600.0 * m_pTrackerData->GetFeature(i)->y / th;
			v = m_pTrackerData->GetFeature(i)->val;

			if (v == MO_TRACKER_TRACKED) glColor4f(0.5, 1.0, 0.5, 1.0);
			else if (v == MO_TRACKER_NOT_FOUND) glColor4f(0.0, 0.0, 0.0, 1.0);
			else if (v == MO_TRACKER_SMALL_DET) glColor4f(1.0, 0.0, 0.0, 1.0);
			else if (v == MO_TRACKER_MAX_ITERATIONS) glColor4f(0.0, 1.0, 0.0, 1.0);
			else if (v == MO_TRACKER_OOB) glColor4f(0.0, 0.0, 1.0, 1.0);
			else if (v == MO_TRACKER_LARGE_RESIDUE) glColor4f(1.0, 1.0, 0.0, 1.0);
			glBegin(GL_QUADS);
				glVertex2f(x - 5, y - 5);
				glVertex2f(x - 5, y + 5);
				glVertex2f(x + 5, y + 5);
				glVertex2f(x + 5, y - 5);
			glEnd();
		}
	}

}

void moEffectMovie::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;//,*aux;
	MOint did,cid,state,valor;//e,estaux,elaux;
	moText txt;
	moSoundManager* SM = m_pResourceManager->GetSoundMan();
	moSoundEffect*	EQ = SM->GetEffects()->Get(1);
	float lowgain;
	float lowcutoff;
	float fxvalue;

	int TrackerId = -1;

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
				//VCR COMMANDS
				case MO_MOVIE_STOP:
					VCRCommand( MO_MOVIE_VCR_STOP );
					MODebug->Push("STOP");
					break;
				case MO_MOVIE_PLAY:
					VCRCommand( MO_MOVIE_VCR_PLAY );
					MODebug->Push("PLAY");
					break;
				case MO_MOVIE_REVERSE:
					VCRCommand( MO_MOVIE_VCR_REVERSE );
					MODebug->Push("REVERSE");
					break;
				case MO_MOVIE_FW:
					VCRCommand( MO_MOVIE_VCR_FW );
					MODebug->Push("FORWARD");
					break;
				case MO_MOVIE_RW:
					VCRCommand( MO_MOVIE_VCR_RW );
					MODebug->Push("REWIND");
					break;
				case MO_MOVIE_FF:
					VCRCommand( MO_MOVIE_VCR_FF );
					MODebug->Push("FAST FORWARD");
					break;
				case MO_MOVIE_FR:
					VCRCommand( MO_MOVIE_VCR_FR );
					MODebug->Push("FAST REWIND");
					break;
				case MO_MOVIE_SPEED:
					VCRCommand( MO_MOVIE_VCR_SPEED );
					MODebug->Push("PLAY AT SPEED");
					break;
				case MO_MOVIE_PAUSE:
					VCRCommand( MO_MOVIE_VCR_PAUSE );
					MODebug->Push("PAUSE");
					break;
				case MO_MOVIE_SEEK:
					VCRCommand( MO_MOVIE_VCR_SEEK );
					MODebug->Push("SEEK");
					break;
				case MO_MOVIE_PREVFRAME:
					VCRCommand( MO_MOVIE_VCR_PREVFRAME );
					MODebug->Push("PREVFRAME");
					break;
				case MO_MOVIE_NEXTFRAME:
					VCRCommand( MO_MOVIE_VCR_NEXTFRAME );
					MODebug->Push("NEXTFRAME");
					break;
				case MO_MOVIE_LOOP:
					this->m_bLoop = !this->m_bLoop;
					if (m_bLoop) MODebug->Push(moText("Looped"));
					else MODebug->Push(moText("Unlooped"));
					break;
				case MO_MOVIE_GAIN_UP:
					if (Sound) {
						Sound->SetVolume(+0.1);
						MODebug->Push(moText("GainUp: ")+FloatToStr(Sound->GetVolume()));
					}
					break;
				case MO_MOVIE_GAIN_DOWN:
					if (Sound) {
						Sound->SetVolume(-0.1);
						MODebug->Push(moText("GainDown: ")+FloatToStr(Sound->GetVolume()));
					}
					break;
//1.0 AL_EQUALIZER_LOW_GAIN [0.126, 7.943] 1.0

                #ifdef MO_WIN32

                //no hay ecualizacion en linux????

				case MO_MOVIE_EQ_LOW_GAIN_UP:
					EQ->GetParameterF( AL_EQUALIZER_LOW_GAIN , &lowgain );
					lowgain+= 0.1;
					if (0.126>lowgain) lowgain = 0.126;
					else if (lowgain>7.9) lowgain = 7.9;
					EQ->SetParameterF( AL_EQUALIZER_LOW_GAIN, lowgain );
					MODebug->Push( moText("LowGainUp: ")+FloatToStr(lowgain) );

					break;
				case MO_MOVIE_EQ_LOW_GAIN_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_LOW_GAIN , &lowgain );
					lowgain-= 0.1;
					if (0.126>lowgain) lowgain = 0.126;
					else if (lowgain>7.9) lowgain = 7.9;
					EQ->SetParameterF( AL_EQUALIZER_LOW_GAIN, lowgain );
					MODebug->Push( moText("LowGainDown: ")+FloatToStr(lowgain) );
					break;
//1.0 AL_EQUALIZER_LOW_CUTOFF Hz [50.0, 800.0] 200.0
				case MO_MOVIE_EQ_LOW_CUTOFF_UP:
					EQ->GetParameterF( AL_EQUALIZER_LOW_CUTOFF , &lowcutoff );
					lowcutoff+= 10.0;
					if (50.0>lowcutoff) lowcutoff = 50.0;
					else if (lowcutoff>800.0) lowcutoff = 800.0;
					EQ->SetParameterF( AL_EQUALIZER_LOW_CUTOFF, lowcutoff );
					MODebug->Push( moText("LowCutoffUp: ")+FloatToStr(lowcutoff) );
					break;
				case MO_MOVIE_EQ_LOW_CUTOFF_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_LOW_CUTOFF , &lowcutoff );
					lowcutoff-= 10.0;
					if (50.0>lowcutoff) lowcutoff = 50.0;
					else if (lowcutoff>800.0) lowcutoff = 800.0;
					EQ->SetParameterF( AL_EQUALIZER_LOW_CUTOFF, lowgain );
					MODebug->Push( moText("LowCutoffDown: ")+FloatToStr(lowcutoff) );
					break;
//1.0 AL_EQUALIZER_MID1_GAIN [0.126, 7.943] 1.0
				case MO_MOVIE_EQ_MID1_GAIN_UP:
					EQ->GetParameterF( AL_EQUALIZER_MID1_GAIN , &fxvalue );
					fxvalue+= 10;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_MID1_GAIN, fxvalue );
					MODebug->Push( moText("Mid1GainUp: ")+FloatToStr(fxvalue) );
					break;
				case MO_MOVIE_EQ_MID1_GAIN_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_MID1_GAIN , &fxvalue );
					fxvalue-= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_MID1_GAIN, fxvalue );
					MODebug->Push(  moText("Mid1GainDown: ")+ FloatToStr(fxvalue) );
					break;
//1.0 AL_EQUALIZER_MID1_CENTER Hz [200.0, 3000.0] 500.0
				case MO_MOVIE_EQ_MID1_CENTER_UP:
					EQ->GetParameterF( AL_EQUALIZER_MID1_CENTER , &fxvalue );
					fxvalue+= 20;
					if (200>fxvalue) fxvalue = 200;
					else if (fxvalue>3000) fxvalue = 3000;
					EQ->SetParameterF( AL_EQUALIZER_MID1_CENTER, fxvalue );
					MODebug->Push( moText("Mid1CenterUp: ")+ FloatToStr(fxvalue) );
					break;
				case MO_MOVIE_EQ_MID1_CENTER_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_MID1_CENTER , &fxvalue );
					fxvalue-= 20;
					if (200>fxvalue) fxvalue = 200;
					else if (fxvalue>3000) fxvalue = 3000;
					EQ->SetParameterF( AL_EQUALIZER_MID1_CENTER, fxvalue );
					MODebug->Push( moText("Mid1CenterDown: ")+ FloatToStr(fxvalue) );
					break;
//1.0 AL_EQUALIZER_MID1_WIDTH [0.01, 1.0] 1.0
				case MO_MOVIE_EQ_MID1_WIDTH_UP:
					EQ->GetParameterF( AL_EQUALIZER_MID1_WIDTH , &fxvalue );
					fxvalue+= 0.1;
					if (0.01>fxvalue) fxvalue = 0.01;
					else if (fxvalue>1.0) fxvalue = 1.0;
					EQ->SetParameterF( AL_EQUALIZER_MID1_WIDTH, fxvalue );
					MODebug->Push( moText("Mid1WidthUp: ")+FloatToStr(fxvalue) );
					break;
				case MO_MOVIE_EQ_MID1_WIDTH_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_MID1_WIDTH , &fxvalue );
					fxvalue-= 0.1;
					if (0.01>fxvalue) fxvalue = 0.01;
					else if (fxvalue>1.0) fxvalue = 1.0;
					EQ->SetParameterF( AL_EQUALIZER_MID1_WIDTH, fxvalue );
					MODebug->Push( moText("Mid1WidthDown: ")+FloatToStr(fxvalue) );
					break;
//1.0 AL_EQUALIZER_MID2_GAIN [0.126, 7.943] 1.0
				case MO_MOVIE_EQ_MID2_GAIN_UP:
					EQ->GetParameterF( AL_EQUALIZER_MID2_GAIN , &fxvalue );
					fxvalue+= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_MID2_GAIN, fxvalue );
					MODebug->Push( moText("Mid2GainUp: ")+FloatToStr(fxvalue) );
					break;
				case MO_MOVIE_EQ_MID2_GAIN_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_MID2_GAIN , &fxvalue );
					fxvalue-= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_MID2_GAIN, fxvalue );
					MODebug->Push( moText("Mid2GainDown: ")+FloatToStr(fxvalue) );
					break;
//1.0 AL_EQUALIZER_MID2_CENTER [1000.0, 8000.0] 3000.0
				case MO_MOVIE_EQ_MID2_CENTER_UP:
					EQ->GetParameterF( AL_EQUALIZER_MID2_CENTER , &fxvalue );
					fxvalue+= 50;
					if (1000.0>fxvalue) fxvalue = 1000.0;
					else if (fxvalue>8000.0) fxvalue = 8000.0;
					EQ->SetParameterF( AL_EQUALIZER_MID2_CENTER, fxvalue );
					MODebug->Push( moText("Mid2CenterUp: ")+FloatToStr(fxvalue) );
					break;
				case MO_MOVIE_EQ_MID2_CENTER_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_MID2_CENTER , &fxvalue );
					fxvalue-= 50;
					if (1000.0>fxvalue) fxvalue = 1000.0;
					else if (fxvalue>8000.0) fxvalue = 8000.0;
					EQ->SetParameterF( AL_EQUALIZER_MID2_CENTER, fxvalue );
					MODebug->Push( moText("Mid2CenterDown: ")+FloatToStr(fxvalue) );
					break;
//1.0 AL_EQUALIZER_MID2_WIDTH [0.01, 1.0] 1.0
				case MO_MOVIE_EQ_MID2_WIDTH_UP:
					EQ->GetParameterF( AL_EQUALIZER_MID2_WIDTH , &fxvalue );
					fxvalue+= 0.1;
					if (0.01>fxvalue) fxvalue = 0.01;
					else if (fxvalue>1.0) fxvalue = 1.0;
					EQ->SetParameterF( AL_EQUALIZER_MID2_WIDTH, fxvalue );
					MODebug->Push( moText("Mid2WidthUp: ")+FloatToStr(fxvalue) );
					break;
				case MO_MOVIE_EQ_MID2_WIDTH_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_MID2_WIDTH , &fxvalue );
					fxvalue-= 0.1;
					if (0.01>fxvalue) fxvalue = 0.01;
					else if (fxvalue>1.0) fxvalue = 1.0;
					EQ->SetParameterF( AL_EQUALIZER_MID2_WIDTH, fxvalue );
					MODebug->Push( moText("Mid2WidthDown: ")+FloatToStr(fxvalue) );
					break;
//1.0 AL_EQUALIZER_HIGH_GAIN [0.126, 7.943] 1.0
				case MO_MOVIE_EQ_HIGH_GAIN_UP:
					EQ->GetParameterF( AL_EQUALIZER_HIGH_GAIN , &fxvalue );
					fxvalue+= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_HIGH_GAIN, fxvalue );
					MODebug->Push( moText("HighGainUp: ")+FloatToStr(fxvalue) );
					break;
				case MO_MOVIE_EQ_HIGH_GAIN_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_HIGH_GAIN , &fxvalue );
					fxvalue-= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_HIGH_GAIN, fxvalue );
					MODebug->Push(  moText("HighGainDown: ")+FloatToStr(fxvalue) );
					break;
//1.0 AL_EQUALIZER_HIGH_CUTOFF Hz [4000.0, 16000.0] 6000.0
				case MO_MOVIE_EQ_HIGH_CUTOFF_UP:
					EQ->GetParameterF( AL_EQUALIZER_HIGH_CUTOFF , &fxvalue );
					fxvalue+= 100;
					if (4000.0>fxvalue) fxvalue = 4000.0;
					else if (fxvalue>16000.0) fxvalue = 16000.0;
					EQ->SetParameterF( AL_EQUALIZER_HIGH_CUTOFF, fxvalue );
					MODebug->Push( moText("HighCutoffUp: ")+FloatToStr(fxvalue) );
					break;
				case MO_MOVIE_EQ_HIGH_CUTOFF_DOWN:
					EQ->GetParameterF( AL_EQUALIZER_HIGH_CUTOFF , &fxvalue );
					fxvalue-= 100;
					if (4000.0>fxvalue) fxvalue = 4000.0;
					else if (fxvalue>16000.0) fxvalue = 16000.0;
					EQ->SetParameterF( AL_EQUALIZER_HIGH_CUTOFF, fxvalue );
					MODebug->Push( moText("HighCutoffDown: ")+FloatToStr(fxvalue) );
					break;

                #endif

			}
		temp = temp->next;
		}
	}

}

void
moEffectMovie::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);


	//Procesar Inlets

	for(int i=0; i<m_Inlets.Count(); i++) {
		moInlet* pInlet = m_Inlets[i];
		if (pInlet->Updated() && pInlet->GetConnectorLabelName()==moText("TRACKER")) {

			m_pTrackerData = (moTrackerSystemData *)pInlet->GetData()->Pointer();
			if (m_pTrackerData && !m_bTrackerInit) {
				m_bTrackerInit = true;

				SelectScriptFunction("Reset");
				RunSelectedFunction();
				//MODebug->Push(IntToStr(TrackerId));
			}
		}
        /*
		if (pInlet->Updated() && pInlet->GetConnectorLabelName()==moText("TRACKERKLT")) {

			m_pTrackerData = (moTrackerKLTSystemData *)pInlet->GetData()->Pointer();
			if (m_pTrackerData && !m_bTrackerInit) {
				m_bTrackerInit = true;

				SelectScriptFunction("Reset");
				RunSelectedFunction();
				//MODebug->Push(IntToStr(TrackerId));
			}
		} else if (pInlet->Updated() && pInlet->GetConnectorLabelName()==moText("TRACKERGPUKLT")) {
			m_pTrackerGpuData = (moTrackerGpuKLTSystemData *)pInlet->GetData()->Pointer();
			if (m_pTrackerGpuData && !m_bTrackerInit) {
				m_bTrackerInit = true;

				SelectScriptFunction("Reset");
				RunSelectedFunction();
				//MODebug->Push(IntToStr(TrackerId));
			}
		}*/
	}

}

moConfigDefinition *
moEffectMovie::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("movies"), MO_PARAM_TEXTURE, MOVIE_MOVIES );
	p_configdefinition->Add( moText("sounds"), MO_PARAM_SOUND, MOVIE_SOUNDS );
	p_configdefinition->Add( moText("script"), MO_PARAM_SCRIPT, MOVIE_SCRIPT );
	p_configdefinition->Add( moText("moviemode"), MO_PARAM_NUMERIC, MOVIE_MODE );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, MOVIE_BLENDING);
	p_configdefinition->Add( moText("startplaying"), MO_PARAM_NUMERIC, MOVIE_STARTPLAYING );
	p_configdefinition->Add( moText("loop"), MO_PARAM_NUMERIC, MOVIE_LOOP );
	p_configdefinition->Add( moText("showtrackdata"), MO_PARAM_NUMERIC, MOVIE_SHOWTRACKDATA );
	p_configdefinition->Add( moText("showmoviedata"), MO_PARAM_NUMERIC, MOVIE_SHOWMOVIEDATA );
	p_configdefinition->Add( moText("interpolation"), MO_PARAM_NUMERIC, MOVIE_INTERPOLATION );
	p_configdefinition->Add( moText("pos_text_x"), MO_PARAM_FUNCTION, MOVIE_POSTEXX );
	p_configdefinition->Add( moText("pos_text_y"), MO_PARAM_FUNCTION, MOVIE_POSTEXY );
	p_configdefinition->Add( moText("anc_text_x"), MO_PARAM_FUNCTION, MOVIE_ANCTEXX );
	p_configdefinition->Add( moText("alt_text_y"), MO_PARAM_FUNCTION, MOVIE_ALTTEXY );
	p_configdefinition->Add( moText("pos_cuad_x"), MO_PARAM_FUNCTION, MOVIE_POSCUADX );
	p_configdefinition->Add( moText("pos_cuad_y"), MO_PARAM_FUNCTION, MOVIE_POSCUADY );
	p_configdefinition->Add( moText("anc_cuad_x"), MO_PARAM_FUNCTION, MOVIE_ANCCUADX );
	p_configdefinition->Add( moText("alt_cuad_y"), MO_PARAM_FUNCTION, MOVIE_ALTCUADY );
	return p_configdefinition;
}

//CUSTOM FUNCTIONS


//play en funcion de play speed:
void moEffectMovie::VCRPlaySpeed() {

	//if (m_TAnim)
	//if (!m_TAnim->IsInterpolating()) {


		m_FramePositionAux = m_FramePosition;
		m_FramePositionInc = (double)( m_Ticks - m_TicksAux ) * m_PlaySpeed * m_FramesPerSecond / ((double)1000.0) ;
		m_FramePositionF+= m_FramePositionInc;
		if ( m_FramePositionF < 0.0 ) m_FramePositionF = 0.0;
		m_FramePosition = (long) m_FramePositionF;
		if ( m_FramePosition >= m_FramesLength ) m_FramePosition = (m_FramesLength-1);
	//}
	/*
	if (Sound) {

		MOlong bytesperframe = Sound->GetBufferSize() / m_FramesLength;

		if ( m_PlaySpeed == 1.0 && !m_bPlayingSound ) {
			if ((m_FramePosition * bytesperframe) < Sound->GetBufferSize() ) {
				Sound->PlaySample( m_FramePosition * bytesperframe );
				m_bPlayingSound = true;
			}
		} else if (m_PlaySpeed==0.0 || m_FramePosition==0) {
			Sound->Stop();
			m_bPlayingSound = false;
		}

		//RESYNC SOUND WITH VIDEO
		if (m_bPlayingSound) {

			if (bytesperframe>0 && m_FramesLength>0) {
				MOint async = (Sound->GetActualSample() / bytesperframe )  - m_FramePosition;

				if ( fabs((double)async) > 10.0 ) {

					Sound->PlaySample( m_FramePosition * bytesperframe );

				}
			}
		}

	}*/

}

void moEffectMovie::SpeedRegulation( MOfloat p_fMinSpeed, MOfloat p_fMaxSpeed ) {

	MOfloat speedF;

	MOlong deltaticks;
	MOfloat deltaticksF;

	deltaticks = m_TicksAux - m_Ticks;
	deltaticksF = deltaticks;

	speedF = (double) 1000.0 * (double) (m_FramePositionFAux - m_FramePositionF) / (deltaticksF * m_FramesPerSecond);

	//MODebug->Push(FloatToStr(speedF));

	if ( speedF > p_fMaxSpeed ) {
		m_PlaySpeed = p_fMaxSpeed;
		VCRPlaySpeed();
	}

	if ( speedF < p_fMinSpeed ) {
		m_PlaySpeed = p_fMinSpeed;
		VCRPlaySpeed();
	}

}

void moEffectMovie::VCRCommand( moEffectMovieVCRCommand p_Command, MOint p_iValue, MOfloat p_fValue ) {
	switch(p_Command) {
		case MO_MOVIE_VCR_STOP:
			m_PlayState = MO_MOVIE_PLAYSTATE_STOPPED;
			m_PlaySpeed = 0.0;
			/*if (Sound) Sound->Stop();*/
			/*if (Sound) Sound->Stop();*/
			m_SoundSystem.Stop();
			m_bPlayingSound = false;
			break;

		case MO_MOVIE_VCR_PLAY:
			m_PlayState = MO_MOVIE_PLAYSTATE_PLAYING;
			m_PlaySpeed = 1.0;
			m_SoundSystem.Play();
			break;

		case MO_MOVIE_VCR_PAUSE:
			m_PlaySpeed = 0.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_PAUSED;
			/*if (Sound) Sound->Pause();*/
			m_SoundSystem.Pause();
			m_bPlayingSound = false;
			break;

		case MO_MOVIE_VCR_REVERSE:
			m_PlaySpeed = -1.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_REVERSE;
			break;

		case MO_MOVIE_VCR_FW:
			m_PlaySpeed = 2.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;

		case MO_MOVIE_VCR_RW:
			m_PlaySpeed = -2.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;

		case MO_MOVIE_VCR_FF:
			m_PlaySpeed = 4.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;
		case MO_MOVIE_VCR_FR:
			m_PlaySpeed = -4.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;
		case MO_MOVIE_VCR_SEEK:
			//nada por ahora
			m_PlayState = MO_MOVIE_PLAYSTATE_SEEKING;
			m_FramePosition = p_iValue;
			break;
		case MO_MOVIE_VCR_SPEED:
			m_PlaySpeed = p_fValue;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;
		case MO_MOVIE_VCR_PREVFRAME:
			if ( (m_FramePosition > 0) && (m_FramePosition < m_FramesLength) ) {
					m_FramePosition = m_FramePosition - 1;
					m_FramePositionF = m_FramePosition;
			}
			m_PlayState = MO_MOVIE_PLAYSTATE_PAUSED;
			break;
		case MO_MOVIE_VCR_NEXTFRAME:
			if ( (m_FramePosition >= 0) && (m_FramePosition < (m_FramesLength-1)) ) {
					m_FramePosition = m_FramePosition + 1;
					m_FramePositionF = m_FramePosition;
			}
			m_PlayState = MO_MOVIE_PLAYSTATE_PAUSED;
			break;
		case MO_MOVIE_VCR_LOOP:
			m_bLoop = !m_bLoop;
			break;
	}

}


int moEffectMovie::ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber)
{
    switch (iFunctionNumber - m_iMethodBase)
    {
        case 0:
            return GetFeature(vm);
        case 1:
            return SpeedRegulation(vm);
        case 2:
            return VCRStop(vm);
        case 3:
            return VCRPlay(vm);
        case 4:
            return VCRPause(vm);
        case 5:
            return VCRReverse(vm);
        case 6:
            return VCRForward(vm);
        case 7:
            return VCRRewind(vm);
        case 8:
            return VCRFForward(vm);
        case 9:
            return VCRFRewind(vm);
        case 10:
            return VCRSeek(vm);
        case 11:
            return VCRSpeed(vm);
        case 12:
            return VCRPrevFrame(vm);
        case 13:
            return VCRNextFrame(vm);
        case 14:
            return VCRLoop(vm);
        case 15:
            return VCRPlaySpeed(vm);
        case 16:
            return GetFramePosition(vm);
        case 17:
            return GetFramePositionF(vm);
        case 18:
            return GetPlaySpeed(vm);
        case 19:
            return SetFramePosition(vm);
        case 20:
            return SetFramePositionF(vm);
        case 21:
            return SetPlaySpeed(vm);
        case 22:
            return GetTicks(vm);
        case 23:
            return DrawLine(vm);
        case 24:
            return PushDebugString(vm);
	}
    return 0;
}

void moEffectMovie::HandleReturns(moLuaVirtualMachine& vm, const char *strFunc)
{
    if (strcmp (strFunc, "Run") == 0)
    {
        lua_State *state = (lua_State *) vm;
        MOint script_result = (MOint) lua_tonumber (state, 1);
		if (script_result != 0)
			MODebug->Push(moText("Script returned error code: ") + (moText)IntToStr(script_result));
    }
}

void moEffectMovie::RegisterFunctions()
{
    m_iMethodBase = RegisterFunction("GetFeature");

	RegisterFunction("SpeedRegulation");

	RegisterFunction("VCRStop");
	RegisterFunction("VCRPlay");
	RegisterFunction("VCRPause");
	RegisterFunction("VCRReverse");
	RegisterFunction("VCRForward");
	RegisterFunction("VCRRewind");
	RegisterFunction("VCRFForward");
	RegisterFunction("VCRFRewind");
	RegisterFunction("VCRSeek");
	RegisterFunction("VCRSpeed");
	RegisterFunction("VCRPrevFrame");
	RegisterFunction("VCRNextFrame");
	RegisterFunction("VCRLoop");
	RegisterFunction("VCRPlaySpeed");

	RegisterFunction("GetFramePosition");
	RegisterFunction("GetFramePositionF");
	RegisterFunction("GetPlaySpeed");

	RegisterFunction("SetFramePosition");
	RegisterFunction("SetFramePositionF");
	RegisterFunction("SetPlaySpeed");

	RegisterFunction("GetTicks");

	RegisterFunction("DrawLine");

	RegisterFunction("PushDebugString");
}

int moEffectMovie::GetFeature(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    float x, y, v;

    if (m_pTrackerData) {
		x = m_pTrackerData->GetFeature(i)->x;
		y = m_pTrackerData->GetFeature(i)->y;
		v = m_pTrackerData->GetFeature(i)->val;
	}
    /*
	if (m_pTrackerData) {
		x = m_pTrackerData->m_FeatureList->feature[i]->x;
		y = m_pTrackerData->m_FeatureList->feature[i]->y;
		v = m_pTrackerData->m_FeatureList->feature[i]->val;
	} else if (m_pTrackerGpuData) {
		x = m_pTrackerGpuData->m_FeatureList->_list[i]->x;
		y = m_pTrackerGpuData->m_FeatureList->_list[i]->y;
		v = m_pTrackerGpuData->m_FeatureList->_list[i]->valid;
	}*/

	lua_pushnumber(state, (lua_Number)x);
	lua_pushnumber(state, (lua_Number)y);
	lua_pushnumber(state, (lua_Number)v);

    return 3;
}

int moEffectMovie::SpeedRegulation(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOfloat min_speed = (MOfloat) lua_tonumber (state, 1);
    MOfloat max_speed = (MOfloat) lua_tonumber (state, 2);
	SpeedRegulation(min_speed, max_speed);

    return 0;
}

int moEffectMovie::VCRStop(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_STOP);
    return 0;
}

int moEffectMovie::VCRPlay(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_PLAY);
    return 0;
}

int moEffectMovie::VCRPause(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_PAUSE);
    return 0;
}

int moEffectMovie::VCRReverse(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_REVERSE);
    return 0;
}

int moEffectMovie::VCRForward(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_FW);
    return 0;
}

int moEffectMovie::VCRRewind(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_RW);
    return 0;
}

int moEffectMovie::VCRFForward(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_FF);
    return 0;
}

int moEffectMovie::VCRFRewind(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_FR);
    return 0;
}

int moEffectMovie::VCRSeek(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_SEEK);
    return 0;
}

int moEffectMovie::VCRSpeed(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_SPEED);
    return 0;
}

int moEffectMovie::VCRPrevFrame(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_PREVFRAME);
    return 0;
}

int moEffectMovie::VCRNextFrame(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_NEXTFRAME);
    return 0;
}

int moEffectMovie::VCRLoop(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_LOOP);
    return 0;
}

int moEffectMovie::VCRPlaySpeed(moLuaVirtualMachine& vm)
{
	VCRPlaySpeed();
    return 0;
}

int moEffectMovie::GetFramePosition(moLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	lua_pushnumber(state, (lua_Number)m_FramePosition);
    return 1;
}

int moEffectMovie::GetFramePositionF(moLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	lua_pushnumber(state, (lua_Number)m_FramePositionF);
    return 1;
}

int moEffectMovie::GetPlaySpeed(moLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	lua_pushnumber(state, (lua_Number)m_PlaySpeed);
    return 1;
}

int moEffectMovie::SetFramePosition(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
    m_FramePosition = (MOlong) lua_tonumber (state, 1);
    return 0;
}

int moEffectMovie::SetFramePositionF(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
    m_FramePositionF = (MOdouble) lua_tonumber (state, 1);
    return 0;
}

int moEffectMovie::SetPlaySpeed(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
    m_PlaySpeed = (MOfloat) lua_tonumber (state, 1);
    return 0;
}

int moEffectMovie::GetTicks(moLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	lua_pushnumber(state, (lua_Number)m_Ticks);
    return 1;
}

int moEffectMovie::DrawLine(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOfloat x0 = (MOfloat) lua_tonumber (state, 1);
    MOfloat y0 = (MOfloat) lua_tonumber (state, 2);
    MOfloat x1 = (MOfloat) lua_tonumber (state, 3);
    MOfloat y1 = (MOfloat) lua_tonumber (state, 4);
    MOfloat w = (MOfloat) lua_tonumber (state, 5);
    MOfloat r = (MOfloat) lua_tonumber (state, 6);
    MOfloat g = (MOfloat) lua_tonumber (state, 7);
    MOfloat b = (MOfloat) lua_tonumber (state, 8);
    MOfloat a = (MOfloat) lua_tonumber (state, 9);

	glColor4f(r, g, b, a);
    glLineWidth(w);
    glBegin(GL_LINES);
        glVertex2f(x0, y0);
        glVertex2f(x1, y1);
    glEnd();

    return 0;
}

int moEffectMovie::PushDebugString(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
	if (lua_isboolean(state,1)) {
		bool vb = lua_toboolean(state,1);
		vb ? m_tScriptDebug.Push(moText("true")) : m_tScriptDebug.Push(moText("false"));
	} else {
		char *text = (char *) lua_tostring (state, 1);
		m_tScriptDebug.Push(moText(text));
	}

    return 0;
}
