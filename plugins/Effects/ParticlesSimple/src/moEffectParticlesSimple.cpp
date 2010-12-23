/*******************************************************************************

                              moEffectParticlesSimple.cpp

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

#include "moEffectParticlesSimple.h"


#include "moArray.cpp"
moDefineDynamicArray( moParticlesSimpleArray )

//========================
//  Factory
//========================

moEffectParticlesSimpleFactory *m_EffectParticlesSimpleFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectParticlesSimpleFactory==NULL)
		m_EffectParticlesSimpleFactory = new moEffectParticlesSimpleFactory();
	return(moEffectFactory*) m_EffectParticlesSimpleFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectParticlesSimpleFactory;
	m_EffectParticlesSimpleFactory = NULL;
}

moEffect*  moEffectParticlesSimpleFactory::Create() {
	return new moEffectParticlesSimple();
}

void moEffectParticlesSimpleFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectParticlesSimple::moEffectParticlesSimple() {
	SetName("particlessimple");

	///stereo for this effect is activated by default
	state.stereo = MO_ACTIVATED;
}

moEffectParticlesSimple::~moEffectParticlesSimple() {
	Finish();
}

moConfigDefinition *
moEffectParticlesSimple::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );

	p_configdefinition->Add( moText("particlecolor"), MO_PARAM_COLOR, PARTICLES_PARTICLECOLOR );

	p_configdefinition->Add( moText("font"), MO_PARAM_FONT, PARTICLES_FONT, moValue( "Default", "TXT", "0", "NUM", "32.0", "NUM") );
	p_configdefinition->Add( moText("text"), MO_PARAM_TEXT, PARTICLES_TEXT, moValue( "Insert text in text parameter", "TXT") );
	p_configdefinition->Add( moText("ortho"), MO_PARAM_NUMERIC, PARTICLES_ORTHO, moValue( "0", "NUM").Ref() );


	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("folders"), MO_PARAM_TEXTUREFOLDER, PARTICLES_FOLDERS, moValue( "", "TXT") );
	p_configdefinition->Add( moText("texture_mode"), MO_PARAM_NUMERIC, PARTICLES_TEXTUREMODE, moValue( "0", "NUM").Ref() );

	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, PARTICLES_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_NUMERIC, PARTICLES_WIDTH, moValue( "9", "NUM") );
	p_configdefinition->Add( moText("height"), MO_PARAM_NUMERIC, PARTICLES_HEIGHT, moValue( "9", "NUM") );

    p_configdefinition->Add( moText("sizex"), MO_PARAM_FUNCTION, PARTICLES_SIZEX, moValue( "10.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizey"), MO_PARAM_FUNCTION, PARTICLES_SIZEY, moValue( "10.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizez"), MO_PARAM_FUNCTION, PARTICLES_SIZEY, moValue( "10.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("gravity"), MO_PARAM_FUNCTION, PARTICLES_GRAVITY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viscosity"), MO_PARAM_FUNCTION, PARTICLES_VISCOSITY, moValue( "0.8", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("maxage"), MO_PARAM_NUMERIC, PARTICLES_MAXAGE, moValue( "3000", "NUM").Ref() );
	p_configdefinition->Add( moText("emitionperiod"), MO_PARAM_NUMERIC, PARTICLES_EMITIONPERIOD, moValue( "10", "NUM").Ref() );
	p_configdefinition->Add( moText("emitionrate"), MO_PARAM_NUMERIC, PARTICLES_EMITIONRATE, moValue( "1", "NUM").Ref() );
	p_configdefinition->Add( moText("deathperiod"), MO_PARAM_NUMERIC, PARTICLES_DEATHPERIOD, moValue( "1", "NUM").Ref() );
	p_configdefinition->Add( moText("particlescript"), MO_PARAM_SCRIPT, PARTICLES_SCRIPT );

	p_configdefinition->Add( moText("randommethod"), MO_PARAM_NUMERIC, PARTICLES_RANDOMMETHOD, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("creationmethod"), MO_PARAM_NUMERIC, PARTICLES_CREATIONMETHOD, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("orientationmode"), MO_PARAM_NUMERIC, PARTICLES_ORIENTATIONMODE, moValue( "0", "NUM").Ref() );

	p_configdefinition->Add( moText("fadein"), MO_PARAM_FUNCTION, PARTICLES_FADEIN, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("fadeout"), MO_PARAM_FUNCTION, PARTICLES_FADEOUT, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizein"), MO_PARAM_FUNCTION, PARTICLES_SIZEIN, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizeout"), MO_PARAM_FUNCTION, PARTICLES_SIZEOUT, moValue( "0", "FUNCTION").Ref() );



	p_configdefinition->Add( moText("attractortype"), MO_PARAM_NUMERIC, PARTICLES_ATTRACTORTYPE, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("attractormode"), MO_PARAM_NUMERIC, PARTICLES_ATTRACTORMODE, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("attractorvectorx"), MO_PARAM_FUNCTION, PARTICLES_ATTRACTORVECTOR_X, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("attractorvectory"), MO_PARAM_FUNCTION, PARTICLES_ATTRACTORVECTOR_Y, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("attractorvectorz"), MO_PARAM_FUNCTION, PARTICLES_ATTRACTORVECTOR_Z, moValue( "0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("emittertype"), MO_PARAM_NUMERIC, PARTICLES_EMITTERTYPE, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("emittervectorx"), MO_PARAM_FUNCTION, PARTICLES_EMITTERVECTOR_X, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("emittervectory"), MO_PARAM_FUNCTION, PARTICLES_EMITTERVECTOR_Y, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("emittervectorz"), MO_PARAM_FUNCTION, PARTICLES_EMITTERVECTOR_Z, moValue( "0", "FUNCTION").Ref() );


	p_configdefinition->Add( moText("randomposition"), MO_PARAM_FUNCTION, PARTICLES_RANDOMPOSITION, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randompositionx"), MO_PARAM_FUNCTION, PARTICLES_RANDOMPOSITION_X, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randompositiony"), MO_PARAM_FUNCTION, PARTICLES_RANDOMPOSITION_Y, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randompositionz"), MO_PARAM_FUNCTION, PARTICLES_RANDOMPOSITION_Y, moValue( "0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("randomvelocity"), MO_PARAM_FUNCTION, PARTICLES_RANDOMVELOCITY, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randomvelocityx"), MO_PARAM_FUNCTION, PARTICLES_RANDOMVELOCITY_X, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randomvelocityy"), MO_PARAM_FUNCTION, PARTICLES_RANDOMVELOCITY_Y, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randomvelocityz"), MO_PARAM_FUNCTION, PARTICLES_RANDOMVELOCITY_Z, moValue( "1.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("randommotion"), MO_PARAM_FUNCTION, PARTICLES_RANDOMMOTION, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randommotionx"), MO_PARAM_FUNCTION, PARTICLES_RANDOMMOTION_X, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randommotiony"), MO_PARAM_FUNCTION, PARTICLES_RANDOMMOTION_Y, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randommotionz"), MO_PARAM_FUNCTION, PARTICLES_RANDOMMOTION_Z, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("rotatex_particle"), MO_PARAM_ROTATEX, PARTICLES_ROTATEX_PARTICLE, moValue( "0.0", "FUNCTION").Ref() );
    p_configdefinition->Add( moText("rotatey_particle"), MO_PARAM_ROTATEY, PARTICLES_ROTATEY_PARTICLE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez_particle"), MO_PARAM_ROTATEZ, PARTICLES_ROTATEZ_PARTICLE, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("scalex_particle"), MO_PARAM_SCALEX, PARTICLES_SCALEX_PARTICLE, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley_particle"), MO_PARAM_SCALEY, PARTICLES_SCALEY_PARTICLE, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalez_particle"), MO_PARAM_SCALEZ, PARTICLES_SCALEZ_PARTICLE, moValue( "1.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("time_to_revelation"), MO_PARAM_NUMERIC, PARTICLES_TIMETOREVELATION, moValue( "2000", "NUM") );
	p_configdefinition->Add( moText("time_of_revelation"), MO_PARAM_NUMERIC, PARTICLES_TIMEOFREVELATION, moValue( "3000", "NUM") );
	p_configdefinition->Add( moText("time_to_restoration"), MO_PARAM_NUMERIC, PARTICLES_TIMETORESTORATION, moValue( "2000", "NUM") );
	p_configdefinition->Add( moText("time_of_restoration"), MO_PARAM_NUMERIC, PARTICLES_TIMEOFRESTORATION, moValue( "3000", "NUM") );
	p_configdefinition->Add( moText("drawing_features"), MO_PARAM_NUMERIC, PARTICLES_DRAWINGFEATURES, moValue( "0", "NUM") );

	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, PARTICLES_TRANSLATEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, PARTICLES_TRANSLATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, PARTICLES_TRANSLATEZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, PARTICLES_ROTATEX, moValue( "0.0", "FUNCTION").Ref() );
    p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, PARTICLES_ROTATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, PARTICLES_ROTATEZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, PARTICLES_SCALEX, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, PARTICLES_SCALEY, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, PARTICLES_SCALEZ, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyex"), MO_PARAM_FUNCTION, PARTICLES_EYEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyey"), MO_PARAM_FUNCTION, PARTICLES_EYEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyez"), MO_PARAM_FUNCTION, PARTICLES_EYEZ, moValue( "10.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewx"), MO_PARAM_FUNCTION, PARTICLES_VIEWX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewy"), MO_PARAM_FUNCTION, PARTICLES_VIEWY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewz"), MO_PARAM_FUNCTION, PARTICLES_VIEWZ, moValue( "0.0", "FUNCTION").Ref() );
	return p_configdefinition;
}

MOboolean
moEffectParticlesSimple::Init()
{
    if (!PreInit()) return false;

	moDefineParamIndex( PARTICLES_ALPHA, moText("alpha") );
	moDefineParamIndex( PARTICLES_COLOR, moText("color") );
	moDefineParamIndex( PARTICLES_PARTICLECOLOR, moText("particlecolor") );
	moDefineParamIndex( PARTICLES_SYNC, moText("syncro") );
	moDefineParamIndex( PARTICLES_PHASE, moText("phase") );
	moDefineParamIndex( PARTICLES_FONT, moText("font") );
	moDefineParamIndex( PARTICLES_TEXT, moText("text") );
	moDefineParamIndex( PARTICLES_ORTHO, moText("ortho") );


	moDefineParamIndex( PARTICLES_TEXTURE, moText("texture") );
	moDefineParamIndex( PARTICLES_FOLDERS, moText("folders") );
	moDefineParamIndex( PARTICLES_TEXTUREMODE, moText("texture_mode") );
	moDefineParamIndex( PARTICLES_BLENDING, moText("blending") );

	moDefineParamIndex( PARTICLES_WIDTH, moText("width") );
	moDefineParamIndex( PARTICLES_HEIGHT, moText("height") );
	moDefineParamIndex( PARTICLES_SIZEX, moText("sizex") );
	moDefineParamIndex( PARTICLES_SIZEY, moText("sizey") );
	moDefineParamIndex( PARTICLES_SIZEZ, moText("sizez") );

	moDefineParamIndex( PARTICLES_GRAVITY, moText("gravity") );
	moDefineParamIndex( PARTICLES_VISCOSITY, moText("viscosity") );

	moDefineParamIndex( PARTICLES_MAXAGE, moText("maxage") );
	moDefineParamIndex( PARTICLES_EMITIONPERIOD, moText("emitionperiod") );
	moDefineParamIndex( PARTICLES_EMITIONRATE, moText("emitionrate") );
	moDefineParamIndex( PARTICLES_DEATHPERIOD, moText("deathperiod") );

	moDefineParamIndex( PARTICLES_SCRIPT, moText("particlescript") );

	moDefineParamIndex( PARTICLES_FADEIN, moText("fadein") );
	moDefineParamIndex( PARTICLES_FADEOUT, moText("fadeout") );
	moDefineParamIndex( PARTICLES_SIZEIN, moText("sizein") );
	moDefineParamIndex( PARTICLES_SIZEOUT, moText("sizeout") );

	moDefineParamIndex( PARTICLES_RANDOMMETHOD, moText("randommethod") );
	moDefineParamIndex( PARTICLES_CREATIONMETHOD, moText("creationmethod") );
	moDefineParamIndex( PARTICLES_ORIENTATIONMODE, moText("orientationmode") );

	moDefineParamIndex( PARTICLES_RANDOMPOSITION, moText("randomposition") );
	moDefineParamIndex( PARTICLES_RANDOMPOSITION_X, moText("randompositionx") );
	moDefineParamIndex( PARTICLES_RANDOMPOSITION_Y, moText("randompositiony") );
	moDefineParamIndex( PARTICLES_RANDOMPOSITION_Z, moText("randompositionz") );

	moDefineParamIndex( PARTICLES_RANDOMVELOCITY, moText("randomvelocity") );
	moDefineParamIndex( PARTICLES_RANDOMVELOCITY_X, moText("randomvelocityx") );
	moDefineParamIndex( PARTICLES_RANDOMVELOCITY_Y, moText("randomvelocityy") );
	moDefineParamIndex( PARTICLES_RANDOMVELOCITY_Z, moText("randomvelocityz") );

	moDefineParamIndex( PARTICLES_RANDOMMOTION, moText("randommotion") );
	moDefineParamIndex( PARTICLES_RANDOMMOTION_X, moText("randommotionx") );
	moDefineParamIndex( PARTICLES_RANDOMMOTION_Y, moText("randommotiony") );
	moDefineParamIndex( PARTICLES_RANDOMMOTION_Z, moText("randommotionz") );

	moDefineParamIndex( PARTICLES_EMITTERTYPE, moText("emittertype") );
	moDefineParamIndex( PARTICLES_EMITTERVECTOR_X, moText("emittervectorx") );
	moDefineParamIndex( PARTICLES_EMITTERVECTOR_Y, moText("emittervectory") );
	moDefineParamIndex( PARTICLES_EMITTERVECTOR_Z, moText("emittervectorz") );

	moDefineParamIndex( PARTICLES_ATTRACTORTYPE, moText("attractortype") );
	moDefineParamIndex( PARTICLES_ATTRACTORMODE, moText("attractormode") );
	moDefineParamIndex( PARTICLES_ATTRACTORVECTOR_X, moText("attractorvectorx") );
	moDefineParamIndex( PARTICLES_ATTRACTORVECTOR_Y, moText("attractorvectory") );
	moDefineParamIndex( PARTICLES_ATTRACTORVECTOR_Z, moText("attractorvectorz") );


	moDefineParamIndex( PARTICLES_SCALEX_PARTICLE, moText("scalex_particle") );
	moDefineParamIndex( PARTICLES_SCALEY_PARTICLE, moText("scaley_particle") );
	moDefineParamIndex( PARTICLES_SCALEZ_PARTICLE, moText("scalez_particle") );
	moDefineParamIndex( PARTICLES_ROTATEX_PARTICLE, moText("rotatex_particle") );
	moDefineParamIndex( PARTICLES_ROTATEY_PARTICLE, moText("rotatey_particle") );
	moDefineParamIndex( PARTICLES_ROTATEZ_PARTICLE, moText("rotatez_particle") );

	moDefineParamIndex( PARTICLES_TIMETOREVELATION, moText("time_to_revelation") );
	moDefineParamIndex( PARTICLES_TIMEOFREVELATION, moText("time_of_revelation") );
	moDefineParamIndex( PARTICLES_TIMETORESTORATION, moText("time_to_restoration") );
	moDefineParamIndex( PARTICLES_TIMEOFRESTORATION, moText("time_of_restoration") );
	moDefineParamIndex( PARTICLES_DRAWINGFEATURES, moText("drawing_features") );

	moDefineParamIndex( PARTICLES_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( PARTICLES_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( PARTICLES_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( PARTICLES_SCALEX, moText("scalex") );
	moDefineParamIndex( PARTICLES_SCALEY, moText("scaley") );
	moDefineParamIndex( PARTICLES_SCALEZ, moText("scalez") );
	moDefineParamIndex( PARTICLES_ROTATEX, moText("rotatex") );
	moDefineParamIndex( PARTICLES_ROTATEY, moText("rotatey") );
	moDefineParamIndex( PARTICLES_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( PARTICLES_EYEX, moText("eyex") );
	moDefineParamIndex( PARTICLES_EYEY, moText("eyey") );
	moDefineParamIndex( PARTICLES_EYEZ, moText("eyez") );
	moDefineParamIndex( PARTICLES_VIEWX, moText("viewx") );
	moDefineParamIndex( PARTICLES_VIEWY, moText("viewy") );
	moDefineParamIndex( PARTICLES_VIEWZ, moText("viewz") );
	moDefineParamIndex( PARTICLES_INLET, moText("inlet") );
	moDefineParamIndex( PARTICLES_OUTLET, moText("outlet") );

    m_Physics.m_ParticleScript = moText("");

    InitScript();
    RegisterFunctions();

    m_Rate = 0;
    last_tick = 0;
    frame = 0;

    ortho = false;

    m_bTrackerInit = false;
    m_pTrackerData = NULL;

    UpdateParameters();

    ResetTimers();

    InitParticlesSimple(  m_Config.Int( moR(PARTICLES_WIDTH) ),
                          m_Config.Int( moR(PARTICLES_HEIGHT)) );


    pTextureDest = NULL;
    pSubSample = NULL;
    samplebuffer = NULL;

    glidori  = 0;
    glid = 0;
    original_width = 0;
    original_height = 0;
    original_proportion = 1.0;

    midi_red = midi_green = midi_blue = 1.0;
    midi_maxage = 1.0; //in millis
    midi_emitionperiod = 1.0;//in millisec
    midi_emitionrate = 1.0; // n per emitionperiod
    midi_randomvelocity = 1.0; //inicial vel
    midi_randommotion = 1.0; //motion dynamic

    m_InletTuioSystemIndex = GetInletIndex("TUIOSYSTEM");
    m_InletTrackerSystemIndex = GetInletIndex("TRACKERKLT");

	return true;

}


void moEffectParticlesSimple::ResetTimers() {

    TimerFullRevelation.Stop();
    TimerFullRestoration.Stop();
    TimerOfRevelation.Stop();
    TimerOfRestoration.Stop();

    FeatureActivity.Stop();
    MotionActivity.Stop();
    NoMotionActivity.Stop();
    m_Physics.EmitionTimer.Stop();

      for ( int i=0; i < m_ParticlesSimpleArray.Count(); i++ ) {
            moParticlesSimple* pPar = m_ParticlesSimpleArray[i];
            pPar->Age.Stop();
            pPar->Visible = false;
      }

  m_Physics.m_pLastBordParticle = NULL;
}

void moEffectParticlesSimple::Shot( moText source, int shot_type, int shot_file ) {

    MODebug2->Push(moText("moEffectParticlesSimple::Shot: Shot activated!!!!")+ (moText)source);

    moTextureBuffer* pTexBuf = NULL;

    pTextureDest = NULL;

    if (pSubSample && samplebuffer) {
        delete [] samplebuffer;
        pSubSample = NULL;
        samplebuffer = NULL;
    }

    glid = 0;
    glidori = 0;

    if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {



        ///primero toma la imagen que esta cargada: hay que generar una mas chica!!!!
        ///......

        if ( source != moText("")) {
            //agregado:
            //version banco de datos
            moTextureBuffer* pTexBufAltas = NULL;
            int ichosen;

            int idid = m_pResourceManager->GetTextureMan()->GetTextureBuffer(moText( source ),true);
            if (idid>-1) {
                pTexBufAltas = m_pResourceManager->GetTextureMan()->GetTextureBuffer(idid);
                pTexBufAltas->UpdateImages(1);
            }
            if (pTexBufAltas) {
                pTexBufAltas->UpdateImages(1);
                int np = pTexBufAltas->GetImagesProcessed();
                if (np>0) {
                    if (shot_type==0) {
                        /** Aleatorio*/
                        ichosen = ( ::rand() * np )/ RAND_MAX;
                    } else {
                        /** Secuencial (ultimo ingresado)*/
                        if (shot_file<0) {
                            ichosen = np - 1;
                        } else {
                            ichosen = shot_file;
                        }
                    }

                    glidori = pTexBufAltas->GetFrame(ichosen);
                    glid = glidori;
                    if (glidori>0) {
                        pTextureDest = (moTexture*) pTexBufAltas->GetTexture(ichosen);
                    }
                } else MODebug2->Error(  moText("no images processed yet"));
            }

        } else {
            glid = m_Config[moR(PARTICLES_TEXTURE)].GetData()->GetGLId();
            glidori = glid;
            //MODebug2->Error(  moText(" no texture buffer"));
            //version self texture (LIVEIN0 o imagen)
            if (glid>0) pTextureDest = (moTexture*)m_Config[moR(PARTICLES_TEXTURE)].GetData()->Pointer();
        }


        ///m_pTextureManager->GetBitmap( glid );
        if (pTextureDest) {


            ///levantar la textura!!! del thumbnail: y su buffer!!!
            time_t rawtime;
            char strbuffer[0x100];
            struct tm * timeinfo;

            srand(2);
            int randcode = rand();

            time(&rawtime);
            timeinfo = localtime ( &rawtime );
            strftime (strbuffer,80,"%Y-%m-%d-%H-%M-",timeinfo);


            moText datetime = strbuffer;
            datetime+= IntToStr(randcode);

            moText alta;
            moText baja;
            moText extension;

            original_width = pTextureDest->GetWidth();
            original_height = pTextureDest->GetHeight();
            if (original_height>0) {
                original_proportion = (float)original_width / (float) original_height;
            } else original_proportion = 1.0f;

            alta = (this->GetLabelName()) + moText("ALTA");
            baja = (this->GetLabelName()) + moText("BAJA");
            extension = moText(".tga");

            moText destalta;
            moText destbaja;
            moText copyalta;
            destalta = m_pResourceManager->GetDataMan()->GetDataPath() + (moText)alta;
            destbaja = m_pResourceManager->GetDataMan()->GetDataPath() + (moText)baja;
            copyalta = m_pResourceManager->GetDataMan()->GetDataPath() + moText("cams/") + (moText)datetime + (moText)alta;

            pTextureDest->CreateThumbnail( "TGA", pTextureDest->GetWidth(), pTextureDest->GetHeight(), destalta  );
            pTextureDest->CreateThumbnail( "TGA", m_cols, m_rows, destbaja  );

            if (source==moText("")) m_pResourceManager->GetFileMan()->CopyFile( destalta+(moText)extension , copyalta+(moText)extension );

            int idori = m_pResourceManager->GetTextureMan()->GetTextureMOId( alta+(moText)extension, false );
            if (idori>-1) {
                m_pResourceManager->GetTextureMan()->GetTexture(idori)->BuildFromFile( destalta+(moText)extension);
            } else idori = m_pResourceManager->GetTextureMan()->AddTexture( alta+(moText)extension );

            int idti = m_pResourceManager->GetTextureMan()->GetTextureMOId( baja+(moText)extension, false );
            if (idti>-1) {
                m_pResourceManager->GetTextureMan()->GetTexture(idti)->BuildFromFile( destbaja+(moText)extension );
            } else idti = m_pResourceManager->GetTextureMan()->AddTexture( baja+(moText)extension );

            if (idori>-1) glidori = m_pResourceManager->GetTextureMan()->GetTexture(idori)->GetGLId();

            //BAJA FOR SUBSAMPLING
            if (idti>-1) {
                    pSubSample = m_pResourceManager->GetTextureMan()->GetTexture(idti);
                    if (pSubSample) {
                        glid = pSubSample->GetGLId();
                        samplebuffer = new MObyte [ pSubSample->GetWidth() * pSubSample->GetHeight() * 3];

                        pSubSample->GetBuffer( samplebuffer, GL_RGB, GL_UNSIGNED_BYTE );

                    }
            }
        }
    }


}

void moEffectParticlesSimple::ReInit() {

    MODebug2->Push(moText("moEffectParticlesSimple::ReInit Face construction activated!!!!"));

    int i;
    int j;
    int lum = 0;
    int lumindex = 0;
    int contrast = 0;

    UpdateParameters();
    //ResetTimers();

    m_pResourceManager->GetTimeMan()->ClearByObjectId(  this->GetId() );


    //m_ParticlesSimpleArray.Init( p_cols*p_rows, NULL );
    //m_ParticlesSimpleArrayTmp.Init( p_cols*p_rows, NULL );

    for( i=0; i<m_cols ; i++) {
        for( j=0; j<m_rows ; j++) {
            moParticlesSimple* pPar = m_ParticlesSimpleArray[i+j*m_cols];

            if (pPar) {

                pPar->Pos = moVector2f( (float) i, (float) j);
                pPar->ImageProportion = 1.0;
                pPar->Color = moVector3f(1.0,1.0,1.0);
                pPar->GLId2 = 0;

                if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {

                    ///take the texture preselected
                    moTextureBuffer* pTexBuf = m_Config[moR(PARTICLES_FOLDERS)].GetData()->TextureBuffer();

                    pPar->GLId = glidori;
                    pPar->GLId2 = glid;
                    //pPar->GLId2 = 0;

                    pPar->TCoord2 = moVector2f( 0.0, 0.0 );
                    pPar->TSize2 = moVector2f( 1.0f, 1.0f );

                    pPar->TCoord = moVector2f( (float) (i ) / (float) m_cols, (float) (j) / (float) m_rows );
                    pPar->TSize = moVector2f( 1.0f / (float) m_cols, 1.0f / (float) m_rows );

                    ///calculamos la luminancia del cuadro correspondiente
                    //int x0, y0, x1, y1;
                    float lumf = 0.0;

                    if (pSubSample && samplebuffer) {

                        if (pSubSample->GetWidth()!=m_cols) MODebug2->Error(moText("pSubSample width doesnt match m_cols"));
                        if (pSubSample->GetHeight()!=m_rows) MODebug2->Error(moText("pSubSample height doesnt match m_rows"));

                        int r = samplebuffer[ (i + j*pSubSample->GetWidth() ) *3 ];
                        int g = samplebuffer[ (i+1 + j*pSubSample->GetWidth() ) *3 ];
                        int b = samplebuffer[ (i+2 + j*pSubSample->GetWidth() ) *3 ];
                        //MODebug2->Message(moText("r:")+IntToStr(r)+moText(" g:")+IntToStr(g)+moText(" b:")+IntToStr(b));

                        lum = (r+g+b)/3;

                        //lum = ( + samplebuffer[ (i+1 + j*pSubSample->GetWidth() ) *3 ] + samplebuffer[ (i+2 + j*pSubSample->GetWidth() ) *3 ] ) / 3;
                        lum = ((lum & 1) << 7) | ((lum & 2) << 5) | ((lum & 4) << 3) | ((lum & 8) << 1) | ((lum & 16) >> 1) | ((lum & 32) >> 3) | ((lum & 64) >> 5) | ((lum & 128) >> 7);
                        if (lum<0) lum = 0;
                        //MODebug2->Message(moText("lum:")+IntToStr(lum));

                        if (lum>=0) {
                            lum = lum*1.2;

                            lumf = ( 100.0 * (float)lum ) / (float)255.0;
                            lumindex = (int) lumf;
                            if (lumindex>99) lumindex = 99;
                            //MODebug2->Push( moText("## Lum Index R G B ##") +IntToStr(lum)+IntToStr(r)+IntToStr(g)+IntToStr(b));

                        } else {
                            MODebug2->Message(moText("ReInit error:## lum is negative!!! ##")+IntToStr(lum)
                                    +moText("subs: w:") + IntToStr(pSubSample->GetWidth())
                                    +moText("subs: h:") + IntToStr(pSubSample->GetHeight())
                            );
                            lumindex = 99;
                        }

                    } else MODebug2->Push(moText("ReInit error: no texture nor samplebuffer"));


                     if (pTexBuf && pTextureDest && samplebuffer) {

                         int nim = pTexBuf->GetImagesProcessed();

                         pPar->ImageProportion = 1.0;

                         if (nim>0) {

                             moTextureFrames& pTextFrames(pTexBuf->GetBufferLevels( lumindex, 0 ) );

                             int nc = pTextFrames.Count();
                             int irandom = -1;

                             irandom = ( ::rand() * nc )/ RAND_MAX;
                             //irandom = 0;

                            moTextureMemory* pTexMem = pTextFrames.Get( irandom );

                            if (pTexMem) {
                                pPar->GLId = glidori;
                                pTexMem->GetReference();
                                pPar->GLId2 = pTexMem->GetGLId();
                                pPar->pTextureMemory = pTexMem;
                                if (pTexMem->GetHeight()>0) pPar->ImageProportion = (float) pTexMem->GetWidth() / (float) pTexMem->GetHeight();
                            } else {
                                #ifdef _DEBUG
                                MODebug2->Message(moText("Sample not founded: lumindex:")+IntToStr(lumindex)+moText(" irandom:")+IntToStr(irandom));
                                #endif
                                pPar->GLId = glidori;
                                pPar->GLId2 = pPar->GLId;
                                pPar->Color.X() = ((float)lum )/ 255.0f;
                                pPar->Color.Y() = ((float)lum )/ 255.0f;
                                pPar->Color.Z() = ((float)lum )/ 255.0f;
                                pPar->Color.X()*= pPar->Color.X();
                                pPar->Color.Y()*= pPar->Color.Y();
                                pPar->Color.Z()*= pPar->Color.Z();
                            }

                            //MODebug2->Push( moText("creating particle: irandom:") + IntToStr(irandom) + moText(" nc:") + IntToStr(nc) + moText(" count:") + IntToStr(pTexBuf->GetImagesProcessed()) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2) );

                         }

                     } else {
                         MODebug2->Error( moText("particles error creating texture") );
                     }


                     //MODebug2->Log( moText("i:") + IntToStr(i) + moText(" J:") + IntToStr(j) + moText(" lum:") + IntToStr(lum) + moText(" lumindex:") + IntToStr(lumindex) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2));

                }


                pPar->Size = moVector2f( m_Physics.m_EmitterSize.X() / (float) m_cols, m_Physics.m_EmitterSize.Y() / (float) m_rows );
                pPar->Force = moVector3f( 0.0f, 0.0f, 0.0f );

                SetParticlePosition( pPar );

                if (m_Physics.m_EmitionPeriod>0) {
                    pPar->Age.Stop();
                    pPar->Visible = false;
                } else {
                    pPar->Age.Stop();
                    pPar->Age.Start();
                    pPar->Visible = true;
                }

                ///Set Timer management
                pPar->Age.SetObjectId( this->GetId() );
                pPar->Age.SetTimerId( i + j*m_cols );
                m_pResourceManager->GetTimeMan()->AddTimer( &pPar->Age );

                m_ParticlesSimpleArray.Set( i + j*m_cols, pPar );

                moParticlesSimple* pParTmp = m_ParticlesSimpleArrayTmp[i + j*m_cols];
                pParTmp->Pos3d = pPar->Pos3d;
                pParTmp->Velocity = pPar->Velocity;
                pParTmp->Mass = pPar->Mass;
                pParTmp->Force = pPar->Force;
                pParTmp->Fixed = pPar->Fixed;
                m_ParticlesSimpleArrayTmp.Set( i + j*m_cols, pParTmp );
/*
                MODebug2->Log( moText("Pos:") + IntToStr(i) + moText(" J:") + IntToStr(j) + moText(" lum:") + IntToStr(lum) + moText(" lumindex:") + IntToStr(lumindex) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2));
                MODebug2->Log( moText("X:") + FloatToStr(pPar->Pos3d.X()) + moText(" Y:") + FloatToStr(pPar->Pos3d.Y()) + moText(" Z:") + FloatToStr(pPar->Pos3d.Z()) );
                MODebug2->Log( moText("VX:") + FloatToStr(pPar->Velocity.X()) + moText(" VY:") + FloatToStr(pPar->Velocity.Y()) + moText(" VZ:") + FloatToStr(pPar->Velocity.Z()) );
                MODebug2->Log( moText("FX:") + FloatToStr(pPar->Force.X()) + moText(" FY:") + FloatToStr(pPar->Force.Y()) + moText(" FZ:") + FloatToStr(pPar->Force.Z()) );
                MODebug2->Log( moText("Size X:") + FloatToStr(pPar->Size.X()) + moText(" Size Y:") + FloatToStr(pPar->Size.Y()) );
                MODebug2->Log( moText("TCoord X:") + FloatToStr(pPar->TCoord.X()) + moText(" TCoord Y:") + FloatToStr(pPar->TCoord.Y()) );
                MODebug2->Log( moText("TCoord2 X:") + FloatToStr(pPar->TCoord2.X()) + moText(" TCoord2 Y:") + FloatToStr(pPar->TCoord2.Y()) );
                MODebug2->Log( moText("Visible:") + IntToStr((int)pPar->Visible) );
                MODebug2->Log( moText("Age:") + IntToStr((int)pPar->Age.Duration()) );
                */

            } else MODebug2->Error(moText("ParticleSimple::ReInit::no particle pointer"));
        }
    }

    //TrackParticle(1);

}

void moEffectParticlesSimple::UpdateParameters() {


    time_tofull_restoration = m_Config[moR(PARTICLES_TIMETORESTORATION)][MO_SELECTED][0].Int();
    time_of_restoration = m_Config[moR(PARTICLES_TIMEOFRESTORATION)][MO_SELECTED][0].Int();

    time_tofull_revelation = m_Config[moR(PARTICLES_TIMETOREVELATION)][MO_SELECTED][0].Int();
    time_of_revelation = m_Config[moR(PARTICLES_TIMEOFREVELATION)][MO_SELECTED][0].Int();

    ortho = (bool)m_Config[moR(PARTICLES_ORTHO)][MO_SELECTED][0].Int();

    if (!moTimeManager::MoldeoTimer->Started()) {
        ResetTimers();
        //MODebug2->Message("moEffectParticlesSimple::UpdateParameters  > ResetTimers!!!");
    }

    //if script is modified... recompile
	if ((moText)m_Physics.m_ParticleScript!=m_Config[moParamReference(PARTICLES_SCRIPT)][MO_SELECTED][0].Text()) {

        m_Physics.m_ParticleScript = m_Config[moParamReference(PARTICLES_SCRIPT)][MO_SELECTED][0].Text();
        moText fullscript = m_pResourceManager->GetDataMan()->GetDataPath()+ moSlash + (moText)m_Physics.m_ParticleScript;

        if ( CompileFile(fullscript) ) {

            MODebug2->Message(moText("ParticlesSimple script loaded ") + (moText)fullscript );

            SelectScriptFunction( "Init" );
            //AddFunctionParam( m_FramesPerSecond );
            RunSelectedFunction();

        } else MODebug2->Error(moText("ParticlesSimple couldnt compile lua script ") + (moText)fullscript );
	}

    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("RunSystem")) {
            SelectScriptFunction("RunSystem");
            //passing number of particles
            AddFunctionParam( (int) ( m_rows*m_cols ) );
            RunSelectedFunction(1);
        }
    }

    drawing_features = m_Config[moR(PARTICLES_DRAWINGFEATURES)][MO_SELECTED][0].Int();
    texture_mode = m_Config[moR(PARTICLES_TEXTUREMODE)][MO_SELECTED][0].Int();

    m_Physics.m_EyeVector = moVector3f(m_Config[moR(PARTICLES_EYEX)].GetData()->Fun()->Eval(state.tempo.ang),
                                        m_Config[moR(PARTICLES_EYEY)].GetData()->Fun()->Eval(state.tempo.ang),
                                        m_Config[moR(PARTICLES_EYEZ)].GetData()->Fun()->Eval(state.tempo.ang));


    m_Physics.gravitational = m_Config[moR(PARTICLES_GRAVITY)].GetData()->Fun()->Eval(state.tempo.ang);
    m_Physics.viscousdrag = m_Config[moR(PARTICLES_VISCOSITY)].GetData()->Fun()->Eval(state.tempo.ang);


    /*
    m_Physics.m_MaxAge = (int)((float)(m_Config[moR(PARTICLES_MAXAGE)][MO_SELECTED][0].Int()) * midi_maxage);
    m_Physics.m_EmitionPeriod = (int)((float)(m_Config[moR(PARTICLES_EMITIONPERIOD)][MO_SELECTED][0].Int()) * midi_emitionperiod);
    m_Physics.m_EmitionRate = (int)((float)(m_Config[moR(PARTICLES_EMITIONRATE)][MO_SELECTED][0].Int()) * midi_emitionrate);
    m_Physics.m_DeathPeriod = m_Config[moR(PARTICLES_DEATHPERIOD)][MO_SELECTED][0].Float();
*/

    //emiper = (float)m_Config[moR(PARTICLES_EMITIONPERIOD)][MO_SELECTED][0].Int();
    //emiper = emiper * midi_emitionperiod;
    //emiperi = (long) emiper;
    //MODebug2->Message(moText("Emiper:")+IntToStr(emiperi));

    m_Physics.m_MaxAge = m_Config[moR(PARTICLES_MAXAGE)][MO_SELECTED][0].Int();
    //m_Physics.m_EmitionPeriod = emiperi;
    //emiperi = m_Config[moR(PARTICLES_EMITIONPERIOD)][MO_SELECTED][0].Int() * midi_emitionperiod;
    //m_Physics.m_EmitionPeriod = emiperi;
    m_Physics.m_EmitionPeriod = m_Config[moR(PARTICLES_EMITIONPERIOD)][MO_SELECTED][0].Int();
    //m_Physics.m_EmitionPeriod = m_Config[moR(PARTICLES_EMITIONPERIOD)][MO_SELECTED][0].Int();
    //MODebug2->Message(moText("Emiperiod:")+IntToStr(m_Physics.m_EmitionPeriod));
    m_Physics.m_EmitionRate = m_Config[moR(PARTICLES_EMITIONRATE)][MO_SELECTED][0].Int();
    m_Physics.m_DeathPeriod = m_Config[moR(PARTICLES_DEATHPERIOD)][MO_SELECTED][0].Float();


    m_Physics.m_RandomMethod = (moParticlesRandomMethod) m_Config[moR(PARTICLES_RANDOMMETHOD)][MO_SELECTED][0].Int();
    m_Physics.m_CreationMethod = (moParticlesCreationMethod) m_Config[moR(PARTICLES_CREATIONMETHOD)][MO_SELECTED][0].Int();

    m_Physics.m_OrientationMode = (moParticlesOrientationMode) m_Config[moR(PARTICLES_ORIENTATIONMODE)][MO_SELECTED][0].Int();

    m_Physics.m_FadeIn = m_Config[moR(PARTICLES_FADEIN)].GetData()->Fun()->Eval(state.tempo.ang);
    m_Physics.m_FadeOut = m_Config[moR(PARTICLES_FADEOUT)].GetData()->Fun()->Eval(state.tempo.ang);
    m_Physics.m_SizeIn = m_Config[moR(PARTICLES_SIZEIN)].GetData()->Fun()->Eval(state.tempo.ang);
    m_Physics.m_SizeOut = m_Config[moR(PARTICLES_SIZEOUT)].GetData()->Fun()->Eval(state.tempo.ang);


    /*
    m_Physics.m_RandomPosition = m_Config[moR(PARTICLES_RANDOMPOSITION)].GetData()->Fun()->Eval(state.tempo.ang);
    m_Physics.m_RandomVelocity = m_Config[moR(PARTICLES_RANDOMVELOCITY)].GetData()->Fun()->Eval(state.tempo.ang) * midi_randomvelocity;
    m_Physics.m_RandomMotion = m_Config[moR(PARTICLES_RANDOMMOTION)].GetData()->Fun()->Eval(state.tempo.ang) * midi_randommotion;
    */
    m_Physics.m_RandomPosition = m_Config[moR(PARTICLES_RANDOMPOSITION)].GetData()->Fun()->Eval(state.tempo.ang);
    m_Physics.m_RandomVelocity = m_Config[moR(PARTICLES_RANDOMVELOCITY)].GetData()->Fun()->Eval(state.tempo.ang);
    m_Physics.m_RandomMotion = m_Config[moR(PARTICLES_RANDOMMOTION)].GetData()->Fun()->Eval(state.tempo.ang);


    m_Physics.m_EmitterType = (moParticlesSimpleEmitterType) m_Config[moR(PARTICLES_EMITTERTYPE)][MO_SELECTED][0].Int();
    m_Physics.m_AttractorType = (moParticlesSimpleAttractorType) m_Config[moR(PARTICLES_ATTRACTORTYPE)][MO_SELECTED][0].Int();

    m_Physics.m_PositionVector = moVector3f(m_Config[moR(PARTICLES_RANDOMPOSITION_X)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_RANDOMPOSITION_Y)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_RANDOMPOSITION_Z)].GetData()->Fun()->Eval(state.tempo.ang));

    m_Physics.m_EmitterSize = moVector3f( m_Config[moR(PARTICLES_SIZEX)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_SIZEY)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_SIZEZ)].GetData()->Fun()->Eval(state.tempo.ang));

    m_Physics.m_VelocityVector =  moVector3f( m_Config[moR(PARTICLES_RANDOMVELOCITY_X)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_RANDOMVELOCITY_Y)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_RANDOMVELOCITY_Z)].GetData()->Fun()->Eval(state.tempo.ang));

    m_Physics.m_MotionVector =  moVector3f( m_Config[moR(PARTICLES_RANDOMMOTION_X)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_RANDOMMOTION_Y)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_RANDOMMOTION_Z)].GetData()->Fun()->Eval(state.tempo.ang));

    m_Physics.m_EmitterVector = moVector3f( m_Config[moR(PARTICLES_EMITTERVECTOR_X)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_EMITTERVECTOR_Y)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_EMITTERVECTOR_Z)].GetData()->Fun()->Eval(state.tempo.ang));

    if (m_bTrackerInit && m_Physics.m_EmitterType==PARTICLES_EMITTERTYPE_TRACKER2) {
        m_Physics.m_EmitterVector = moVector3f( m_TrackerBarycenter.X()*normalf, m_TrackerBarycenter.Y()*normalf, 0.0f );
    }

    m_Physics.m_AttractorMode = (moParticlesSimpleAttractorMode) m_Config[moR(PARTICLES_ATTRACTORMODE)][MO_SELECTED][0].Int();
    m_Physics.m_AttractorVector = moVector3f( m_Config[moR(PARTICLES_ATTRACTORVECTOR_X)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_ATTRACTORVECTOR_Y)].GetData()->Fun()->Eval(state.tempo.ang),
                                            m_Config[moR(PARTICLES_ATTRACTORVECTOR_Z)].GetData()->Fun()->Eval(state.tempo.ang));

    if (original_proportion!=1.0f) {
            if (original_proportion>1.0f) {
                m_Physics.m_AttractorVector.Y() = m_Physics.m_AttractorVector.Y() / original_proportion;
            } else {
                m_Physics.m_AttractorVector.X() = m_Physics.m_AttractorVector.X() * original_proportion;
            }
    }

    normalf = m_Physics.m_EmitterSize.X();
}

void moEffectParticlesSimple::SetParticlePosition( moParticlesSimple* pParticle ) {

    float left =  - (m_Physics.m_EmitterSize.X()) / 2.0;
    float top =  m_Physics.m_EmitterSize.Y() / 2.0;
    float randomvelx = 0;
    float randomvely = 0;
    float randomvelz = 0;
    float randomposx = 0;
    float randomposy = 0;
    float randomposz = 0;
    double alpha;
    double phi;
    double radius;
    double z;
    double radius1;
    double radius2;

    randomposx = (m_Physics.m_RandomPosition>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomPosition*m_Physics.m_PositionVector.X() : m_Physics.m_PositionVector.X();
    randomposy = (m_Physics.m_RandomPosition>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomPosition*m_Physics.m_PositionVector.Y() : m_Physics.m_PositionVector.Y();
    randomposz = (m_Physics.m_RandomPosition>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomPosition*m_Physics.m_PositionVector.Z() : m_Physics.m_PositionVector.Z();

    randomvelx = (m_Physics.m_RandomVelocity>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomVelocity*m_Physics.m_VelocityVector.X() : m_Physics.m_VelocityVector.X();
    randomvely = (m_Physics.m_RandomVelocity>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomVelocity*m_Physics.m_VelocityVector.Y() : m_Physics.m_VelocityVector.Y();
    randomvelz = (m_Physics.m_RandomVelocity>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomVelocity*m_Physics.m_VelocityVector.Z() : m_Physics.m_VelocityVector.Z();

    moVector4d fullcolor;
    fullcolor = m_Config.EvalColor( moR(PARTICLES_PARTICLECOLOR) , state.tempo.ang);
    pParticle->Color = moVector3f(
                              fullcolor.X(),
                              fullcolor.Y(),
                              fullcolor.Z() );

    pParticle->Mass = 10.0f;
    pParticle->Fixed = false;

    pParticle->U = moVector3f( 1.0, 0.0, 0.0 );
    pParticle->V = moVector3f( 0.0, 1.0, 0.0 );
    pParticle->W = moVector3f( 0.0, 0.0, 1.0 );

    pParticle->dpdt = moVector3f( 0.0f, 0.0f, 0.0f );
    pParticle->dvdt = moVector3f( 0.0f, 0.0f, 0.0f );

    if (m_Physics.m_FadeIn>0.0) pParticle->Alpha = 0.0;///fade in ? to middle age?
    else pParticle->Alpha = fullcolor.W();

    if (m_Physics.m_SizeIn>0.0) pParticle->Scale = 0.0;///fade in ? to middle age?
    else pParticle->Scale = 1.0;

    switch(m_Physics.m_EmitterType) {

        case PARTICLES_EMITTERTYPE_GRID:
            //GRID POSITION
           switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                    pParticle->Pos3d = moVector3f(   ( left + pParticle->Pos.X()*pParticle->Size.X() + pParticle->Size.X()*randomposx/2.0 )*m_Physics.m_EmitterVector.X() ,
                                                     ( top - pParticle->Pos.Y()*pParticle->Size.Y() - pParticle->Size.Y()*randomposy/2.0 )*m_Physics.m_EmitterVector.Y(),
                                                    randomposz*m_Physics.m_EmitterVector.Z() );
                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case PARTICLES_CREATIONMETHOD_PLANAR:
                case PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    pParticle->Pos3d = moVector3f(   ( left + moMathf::UnitRandom()*m_Physics.m_EmitterSize.X() + pParticle->Size.X()*randomposx/2.0 )*m_Physics.m_EmitterVector.X() ,
                                                     ( top - moMathf::UnitRandom()*m_Physics.m_EmitterSize.Y() - pParticle->Size.Y()*randomposy/2.0 )*m_Physics.m_EmitterVector.Y(),
                                                    randomposz*m_Physics.m_EmitterVector.Z() );
                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }

            break;

        case PARTICLES_EMITTERTYPE_SPHERE:
            //SPHERE POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                    alpha = 2 * moMathf::PI * pParticle->Pos.X() / (double)m_cols;
                    phi = moMathf::PI * pParticle->Pos.Y() / (double)m_rows;
                    radius = moMathf::Sqrt( m_Physics.m_EmitterSize.X()*m_Physics.m_EmitterSize.X()+m_Physics.m_EmitterSize.Y()*m_Physics.m_EmitterSize.Y()) / 2.0;

                    pParticle->Pos3d = moVector3f(  (radius*moMathf::Cos(alpha)*moMathf::Sin(phi) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    (radius*moMathf::Sin(alpha)*moMathf::Sin(phi) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    (radius*moMathf::Cos(phi) + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case  PARTICLES_CREATIONMETHOD_PLANAR:
                    alpha = 2 * (moMathf::PI) * moMathf::UnitRandom();
                    phi = moMathf::PI * moMathf::UnitRandom();
                    radius = moMathf::Sqrt( m_Physics.m_EmitterSize.X()*m_Physics.m_EmitterSize.X()+m_Physics.m_EmitterSize.Y()*m_Physics.m_EmitterSize.Y()) / 2.0;
                    pParticle->Pos3d = moVector3f(
                                        (radius*moMathf::Cos(alpha)*moMathf::Sin(phi) + randomposx)* m_Physics.m_EmitterVector.X(),
                                        (radius*moMathf::Sin(alpha)*moMathf::Sin(phi) + randomposy)* m_Physics.m_EmitterVector.Y(),
                                        (radius*moMathf::Cos(phi) + randomposz)* m_Physics.m_EmitterVector.Z()
                                        );
                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case  PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    alpha = 2 * moMathf::PI * moMathf::UnitRandom();
                    phi = moMathf::PI * moMathf::UnitRandom();
                    radius = moMathf::Sqrt( m_Physics.m_EmitterSize.X()*m_Physics.m_EmitterSize.X()+m_Physics.m_EmitterSize.Y()*m_Physics.m_EmitterSize.Y())*moMathf::UnitRandom() / 2.0;

                    pParticle->Pos3d = moVector3f(  (radius*moMathf::Cos(alpha)*moMathf::Sin(phi) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    (radius*moMathf::Sin(alpha)*moMathf::Sin(phi) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    (radius*moMathf::Cos(phi) + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }
            break;

        case PARTICLES_EMITTERTYPE_TUBE:
            //SPHERE POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                    alpha = 2 * moMathf::PI * pParticle->Pos.X() / (double)m_cols;
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    z = m_Physics.m_EmitterSize.Z() * ( 0.5f - ( pParticle->Pos.Y() / (double)m_rows ) );

                    pParticle->Pos3d = moVector3f(  ( radius1*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius1*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case  PARTICLES_CREATIONMETHOD_PLANAR:
                    alpha = 2 * moMathf::PI * moMathf::UnitRandom();
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    z = m_Physics.m_EmitterSize.Z() * ( 0.5f - moMathf::UnitRandom());

                    pParticle->Pos3d = moVector3f(  ( radius1*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius1*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case  PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    alpha = 2 * moMathf::PI * moMathf::UnitRandom();
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    radius = radius1 + moMathf::UnitRandom()*(radius2-radius1)*moMathf::UnitRandom();
                    z = m_Physics.m_EmitterSize.Z() * ( 0.5f - moMathf::UnitRandom());

                    pParticle->Pos3d = moVector3f(  ( radius*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }
            break;

        case PARTICLES_EMITTERTYPE_JET:
            //SPHERE POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                case PARTICLES_CREATIONMETHOD_PLANAR:
                case PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    z = m_Physics.m_EmitterSize.Z() * moMathf::UnitRandom();

                    pParticle->Pos3d = moVector3f(  m_Physics.m_EmitterVector.X()*( z + randomposx ),
                                                    m_Physics.m_EmitterVector.Y()*( z + randomposy ),
                                                    m_Physics.m_EmitterVector.Z()*( z + randomposz) );

                    pParticle->Velocity = moVector3f(   randomvelx,
                                                        randomvely,
                                                        randomvelz);
                    break;

            }
            break;

        case PARTICLES_EMITTERTYPE_POINT:
            //SPHERE POSITION
            pParticle->Pos3d = moVector3f(  randomposx+m_Physics.m_EmitterVector.X(),
                                            randomposy+m_Physics.m_EmitterVector.Y(),
                                            randomposz+m_Physics.m_EmitterVector.Z() );

            pParticle->Velocity = moVector3f(   randomvelx,
                                                randomvely,
                                                randomvelz);

            break;
        case PARTICLES_EMITTERTYPE_SPIRAL:
            //SPIRAL POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                case  PARTICLES_CREATIONMETHOD_PLANAR:
                case  PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    alpha = 2 * moMathf::PI * pParticle->Pos.X() / (double)m_cols;
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    z = m_Physics.m_EmitterSize.Z() * ( 0.5f - ( pParticle->Pos.Y() / (double)m_rows ) - (pParticle->Pos.X() / (double)(m_cols*m_rows)) );

                    pParticle->Pos3d = moVector3f(  ( radius1*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius1*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }
            break;
        case PARTICLES_EMITTERTYPE_CIRCLE:
            //CIRCLE POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                case  PARTICLES_CREATIONMETHOD_PLANAR:
                case  PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    alpha = 2 * moMathf::PI *  ( pParticle->Pos.X()*m_rows + pParticle->Pos.Y()) / ((double)m_cols*(double)m_rows );
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    z = 0.0;
                    //z = m_Physics.m_EmitterSize.Z() * ( 0.5f - ( pParticle->Pos.Y() / (double)m_rows ) - (pParticle->Pos.X() / (double)(m_cols*m_rows)) );

                    pParticle->Pos3d = moVector3f(  ( radius1*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius1*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }
            break;

        case PARTICLES_EMITTERTYPE_TRACKER:
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_CENTER:
                    if (m_pTrackerData) {
                        pParticle->Pos3d = moVector3f( (m_pTrackerData->GetBarycenter().X() - 0.5)*normalf, (-m_pTrackerData->GetBarycenter().Y()+0.5)*normalf, 0.0 );
                        pParticle->Pos3d+= moVector3f( randomposx, randomposy, randomposz );
                        pParticle->Velocity = moVector3f( randomvelx, randomvely, randomvelz );
                    }
                    break;

                case PARTICLES_CREATIONMETHOD_LINEAR:
                case PARTICLES_CREATIONMETHOD_PLANAR:
                case PARTICLES_CREATIONMETHOD_VOLUMETRIC:



                    if (m_pTrackerData) {
                        bool bfounded = false;
                        int np =  (int) ( moMathf::UnitRandom() * m_pTrackerData->GetFeaturesCount() );

                        moTrackerFeature *pTF = NULL;

                        pTF = m_pTrackerData->GetFeature( np );
                        if (pTF->valid) {
                            pParticle->Pos3d = moVector3f( (pTF->x - 0.5)*normalf, (-pTF->y+0.5)*normalf, 0.0 );
                            bfounded = true;
                        }

                        np = 0;
                        //como no encontro un feature valido para usar de generador arranca desde el primero....
                        //error, deberia tomar el baricentro.... o tomar al azar otro...
                        /*
                        if (!bfounded) {
                            pParticle->Pos3d = moVector3f( (m_pTrackerData->GetBarycenter().X() - 0.5)*normalf, (-m_pTrackerData->GetBarycenter().Y()+0.5)*normalf, 0.0 );
                        }*/
                        int cn=0;
                        if (!bfounded) {
                            do {
                                pTF = m_pTrackerData->GetFeature( np );
                                if (pTF->valid) {
                                    pParticle->Pos3d = moVector3f( (pTF->x - 0.5)*normalf, (-pTF->y+0.5)*normalf, 0.0 );
                                    bfounded = true;
                                }
                                np =  (int) ( moMathf::UnitRandom() * m_pTrackerData->GetFeaturesCount() );
                                cn++;
                            } while (!pTF->valid && np < m_pTrackerData->GetFeaturesCount() && cn<5 );
                            if (!bfounded) pParticle->Pos3d = moVector3f( (m_pTrackerData->GetBarycenter().X() - 0.5)*normalf, (-m_pTrackerData->GetBarycenter().Y()+0.5)*normalf, 0.0 );
                        }


                    } else {
                        pParticle->Pos3d = moVector3f( 0, 0, 0 );
                    }

                    pParticle->Pos3d+= moVector3f( randomposx, randomposy, randomposz );

                    pParticle->Velocity = moVector3f(   randomvelx,
                                                        randomvely,
                                                        randomvelz);
                    break;

            }
            break;
    };


}

void moEffectParticlesSimple::InitParticlesSimple( int p_cols, int p_rows, bool p_forced ) {

    int i,j;

    bool m_bNewImage = false;


    if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {
        Shot();
    }

    m_pResourceManager->GetTimeMan()->ClearByObjectId(  this->GetId() );

    //if (p_cols==m_cols && p_rows==m_rows && !forced)  {

        if (m_ParticlesSimpleArray.Count()>0) {
            /*for(i=0;i<m_ParticlesSimpleArray.Count();i++) {
                if (m_ParticlesSimpleArray[i]!=NULL) {
                    delete m_ParticlesSimpleArray[i];
                    //m_ParticlesSimple.Set(i, NULL);
                }
            }
            */
            m_ParticlesSimpleArray.Empty();
        }

        if (m_ParticlesSimpleArrayTmp.Count()>0) {
            /*for(i=0;i<m_ParticlesSimpleArrayTmp.Count();i++) {
                if (m_ParticlesSimpleArrayTmp[i]!=NULL) {
                    delete m_ParticlesSimpleArrayTmp[i];
                    //m_ParticlesSimple.Set(i, NULL);
                }
            }
            */
            m_ParticlesSimpleArrayTmp.Empty();
        }
    //}

    m_ParticlesSimpleArray.Init( p_cols*p_rows, NULL );
    m_ParticlesSimpleArrayTmp.Init( p_cols*p_rows, NULL );


        for( j=0; j<p_rows ; j++) {
                for( i=0; i<p_cols ; i++) {

            moParticlesSimple* pPar = new moParticlesSimple();

            pPar->Pos = moVector2f( (float) i, (float) j);
            pPar->ImageProportion = 1.0;
            //pPar->Color = moVector3f(1.0,1.0,1.0);
            moVector4d fullcolor;
            fullcolor = m_Config.EvalColor( moR(PARTICLES_PARTICLECOLOR) , state.tempo.ang);
            pPar->Color = moVector3f(
                                      fullcolor.X(),
                                      fullcolor.Y(),
                                      fullcolor.Z() );
            pPar->GLId2 = 0;

            if (texture_mode==PARTICLES_TEXTUREMODE_UNIT) {

                pPar->TCoord = moVector2f( 0.0, 0.0 );
                pPar->TSize = moVector2f( 1.0f, 1.0f );

            } else if (texture_mode==PARTICLES_TEXTUREMODE_PATCH) {

                pPar->TCoord = moVector2f( (float) (i) / (float) p_cols, (float) (j) / (float) p_rows );
                pPar->TSize = moVector2f( 1.0f / (float) p_cols, 1.0f / (float) p_rows );

            } else if (texture_mode==PARTICLES_TEXTUREMODE_MANY ) {
                pPar->TCoord = moVector2f( 0.0, 0.0 );
                pPar->TSize = moVector2f( 1.0f, 1.0f );
            } else if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {

                ///take the texture preselected
                moTextureBuffer* pTexBuf = m_Config[moR(PARTICLES_FOLDERS)].GetData()->TextureBuffer();

                pPar->GLId = glidori;
                pPar->GLId2 = glid;

                pPar->TCoord2 = moVector2f( 0.0, 0.0 );
                pPar->TSize2 = moVector2f( 1.0f, 1.0f );

                pPar->TCoord = moVector2f( (float) (i ) / (float) p_cols, (float) (j) / (float) p_rows );
                pPar->TSize = moVector2f( 1.0f / (float) p_cols, 1.0f / (float) p_rows );

                ///calculamos la luminancia del cuadro correspondiente
                //int x0, y0, x1, y1;
                int lum = 0;
                int lumindex = 0;
                int contrast = 0;

                if (pSubSample && samplebuffer) {

                    lum = (samplebuffer[ (i + j*pSubSample->GetWidth() ) *3 ] + samplebuffer[ (i+1 + j*pSubSample->GetWidth() ) *3 ] + samplebuffer[ (i+2 + j*pSubSample->GetWidth() ) *3 ]) / 3;
                    if (lum<0) lum = -lum;
                    if (lum>=0) {
                        lumindex = (int)( 100.0 * (float)lum / (float)256)  - 1;
                        if (lumindex>99) lumindex = 99;
                    }
                } else {
                    MODebug2->Message(moText("pSubSample: ")+IntToStr((long)pSubSample) +
                                    moText("samplebuffer: ")+IntToStr((long)samplebuffer));
                }

                 if (pTexBuf) {

                     int nim = pTexBuf->GetImagesProcessed();

                     pPar->ImageProportion = 1.0;


                     if (nim>0) {

                         moTextureFrames& pTextFrames(pTexBuf->GetBufferLevels( lumindex, 0 ) );

                         int nc = pTextFrames.Count();
                         int irandom = -1;

                         irandom = ( ::rand() * nc )/ RAND_MAX;

                        moTextureMemory* pTexMem = pTextFrames.Get( irandom );

                        if (pTexMem) {
                            pPar->GLId = glidori;
                            pTexMem->GetReference();
                            pPar->GLId2 = pTexMem->GetGLId();
                            pPar->pTextureMemory = pTexMem;
                            if (pTexMem->GetHeight()>0) pPar->ImageProportion = (float) pTexMem->GetWidth() / (float) pTexMem->GetHeight();
                        } else {
                            pPar->GLId = glidori;
                            pPar->GLId2 = pPar->GLId;
                            pPar->Color.X() = ((float)lum )/ 255.0f;
                            pPar->Color.Y() = ((float)lum )/ 255.0f;
                            pPar->Color.Z() = ((float)lum )/ 255.0f;
                            pPar->Color.X()*= pPar->Color.X();
                            pPar->Color.Y()*= pPar->Color.Y();
                            pPar->Color.Z()*= pPar->Color.Z();
                        }
                         //MODebug2->Push( moText("creating particle: irandom:") + IntToStr(irandom) + moText(" count:") + IntToStr(pTexBuf->GetImagesProcessed()) + moText(" glid:") + IntToStr(pPar->GLId) );

                     }

                 } else MODebug2->Error( moText("particles error creating texture") );

            }

            pPar->Size = moVector2f( m_Physics.m_EmitterSize.X() / (float) p_cols, m_Physics.m_EmitterSize.Y() / (float) p_rows );
            pPar->Force = moVector3f( 0.0f, 0.0f, 0.0f );

            SetParticlePosition( pPar );

            /*
            MODebug2->Message("i:"+IntToStr(i) );
            MODebug2->Message("j:"+IntToStr(j) );
            MODebug2->Message("px:"+FloatToStr(pPar->Pos3d.X()) );
            MODebug2->Message("py:"+FloatToStr(pPar->Pos3d.Y()) );
            */

            if (m_Physics.m_EmitionPeriod>0) {
                pPar->Age.Stop();
                pPar->Visible = false;
            } else {
                pPar->Age.Start();
                pPar->Visible = true;
            }

            ///Set Timer management
            pPar->Age.SetObjectId( this->GetId() );
            pPar->Age.SetTimerId( i + j*p_cols );
            m_pResourceManager->GetTimeMan()->AddTimer( &pPar->Age );

            m_ParticlesSimpleArray.Set( i + j*p_cols, pPar );

            moParticlesSimple* pParTmp = new moParticlesSimple();
            pParTmp->Pos3d = pPar->Pos3d;
            pParTmp->Velocity = pPar->Velocity;
            pParTmp->Mass = pPar->Mass;
            pParTmp->Force = pPar->Force;
            pParTmp->Fixed = pPar->Fixed;
            m_ParticlesSimpleArrayTmp.Set( i + j*p_cols, pParTmp );

        }
    }

    m_rows = p_rows;
    m_cols = p_cols;

}

void moEffectParticlesSimple::Regenerate() {

    int i,j;
    float randommotionx,randommotiony,randommotionz;

    //long emitiontimer_duration = m_Physics.EmitionTimer.Duration();
    //MODebug2->Message("dur:"+IntToStr(emitiontimer_duration));

    for( j=0; j<m_rows ; j++) {
      for( i=0; i<m_cols ; i++) {


            moParticlesSimple* pPar = m_ParticlesSimpleArray[i+j*m_cols];

            pPar->pTextureMemory = NULL;

            if (pPar->Age.Duration() > moGetTicks() ) {
                pPar->Age.Stop();
                pPar->Visible = false;
                if (pPar->pTextureMemory) {
                    pPar->pTextureMemory->ReleaseReference();
                    pPar->pTextureMemory = NULL;
                    pPar->GLId = 0;
                }
            }


            //KILL PARTICLE
            //if (i==3 && j==3)
              //MODebug2->Message("visible: " + IntToStr((int)pPar->Visible) + "on:" + IntToStr((int)pPar->Age.Started()) + " agedur:" + IntToStr(pPar->Age.Duration()));
            if ( pPar->Visible &&
                (m_Physics.m_MaxAge>0) &&
                (pPar->Age.Duration() > m_Physics.m_MaxAge) ) {

                pPar->Age.Stop();
                pPar->Visible = false;
                if (m_Rate>0) m_Rate--;
                if (pPar->pTextureMemory) {
                    pPar->pTextureMemory->ReleaseReference();
                    pPar->pTextureMemory = NULL;
                    pPar->GLId = 0;
                }

            }

            //REBORN PARTICLE
            //m_Physics.EmitionTimer.Duration()
            //MODebug2->Message("dur:"+IntToStr(emitiontimer_duration)+" vs:"+IntToStr(m_Physics.m_EmitionPeriod) );
            if ( m_Rate<m_Physics.m_EmitionRate &&
                (m_Physics.EmitionTimer.Duration() > m_Physics.m_EmitionPeriod)
                && pPar->Visible==false ) {

                bool letsborn = true;
                int id_last_particle = 0;
                int this_id_particle = 0;

                if (m_Physics.m_CreationMethod==PARTICLES_CREATIONMETHOD_LINEAR ) {
                  if (m_Physics.m_pLastBordParticle!=NULL)
                    id_last_particle = m_Physics.m_pLastBordParticle->Pos.X() + m_Physics.m_pLastBordParticle->Pos.Y()*m_cols;
                  else id_last_particle = -1;

                  this_id_particle = i+j*m_cols;

                  if (id_last_particle==(m_rows*m_cols -1) && this_id_particle == 0 ) {
                      ///OK
                      letsborn = true;
                  } else if ( this_id_particle == (id_last_particle+1) ) {
                      ///OK
                      letsborn = true;
                  } else {
                    ///wait for the other cycle
                    letsborn = false;
                  }

                }

                //m_Physics.EmitionTimer.Start();
                if (letsborn) {
                  pPar->Visible = true;
                  pPar->Age.Start();

                  //guardamos la referencia a esta particula, que servira para la proxima
                  m_Physics.m_pLastBordParticle = pPar;

                  m_Rate++;

                  SetParticlePosition( pPar );
                  /*
                  MODebug2->Message("frame:"+IntToStr(frame) );
                  MODebug2->Message("i:"+IntToStr(i) );
                  MODebug2->Message("j:"+IntToStr(j) );
                  MODebug2->Message("px:"+FloatToStr(pPar->Pos3d.X()) );
                  MODebug2->Message("py:"+FloatToStr(pPar->Pos3d.Y()) );
                  */

                  /*
                  if ((i+j*m_cols)%1000 == 0) {
                      MODebug2->Push(moText("partícula en proceso - regenerando GLID ")+IntToStr(pPar->GLId)
                      + moText(" GLID2: ") + IntToStr(pPar->GLId2) );
                  }*/

                  //pPar->Pos3d = moVector3f( 0, 0, 0);

                  //regenerate

                   //moTexture* pTex = ; //m_pResourceManager->GetTextureMan()->GetTexture();
                   ///asigna un id al azar!!!! de todos los que componen ell moTextureBuffer
                   ///hay q pedir el moTextureBuffer
                   if ( texture_mode==PARTICLES_TEXTUREMODE_MANY ) {
                       moTextureBuffer* pTexBuf = m_Config[moR(PARTICLES_FOLDERS)].GetData()->TextureBuffer();
                       //m_Config[moR(PARTICLES_TEXTURE)].GetData()->GetGLId(&state.tempo, 1, NULL );
                       if (pTexBuf) {
                           int nim = pTexBuf->GetImagesProcessed();

                           pPar->ImageProportion = 1.0;

                           if (nim>0) {

                               //float frandom = moMathf::UnitRandom( (float) 2.0 ) * nim;
                               //srand(2);
                               int irandom = ( ::rand() * nim )/ RAND_MAX;

                               if (irandom>=nim) irandom = nim - 1;

                               pPar->GLId = pTexBuf->GetFrame( irandom );

                               moTextureMemory* pTexMem = pTexBuf->GetTexture( irandom );
                               if (pTexMem) {
                                  pPar->pTextureMemory = pTexMem;
                                  if (pTexMem->GetHeight()>0) pPar->ImageProportion = (float) pTexMem->GetWidth() / (float) pTexMem->GetHeight();
                                }

                               ///MODebug2->Push( moText("creating particle: irandom:") + IntToStr(irandom) + moText(" count:") + IntToStr(pTexBuf->GetImagesProcessed()) + moText(" glid:") + IntToStr(pPar->GLId) );


                           } else {
                               ///pPar->GLId = 0;
                           }
                           pPar->TCoord = moVector2f( 0.0, 0.0 );
                           pPar->TSize = moVector2f( 1.0f, 1.0f );

                       } else MODebug2->Error( moText("PARTICLES_TEXTUREMODE_MANY particles error creating texture") );
                   }

                } ///fin letsborn

            }


            if (m_Rate>=m_Physics.m_EmitionRate) {
                m_Physics.EmitionTimer.Start();
                m_Rate = 0;
            }

            //FADEIN
            if ( pPar->Visible && m_Physics.m_FadeIn>0.0 && (m_Physics.m_MaxAge>0) &&  (pPar->Age.Duration() < m_Physics.m_MaxAge) ) {

                if ( pPar->Age.Duration() < ( m_Physics.m_FadeIn * m_Physics.m_MaxAge / 2.0 )) {

                    pPar->Alpha = ( 2.0 * pPar->Age.Duration() / ( m_Physics.m_FadeIn * m_Physics.m_MaxAge ) );

                } else pPar->Alpha = 1.0 ;


            }

            //FADEOUT
            if ( pPar->Visible && m_Physics.m_FadeOut>0.0 && (m_Physics.m_MaxAge>0) && ( (m_Physics.m_MaxAge/2.0) < pPar->Age.Duration() ) && (pPar->Age.Duration() < m_Physics.m_MaxAge) ) {

                if (  (m_Physics.m_FadeOut*m_Physics.m_MaxAge / 2.0) > (m_Physics.m_MaxAge - pPar->Age.Duration()) && pPar->Age.Duration() < m_Physics.m_MaxAge ) {

                    pPar->Alpha = ( m_Physics.m_MaxAge - pPar->Age.Duration() ) / (m_Physics.m_FadeOut *m_Physics.m_MaxAge / 2.0);

                }


            }

            //SIZEIN
            if ( pPar->Visible && m_Physics.m_SizeIn>0.0 && (m_Physics.m_MaxAge>0) &&  (pPar->Age.Duration() < m_Physics.m_MaxAge) ) {

                if ( pPar->Age.Duration() < ( m_Physics.m_SizeIn * m_Physics.m_MaxAge / 2.0 )) {

                    pPar->Scale = ( 2.0 * pPar->Age.Duration() / ( m_Physics.m_SizeIn * m_Physics.m_MaxAge ) );

                } else pPar->Scale = 1.0 ;


            }


            //SIZEOUT
            if ( pPar->Visible && m_Physics.m_SizeOut>0.0 && (m_Physics.m_MaxAge>0) && ( (m_Physics.m_MaxAge/2.0) < pPar->Age.Duration() ) && (pPar->Age.Duration() < m_Physics.m_MaxAge) ) {

                if (  (m_Physics.m_SizeOut*m_Physics.m_MaxAge / 2.0) > (m_Physics.m_MaxAge - pPar->Age.Duration()) && pPar->Age.Duration() < m_Physics.m_MaxAge ) {

                    pPar->Scale = ( m_Physics.m_MaxAge - pPar->Age.Duration() ) / (m_Physics.m_SizeOut *m_Physics.m_MaxAge / 2.0);

                }


            }

            if ( pPar->Visible ) {

                randommotionx = (m_Physics.m_RandomMotion>0)? (0.5-moMathf::UnitRandom())*m_Physics.m_MotionVector.X() : m_Physics.m_MotionVector.X();
                randommotiony = (m_Physics.m_RandomMotion>0)? (0.5-moMathf::UnitRandom())*m_Physics.m_MotionVector.Y() : m_Physics.m_MotionVector.Y();
                randommotionz = (m_Physics.m_RandomMotion>0)? (0.5-moMathf::UnitRandom())*m_Physics.m_MotionVector.Z() : m_Physics.m_MotionVector.Z();

                m_Physics.m_MotionVector.Normalize();
                if ( m_Physics.m_MotionVector.Length() > 0.0 ) {
                    if (m_Physics.m_RandomMotion>0.0) {
                        pPar->Velocity+= moVector3f( randommotionx, randommotiony, randommotionz ) * m_Physics.m_RandomMotion;
                    }
                }

            }




        }
    }
}

void moEffectParticlesSimple::ParticlesSimpleInfluence( float posx, float posy, float velx, float vely, float veln ) {

/*
    float left =  - normalf / 2.0;
    float top =  normalf / 2.0;
    float sizex = normalf / (float) m_cols;
    float sizey = normalf / (float) m_rows;

    float If,Jf;
    int I,J;
    int II,JJ;

    If = (posx - left - sizex/2.0 ) / sizex;
    Jf = -(posy - top + sizey/2.0 ) / sizey;

    II = (int) If;
    JJ = (int) Jf;


    int Ileft, Iright, Jtop, Jbottom;

    int INFLUENZA = 2;

    if (0<=II && II<m_cols && 0<=JJ && JJ<m_rows) {

        moParticlesSimple* pPar = m_ParticlesSimpleArray.Get( II + JJ*m_cols );

        //pBal->Size[0] = pBal->Size[0] / 1.1;
        //pBal->Size[1] = pBal->Size[1] / 1.1;

        pPar->Force = moVector3f( 0.0, 0.0, 1.01f );

        if (INFLUENZA>0) {

            //influence neigbours!!! on a force radius
            Ileft = II - INFLUENZA;
            Iright = II + INFLUENZA;
            Jbottom = JJ - INFLUENZA;
            Jtop = JJ + INFLUENZA;

            ( Ileft > 0 ) ? Ileft = Ileft : Ileft = 0;
            ( Iright < m_cols ) ? Iright = Iright : Iright = m_cols;
            ( Jbottom > 0 ) ? Jbottom = Jbottom : Jbottom = 0;
            ( Jtop < m_rows ) ? Jtop = Jtop : Jtop = m_rows;

            for( I=Ileft; I<Iright; I++) {
                for( J=Jbottom; J<Jtop; J++) {
                    m_ParticlesSimpleArray.Get( I + J*m_cols )->Force = moVector3f( 0.0, 0.0, 1.01f );
                }
            }

        }


    }
*/

}

void moEffectParticlesSimple::RevealingAll() {

    float scale = 1.0;
    moVector2f BSize = moVector2f( normalf / (float) m_cols, normalf / (float) m_rows );

    if (!TimerOfRevelation.Started() ) {
        TimerOfRevelation.Start();
    }

    if (TimerOfRevelation.Started() ) {

        if (TimerOfRevelation.Duration() < time_of_revelation ) {
            scale =  (float)( time_of_revelation - TimerOfRevelation.Duration() ) / (float)time_of_revelation;
        } else {
            scale = 0.0;
            TimerOfRevelation.Stop();
        }

        for( int i=0; i<m_cols ; i++) {
            for(int j=0; j<m_rows ; j++) {
                if (m_ParticlesSimpleArray[i+j*m_cols]->Size.X() > 0.1 )
                    m_ParticlesSimpleArray[i+j*m_cols]->Size = BSize * scale;
            }
        }

    }

    ///if full revealed reached:
    if (!TimerOfRevelation.Started() ) {
        revelation_status = PARTICLES_FULLREVEALED;
    }

}

void moEffectParticlesSimple::RestoringAll() {

    float scale = 1.0;
    float left =  - normalf / 2.0;
    float top =  normalf / 2.0;
    moVector2f BSize = moVector2f( normalf / (float) m_cols, normalf / (float) m_rows );

    if (!TimerOfRestoration.Started() ) {
        TimerOfRestoration.Start();
    }

    if (TimerOfRestoration.Started() ) {

        if (TimerOfRestoration.Duration() < time_of_restoration ) {
            scale = 1.0f -  (float)( time_of_restoration - TimerOfRestoration.Duration() ) / (float)time_of_restoration;
        } else {
            scale = 1.0f;
            TimerOfRestoration.Stop();
        }

        for( int i=0; i<m_cols ; i++) {
            for(int j=0; j<m_rows ; j++) {
                m_ParticlesSimpleArray[i+j*m_cols]->Size = BSize * scale;
                m_ParticlesSimpleArray[i+j*m_cols]->Pos3d = moVector3f(   left + (float)(i)*BSize.X() + BSize.X()/2.0,
                                        top - (float)(j)*BSize.Y() - BSize.Y()/2.0,
                                        0.0f );
            }
        }

    }

    ///if full revealed reached:
    if (!TimerOfRestoration.Started() ) {
        revelation_status = PARTICLES_FULLRESTORED;
    }
}


void moEffectParticlesSimple::CalculateForces(bool tmparray)
{
   int i,p1,p2;
   //moVector3f down(1.0,1.0,-1.0);
   moVector3f zero(0.0,0.0,0.0);
   moVector3f f;
   moVector3f atdis;
   double len,dx,dy,dz;


   float left =  - (m_Physics.m_EmitterSize.X()) / 2.0;
    float top =  m_Physics.m_EmitterSize.Y() / 2.0;

   for ( i=0; i < m_ParticlesSimpleArray.Count(); i++ ) {
       moParticlesSimple* pPar = m_ParticlesSimpleArray[i];
      pPar->Force = zero;

      /*if (pPar->Fixed)
         continue;*/

      /* Gravitation */
      switch(m_Physics.m_AttractorType) {
        case PARTICLES_ATTRACTORTYPE_POINT:
            pPar->Force = ( m_Physics.m_AttractorVector - pPar->Pos3d )*(m_Physics.gravitational * pPar->Mass);
            break;

        case PARTICLES_ATTRACTORTYPE_TRACKER:
            if (m_pTrackerData) {
                pPar->Force = ( moVector3f( 0.5f - m_pTrackerData->GetBarycenter().X(), 0.5f - m_pTrackerData->GetBarycenter().Y(), 0.0 ) - pPar->Pos3d )*(m_Physics.gravitational * pPar->Mass);
            }
            break;
        case PARTICLES_ATTRACTORTYPE_GRID:
            switch( m_Physics.m_AttractorMode ) {
                case PARTICLES_ATTRACTORMODE_STICK:
                case PARTICLES_ATTRACTORMODE_ACCELERATION:

                    pPar->Destination = moVector3f(   ( left + pPar->Pos.X()*pPar->Size.X() + pPar->Size.X()/2.0 )*m_Physics.m_AttractorVector.X() ,
                                                     ( top - pPar->Pos.Y()*pPar->Size.Y() - pPar->Size.Y()/2.0 )*m_Physics.m_AttractorVector.Y(),
                                                        m_Physics.m_AttractorVector.Z() );

                    if (m_Physics.m_AttractorMode==PARTICLES_ATTRACTORMODE_STICK && moVector3f( pPar->Destination - pPar->Pos3d ).Length() < 0.1 ) {
                        pPar->Pos3d = pPar->Destination;
                        pPar->Velocity = zero;
                        pPar->Force = zero;
                    } else pPar->Force = ( pPar->Destination - pPar->Pos3d )*(m_Physics.gravitational * pPar->Mass);
                    break;
                case PARTICLES_ATTRACTORMODE_LINEAR:
                    pPar->Destination = moVector3f(   ( left + pPar->Pos.X()*pPar->Size.X() + pPar->Size.X()/2.0 )*m_Physics.m_AttractorVector.X() ,
                                                     ( top - pPar->Pos.Y()*pPar->Size.Y() - pPar->Size.Y()/2.0 )*m_Physics.m_AttractorVector.Y(),
                                                        m_Physics.m_AttractorVector.Z() );

                    pPar->Pos3d = pPar->Pos3d + ( pPar->Destination - pPar->Pos3d) * m_Physics.gravitational;

                    //atdis =( pPar->Destination - pPar->Pos3d);
                    //if ( 0.04 < atdis.Length()  && atdis.Length() < 0.05 )  {
                        //MODebug2->Message( moText("Position reached : X:") + FloatToStr(pPar->Pos3d.X()) + moText(" Y:") + FloatToStr(pPar->Pos3d.Y()) );
                   // }
                    break;
                default:
                    break;
            }
            break;

        }


      /* Viscous drag */
      pPar->Force-= pPar->Velocity*m_Physics.viscousdrag;
   }

   // Handle the spring interaction
   /*
   for (i=0;i<ns;i++) {
      p1 = s[i].from;
      p2 = s[i].to;
      dx = p[p1].p.x - p[p2].p.x;
      dy = p[p1].p.y - p[p2].p.y;
      dz = p[p1].p.z - p[p2].p.z;
      len = sqrt(dx*dx + dy*dy + dz*dz);
      f.x  = s[i].springconstant  * (len - s[i].restlength);
      f.x += s[i].dampingconstant * (p[p1].v.x - p[p2].v.x) * dx / len;
      f.x *= - dx / len;
      f.y  = s[i].springconstant  * (len - s[i].restlength);
      f.y += s[i].dampingconstant * (p[p1].v.y - p[p2].v.y) * dy / len;
      f.y *= - dy / len;
      f.z  = s[i].springconstant  * (len - s[i].restlength);
      f.z += s[i].dampingconstant * (p[p1].v.z - p[p2].v.z) * dz / len;
      f.z *= - dz / len;
      if (!p[p1].fixed) {
         p[p1].f.x += f.x;
         p[p1].f.y += f.y;
         p[p1].f.z += f.z;
      }
      if (!p[p2].fixed) {
         p[p2].f.x -= f.x;
         p[p2].f.y -= f.y;
         p[p2].f.z -= f.z;
      }
   }
 */
}

void moEffectParticlesSimple::UpdateParticles( double dt,int method )
{
   int i;
   float randommotionx;
   float randommotiony;
   float randommotionz;

   switch (method) {
   case 0:
                                      /* Euler */
      Regenerate();
      CalculateForces();
      CalculateDerivatives(false,dt);
      for ( i=0; i<m_ParticlesSimpleArray.Count(); i++ ) {
        moParticlesSimple* pPar = m_ParticlesSimpleArray[i];

        randommotionx = (m_Physics.m_RandomMotion>0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomMotion*m_Physics.m_MotionVector.X() : m_Physics.m_MotionVector.X();
        randommotiony = (m_Physics.m_RandomMotion>0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomMotion*m_Physics.m_MotionVector.Y() : m_Physics.m_MotionVector.Y();
        randommotionz = (m_Physics.m_RandomMotion>0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomMotion*m_Physics.m_MotionVector.Z() : m_Physics.m_MotionVector.Z();

        if (pPar && dt!=0.0) {
            pPar->Pos3d+= pPar->dpdt * dt;
            pPar->Velocity+= pPar->dvdt * dt;
        }
      }
      break;
   case 1:                                   /* Midpoint */
      Regenerate();
      CalculateForces();
      CalculateDerivatives(false,dt);
      for (i=0;i<m_ParticlesSimpleArray.Count();i++) {
            moParticlesSimple* pPar = m_ParticlesSimpleArray[i];
            moParticlesSimple* ptmpPar = m_ParticlesSimpleArrayTmp[i];
            if (pPar && ptmpPar &&  dt!=0.0) {
                ptmpPar->Pos3d = pPar->Pos3d;
                ptmpPar->Velocity = pPar->Velocity;
                ptmpPar->Force = pPar->Force;
                ptmpPar->Pos3d = pPar->dpdt * dt / 2;
                ptmpPar->Pos3d+= pPar->dvdt * dt / 2;
            }
      }
      CalculateForces(true);
      CalculateDerivatives(true,dt);
      for ( i=0; i < m_ParticlesSimpleArray.Count(); i++ ) {
            moParticlesSimple* pPar = m_ParticlesSimpleArray[i];
            moParticlesSimple* ptmpPar = m_ParticlesSimpleArrayTmp[i];
            if (pPar && ptmpPar &&  dt!=0.0) {
                pPar->Pos3d+= ptmpPar->dpdt * dt;
                pPar->Velocity += ptmpPar->dvdt * dt;
            }
      }
      break;
   }
}

/*
   Calculate the derivatives
   dp/dt = v
   dv/dt = f / m
*/
void moEffectParticlesSimple::CalculateDerivatives(bool tmparray, double dt)
{
   int i;
   if (tmparray) {
       for ( i=0; i<m_ParticlesSimpleArrayTmp.Count(); i++) {
          if (dt>0) m_ParticlesSimpleArrayTmp[i]->dpdt = m_ParticlesSimpleArrayTmp[i]->Velocity;
          if (dt>0) m_ParticlesSimpleArrayTmp[i]->dvdt = m_ParticlesSimpleArrayTmp[i]->Force * 1.0f / m_ParticlesSimpleArrayTmp[i]->Mass;

       }
   } else {
       for ( i=0; i<m_ParticlesSimpleArray.Count(); i++) {
         if (dt>0) m_ParticlesSimpleArray[i]->dpdt = m_ParticlesSimpleArray[i]->Velocity;

         if (dt>0) m_ParticlesSimpleArray[i]->dvdt = m_ParticlesSimpleArray[i]->Force *  1.0f / m_ParticlesSimpleArray[i]->Mass;

       }
   }
}

/*
   A 1st order 1D DE solver.
   Assumes the function is not time dependent.
   Parameters
      h      - step size
      y0     - last value
      method - algorithm to use [0,5]
      fcn    - evaluate the derivative of the field
*/
double moEffectParticlesSimple::Solver1D(double h,double y0,int method,double (*fcn)(double))
{
   double ynew;
   double k1,k2,k3,k4,k5,k6;

   switch (method) {
   case 0:                          /* Euler method */
      k1 = h * (*fcn)(y0);
      ynew = y0 + k1;
      break;
   case 1:                          /* Modified Euler */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + k1);
      ynew = y0 + (k1 + k2) / 2;
      break;
   case 2:                          /* Heuns method */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + 2 * k1 / 3);
      ynew = y0 + k1 / 4 + 3 * k2 / 4;
      break;
   case 3:                          /* Midpoint */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + k1 / 2);
      ynew = y0 + k2;
      break;
   case 4:                          /* 4'th order Runge-kutta */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + k1/2);
      k3 = h * (*fcn)(y0 + k2/2);
      k4 = h * (*fcn)(y0 + k3);
      ynew = y0 + k1 / 6 + k2 / 3 + k3 / 3 + k4 / 6;
      break;
   case 5:                          /* England 4'th order, six stage */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + k1 / 2);
      k3 = h * (*fcn)(y0 + (k1 + k2) / 4);
      k4 = h * (*fcn)(y0 - k2 + 2 * k3);
      k5 = h * (*fcn)(y0 + (7 * k1 + 10 * k2 + k4) / 27);
      k6 = h * (*fcn)(y0 + (28*k1 - 125*k2 + 546*k3 + 54*k4 - 378*k5) / 625);
      ynew = y0 + k1 / 6 + 4 * k3 / 6 + k4 / 6;
      break;
   }

   return(ynew);
}


void moEffectParticlesSimple::ParticlesSimpleAnimation( moTempo* tempogral, moEffectState* parentstate ) {

    int i,j;
    int I,J;
    int Ileft, Iright, Jtop, Jbottom;

    switch(revelation_status) {
        case PARTICLES_FULLRESTORED:
            /*
            if (MotionActivity.Started()) {
                if (!TimerFullRevelation.Started()) {
                    TimerFullRevelation.Start();
                    revelation_status = PARTICLES_REVEALING;
                }
            }*/
            break;
        case PARTICLES_FULLREVEALED:
            /*
            if (!TimerFullRestoration.Started()) {
                TimerFullRestoration.Start();
            }

            if (TimerFullRestoration.Duration() > time_tofull_restoration ) {
                TimerFullRestoration.Stop();
                revelation_status = PARTICLES_RESTORINGALL;
            }*/
            break;
        case PARTICLES_REVEALINGALL:
            //RevealingAll();
            break;

        case PARTICLES_RESTORINGALL:
            //RestoringAll();
            break;

        case PARTICLES_REVEALING:
            /*
            if (TimerFullRevelation.Started()) {
                if (TimerFullRevelation.Duration() > time_tofull_revelation ) {
                    TimerFullRevelation.Stop();
                    revelation_status = PARTICLES_REVEALINGALL;
                }
            }
*/
            for( i = 0; i<m_cols ; i++) {
                for( j = 0; j<m_rows ; j++) {

                    moParticlesSimple* pPar = m_ParticlesSimpleArray.Get( i + j*m_cols );



                }

            }
            break;

    }


}

void moEffectParticlesSimple::TrackParticle( int partid ) {
    moParticlesSimple* pPar = NULL;
    pPar =  m_ParticlesSimpleArray.Get( partid );
    int i = partid;
    int j = 0;
    if (pPar) {
        //MODebug2->Push( moText("Pos:") + IntToStr(i) + moText(" J:") + IntToStr(j) + moText(" lum:") + IntToStr(lum) + moText(" lumindex:") + IntToStr(lumindex) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2));
        MODebug2->Push( moText("Pos X:") + IntToStr(pPar->Pos.X()) + moText(" Pos Y:") + IntToStr(pPar->Pos.Y()) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2));
        MODebug2->Push( moText("X:") + FloatToStr(pPar->Pos3d.X()) + moText(" Y:") + FloatToStr(pPar->Pos3d.Y()) + moText(" Z:") + FloatToStr(pPar->Pos3d.Z()) );
        MODebug2->Push( moText("VX:") + FloatToStr(pPar->Velocity.X()) + moText(" VY:") + FloatToStr(pPar->Velocity.Y()) + moText(" VZ:") + FloatToStr(pPar->Velocity.Z()) );
        MODebug2->Push( moText("FX:") + FloatToStr(pPar->Force.X()) + moText(" FY:") + FloatToStr(pPar->Force.Y()) + moText(" FZ:") + FloatToStr(pPar->Force.Z()) );
        MODebug2->Push( moText("Size X:") + FloatToStr(pPar->Size.X()) + moText(" Size Y:") + FloatToStr(pPar->Size.Y()) );
        MODebug2->Push( moText("TCoord X:") + FloatToStr(pPar->TCoord.X()) + moText(" TCoord Y:") + FloatToStr(pPar->TCoord.Y()) );
        MODebug2->Push( moText("TCoord2 X:") + FloatToStr(pPar->TCoord2.X()) + moText(" TCoord2 Y:") + FloatToStr(pPar->TCoord2.Y()) );
        MODebug2->Push( moText("Visible:") + IntToStr((int)pPar->Visible) );
        MODebug2->Push( moText("Age:") + IntToStr((int)pPar->Age.Duration()) );
    }

}

void moEffectParticlesSimple::DrawParticlesSimple( moTempo* tempogral, moEffectState* parentstate ) {

    int i,j;
    int cols2,rows2;

    //if ((moGetTicks() % 1000) == 0) TrackParticle(1);

    cols2 = m_Config[moR(PARTICLES_WIDTH)][MO_SELECTED][0].Int();
    rows2 = m_Config[moR(PARTICLES_HEIGHT)][MO_SELECTED][0].Int();

    if (cols2!=m_cols || rows2!=m_rows) {
        InitParticlesSimple(cols2,rows2);
    }

    if ( last_tick > tempogral->ticks || tempogral->ticks==0 ) {
        m_Physics.EmitionTimer.Start();
    }



    //glBindTexture( GL_TEXTURE_2D, 0 );
    if ( texture_mode!=PARTICLES_TEXTUREMODE_MANY && texture_mode!=PARTICLES_TEXTUREMODE_MANY2PATCH ) {
        //glBindTexture( GL_TEXTURE_2D, /*m_Config[moR(PARTICLES_TEXTURE)].GetData()->GetGLId(&state.tempo, 1, NULL )*/0 );
        if (glid>=0) glBindTexture( GL_TEXTURE_2D, glid );
        else glBindTexture( GL_TEXTURE_2D, 0);
    }
    //glColor4f(1.0,1.0,1.0,1.0);
    //glDisable( GL_CULL_FACE);
    //glDisable( GL_DEPTH_TEST);
    //glFrontFace( GL_CW);
    //glPolygonMode( GL_LINES, GL_FRONT_AND_BACK);

    //SetBlending( (moBlendingModes) m_Config[moR(PARTICLES_BLENDING)][MO_SELECTED][0].Int() );


    float sizexd2,sizeyd2;
    float tsizex,tsizey;

    moFont* pFont = m_Config[moR(PARTICLES_FONT)].GetData()->Font();
    moText Texto;

    ///CONSTANTE
    ///version dt = constante por cuadro
    //double dt = m_Config.Eval( moR(PARTICLES_SYNC),state.tempo.ang) * (double)(state.tempo.delta) /  (double)100.0;


    ///VARIABLE
    ///relativo al timecode...
    ///dt relativo al tempo (variable)
    /// entre 0 y inf.
    /// 0 significa que casi no hubo lapso de tiempo medible entre el cuadro anterior y este, por lo tanto no hay animacion
    /// 1 significa que el lapso de tiempo entre el evento anterior y el actual coincide con 1 cuadro cada 16.6 ms ( 60 fps )
    /// > 1 significa que el tiempo entre 2 cuadros supero el correspondiente a 60 fps
    /// > 4 significa que estamos por debajo de los 15 fps, pasada esta brecha deberiamos iterar N veces para obtener resultados correctos.
    double dtrel = (double) ( tempogral->ticks - last_tick ) / (double)16.666666;
    if ( ( (last_tick/100) % 50 ) == 0 ) MODebug2->Push("dtrel:"+FloatToStr(dtrel));
    double dt = m_Config.Eval( moR(PARTICLES_SYNC),state.tempo.ang) * dtrel * (double)(state.tempo.delta) /  (double)100.0;

    UpdateParticles( dt, 0 );
    ParticlesSimpleAnimation( tempogral, parentstate );

    float idxt = 0.0;

    for( j = 0; j<m_rows ; j++) {
      for( i = 0; i<m_cols ; i++) {


            idxt = 0.5 + (float)( i + j * m_cols ) / (float)( m_cols * m_rows * 2 );

            moParticlesSimple* pPar = m_ParticlesSimpleArray.Get( i + j*m_cols );

            if (pPar->Visible) {


                if (texture_mode==PARTICLES_TEXTUREMODE_MANY || texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH ) {
                    //pPar->GLId = 22;
                    if (pPar->GLId>0) {
                        glActiveTextureARB( GL_TEXTURE0_ARB );
                        glEnable(GL_TEXTURE_2D);
                        //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                        glBindTexture( GL_TEXTURE_2D, pPar->GLId );
                    }
                    if (pPar->GLId2>0) {
                        glActiveTextureARB( GL_TEXTURE1_ARB );
                        glEnable(GL_TEXTURE_2D);
                        //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                        glBindTexture( GL_TEXTURE_2D, pPar->GLId2 );
                    }
                }

                if (moScript::IsInitialized()) {
                    if (ScriptHasFunction("RunParticle")) {
                        SelectScriptFunction("RunParticle");
                        AddFunctionParam( (int) ( i + j*m_cols ) );
                        if (!RunSelectedFunction(1)) {
                            MODebug2->Error( moText("RunParticle function not executed") );
                        }
                    }
                }

                sizexd2 = (pPar->Size.X()* pPar->ImageProportion )/2.0;
                sizeyd2 = pPar->Size.Y()/2.0;
                tsizex = pPar->TSize.X();
                tsizey = pPar->TSize.Y();

                glPushMatrix();

                glTranslatef( pPar->Pos3d.X(), pPar->Pos3d.Y(),  pPar->Pos3d.Z() );

                glRotatef(  m_Config[moR(PARTICLES_ROTATEZ_PARTICLE)].GetData()->Fun()->Eval(state.tempo.ang)+pPar->Rotation.Z(), 0.0, 0.0, 1.0 );
                glRotatef(  m_Config[moR(PARTICLES_ROTATEY_PARTICLE)].GetData()->Fun()->Eval(state.tempo.ang)+pPar->Rotation.Y(), 0.0, 1.0, 0.0 );
                glRotatef(  m_Config[moR(PARTICLES_ROTATEX_PARTICLE)].GetData()->Fun()->Eval(state.tempo.ang)+pPar->Rotation.X(), 1.0, 0.0, 0.0 );

                //scale
                glScalef(   m_Config[moR(PARTICLES_SCALEX_PARTICLE)].GetData()->Fun()->Eval(state.tempo.ang)*pPar->Scale,
                            m_Config[moR(PARTICLES_SCALEY_PARTICLE)].GetData()->Fun()->Eval(state.tempo.ang)*pPar->Scale,
                            m_Config[moR(PARTICLES_SCALEZ_PARTICLE)].GetData()->Fun()->Eval(state.tempo.ang)*pPar->Scale);


                glColor4f(  m_Config[moR(PARTICLES_COLOR)][MO_SELECTED][MO_RED].Fun()->Eval(state.tempo.ang) * pPar->Color.X() * state.tintr,

                            m_Config[moR(PARTICLES_COLOR)][MO_SELECTED][MO_GREEN].Fun()->Eval(state.tempo.ang) * pPar->Color.Y() * state.tintg,

                            m_Config[moR(PARTICLES_COLOR)][MO_SELECTED][MO_BLUE].Fun()->Eval(state.tempo.ang) * pPar->Color.Z() * state.tintb,

                            m_Config[moR(PARTICLES_COLOR)][MO_SELECTED][MO_ALPHA].Fun()->Eval(state.tempo.ang) * m_Config[moR(PARTICLES_ALPHA)].GetData()->Fun()->Eval(state.tempo.ang) * state.alpha * pPar->Alpha );


                moVector3f CO(m_Physics.m_EyeVector - pPar->Pos3d);
                moVector3f U,V,W;
                moVector3f CPU,CPW;
                moVector3f A,B,C,D;

                moVector3f CENTRO;

                U = moVector3f( 0.0f, 0.0f, 1.0f );
                V = moVector3f( 1.0f, 0.0f, 0.0f );
                W = moVector3f( 0.0f, 1.0f, 0.0f );

                U = CO;
                U.Normalize();

                //orientation always perpendicular to plane (X,Y)
                switch(m_Physics.m_OrientationMode) {

                        case PARTICLES_ORIENTATIONMODE_FIXED:
                            //cuadrado centrado en Pos3d....
                            U = moVector3f( 0.0f, 0.0f, 1.0f );
                            V = moVector3f( 1.0f, 0.0f, 0.0f );
                            W = moVector3f( 0.0f, 1.0f, 0.0f );
                            break;

                        case PARTICLES_ORIENTATIONMODE_CAMERA:
                            V = moVector3f( -CO.Y(), CO.X(), 0.0f );
                            V.Normalize();

                            CPU = moVector3f( U.X(), U.Y(), 0.0f );
                            W = moVector3f( 0.0f, 0.0f, CPU.Length() );
                            CPU.Normalize();
                            CPW = CPU * -U.Z();
                            W+= CPW;
                            break;

                        case PARTICLES_ORIENTATIONMODE_MOTION:
                            U = pPar->Velocity;
                            U.Normalize();
                            if (U.Length() < 0.5) {
                                U = moVector3f( 0.0, 0.0, 1.0 );
                                U.Normalize();
                            }
                            V = moVector3f( -U.Y(), U.X(), 0.0f );
                            V.Normalize();
                            CPU = moVector3f( U.X(), U.Y(), 0.0f );
                            W = moVector3f( 0.0f, 0.0f, CPU.Length() );
                            CPU.Normalize();
                            CPW = CPU * -U.Z();
                            W+= CPW;
                            break;
                }

                A = V * -sizexd2 + W * sizeyd2;
                B = V *sizexd2 +  W * sizeyd2;
                C = V *sizexd2 + W * -sizeyd2;
                D = V * -sizexd2 + W * -sizeyd2;


                //cuadrado centrado en Pos3d....


                glBegin(GL_QUADS);
                    //glColor4f( 1.0, 0.5, 0.5, idxt );

                    if (pPar->GLId2>0) {
                        //glColor4f( 1.0, 0.5, 0.5, idxt );
                        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, pPar->TCoord.X(), pPar->TCoord.Y() );
                        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, pPar->TCoord2.X(), pPar->TCoord2.Y());
                    } else glTexCoord2f( pPar->TCoord.X(), pPar->TCoord.Y() );
                    glVertex3f( A.X(), A.Y(), 0.0);

                    //glColor4f( 0.5, 1.0, 0.5, idxt );

                    if (pPar->GLId2>0) {
                        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, pPar->TCoord.X()+tsizex, pPar->TCoord.Y() );
                        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, pPar->TCoord2.X()+pPar->TSize2.X(), pPar->TCoord2.Y());
                    } else glTexCoord2f( pPar->TCoord.X()+tsizex, pPar->TCoord.Y() );
                    glVertex3f( B.X(), B.Y(), 0.0);

                    //glColor4f( 0.5, 0.5, 1.0, idxt );
                    if (pPar->GLId2>0) {
                        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, pPar->TCoord.X()+tsizex, pPar->TCoord.Y()+tsizey );
                        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, pPar->TCoord2.X()+pPar->TSize2.X(), pPar->TCoord2.Y()+pPar->TSize2.Y());
                    } else glTexCoord2f( pPar->TCoord.X()+tsizex, pPar->TCoord.Y()+tsizey );
                    glVertex3f( C.X(), C.Y(), 0.0);

                    //glColor4f( 1.0, 1.0, 1.0, idxt );
                    if (pPar->GLId2>0) {
                        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, pPar->TCoord.X(), pPar->TCoord.Y()+pPar->TSize.Y());
                        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, pPar->TCoord2.X(), pPar->TCoord2.Y()+pPar->TSize2.Y());
                    } else glTexCoord2f( pPar->TCoord.X(), pPar->TCoord.Y()+tsizey );
                    glVertex3f( D.X(), D.Y(), 0.0);
                glEnd();

                //draw vectors associated...
                if ( drawing_features>2 ) {
                    CENTRO = moVector3f( 0.0 , 0.0, 0.0 );

                    glDisable( GL_TEXTURE_2D );
                    glLineWidth( 8.0 );
                    glBegin(GL_LINES);
                        ///draw U
                        glColor4f( 0.0, 1.0, 1.0, 1.0);
                        glVertex3f( CENTRO.X(), CENTRO.Y(), 0.0001);

                        glColor4f( 0.0, 1.0, 1.0, 1.0);
                        glVertex3f( CENTRO.X() + U.X(), CENTRO.Y() + U.Y(), 0.0001);

                    glEnd();

                    glBegin(GL_LINES);
                        ///draw V
                        glColor4f( 1.0, 0.0, 1.0, 1.0);
                        glVertex3f( CENTRO.X(), CENTRO.Y(), 0.0001);

                        glColor4f( 1.0, 0.0, 1.0, 1.0);
                        glVertex3f( CENTRO.X() + V.X(), CENTRO.Y() + V.Y(), 0.0001);

                    glEnd();

                    glBegin(GL_LINES);
                        ///draw W
                        glColor4f( 0.0, 0.0, 1.0, 1.0);
                        glVertex3f( CENTRO.X(), CENTRO.Y(), 0.0001);

                        glColor4f( 0.0, 0.0, 1.0, 1.0);
                        glVertex3f( CENTRO.X() + W.X(), CENTRO.Y() + W.Y(), 0.0001);

                    glEnd();
                    glEnable( GL_TEXTURE_2D );
                }


                glPopMatrix();
            }
        }
    }

    if (pFont && drawing_features>2) {
        for( i = 0; i<m_cols ; i++) {
            for( j = 0; j<m_rows ; j++) {

                moParticlesSimple* pPar = m_ParticlesSimpleArray.Get( i + j*m_cols );
                if ((i + j*m_cols) % 10 == 0 ) {
                    Texto = moText( IntToStr(i + j*m_cols));
                    Texto.Left(5);
                    Texto+= moText("F:")+moText( (moText)FloatToStr( pPar->Force.X() ).Left(5) + moText(",")
                                + (moText)FloatToStr( pPar->Force.Y() ).Left(5)
                                + moText(",") + (moText)FloatToStr( pPar->Force.Z() ).Left(5) );

                    Texto+= moText("V:")+ moText( (moText)FloatToStr( pPar->Velocity.X() ).Left(5) + moText(",")
                                + (moText)FloatToStr( pPar->Velocity.Y() ).Left(5)
                                + moText(",") + (moText)FloatToStr( pPar->Velocity.Z() ).Left(5) );

                    pFont->Draw( pPar->Pos3d.X(),
                                 pPar->Pos3d.Y(),
                                 Texto );


                    Texto = moText( moText("(") + (moText)FloatToStr(pPar->TCoord.X()).Left(4) + moText(",") + (moText)FloatToStr(pPar->TCoord.Y()).Left(4) + moText(")") );

                    pFont->Draw( pPar->Pos3d.X()-sizexd2,
                                 pPar->Pos3d.Y()+sizeyd2-2,
                                 Texto );

                    Texto = moText( moText("(") + (moText)FloatToStr(pPar->TCoord.X()+tsizex).Left(4) + moText(",") + (moText)FloatToStr(pPar->TCoord.Y()).Left(4) + moText(")"));

                    pFont->Draw( pPar->Pos3d.X()+sizexd2-12,
                                 pPar->Pos3d.Y()+sizeyd2-5,
                                 Texto );

                    Texto = moText( moText("(") + (moText)FloatToStr(pPar->TCoord.X()+tsizex).Left(4) + moText(",") + (moText)FloatToStr(pPar->TCoord.Y()+tsizey).Left(4) + moText(")"));

                    pFont->Draw( pPar->Pos3d.X()+sizexd2-12,
                                 pPar->Pos3d.Y()-sizeyd2+2,
                                 Texto );

                    Texto = moText( moText("(") + (moText)FloatToStr(pPar->TCoord.X()).Left(4) + moText(",") + (moText)FloatToStr(pPar->TCoord.Y()+tsizey).Left(4) + moText(")"));

                    pFont->Draw( pPar->Pos3d.X()-sizexd2,
                                 pPar->Pos3d.Y()-sizeyd2+5,
                                 Texto );
                }

            }
        }
    }

    if (pFont && drawing_features>2) {

                Texto = moText( moText("T2 Rest.:") + IntToStr(TimerFullRestoration.Duration()));
                pFont->Draw( -10.0f,
                             0.0f,
                             Texto );

                Texto = moText( moText("T2 Revel.:") + IntToStr(TimerFullRevelation.Duration()));
                pFont->Draw( -10.0f,
                             3.0f,
                             Texto );

                Texto = moText( moText("T Revel.:") + IntToStr(TimerOfRevelation.Duration()) + moText(" rev: ") + IntToStr(time_of_revelation));
                pFont->Draw( -10.0f,
                             6.0f,
                             Texto );

                Texto = moText( moText("T Rest.:") + IntToStr(TimerOfRestoration.Duration())  + moText(" res: ") + IntToStr(time_of_restoration) );
                pFont->Draw( -10.0f,
                             9.0f,
                             Texto );

                Texto = moText("Status: ");

                switch(revelation_status) {
                    case PARTICLES_FULLRESTORED:
                        Texto+= moText("Full Restored");
                        break;
                    case PARTICLES_REVEALING:
                        Texto+= moText("Revealing");
                        break;
                    case PARTICLES_REVEALINGALL:
                        Texto+= moText("Revealing");
                        break;
                    case PARTICLES_FULLREVEALED:
                        Texto+= moText("Full Revealed");
                        break;
                    case PARTICLES_RESTORING:
                        Texto+= moText("Restoring");
                        break;
                    case PARTICLES_RESTORINGALL:
                        Texto+= moText("Restoring All");
                        break;
                }
                pFont->Draw( -10.0f,
                             13.0f,
                             Texto );
    }

    last_tick = tempogral->ticks;

}

using namespace TUIO;

void moEffectParticlesSimple::DrawTracker() {

    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    m_pTrackerData = NULL;
    m_pTUIOData = NULL;

    bool has_motion = false;
    bool has_features = false;

    if (m_InletTrackerSystemIndex>-1) {
        moInlet* pInlet = m_Inlets.Get(m_InletTrackerSystemIndex);
        if (pInlet)
            if (pInlet->Updated()) {
                m_pTrackerData = (moTrackerSystemData *)pInlet->GetData()->Pointer();
            }
    }

    if (m_InletTuioSystemIndex>-1) {
        moInlet* pInlet = m_Inlets.Get(m_InletTuioSystemIndex);
        if (pInlet)
            if (pInlet->Updated()) {
                m_pTUIOData = (moTUIOSystemData *)pInlet->GetData()->Pointer();

                if (m_pTUIOData) {

                    TuioObject* pObject;

if (drawing_features > 0  ) {

                    std::list<TuioObject*> objects = m_pTUIOData->getTuioObjects();

                    for (std::list<TuioObject*>::iterator iter=objects.begin(); iter != objects.end(); iter++) {

                        pObject = (*iter);
                        if (pObject) {
                            moVector2f position( pObject->getX(), pObject->getY() * h / w);

                            glBindTexture(GL_TEXTURE_2D,0);
                            glColor4f(0.0, 1.0, 1.0, 1.0);

                            float angle = pObject->getAngle();

                            //int u = pObject->getSymbolID();

                            //moTrackerFeature* NF = NULL;
                            /*
                            if (m_pTrackerData) {
                                NF = m_pTrackerData->GetFeature(u);
                            }*/


                            ///DRAW ANGLE CENTER
                            //glRotatef(  angle*moMathf::RAD_TO_DEG , 0.0, 0.0, 1.0 );

                            glBegin(GL_QUADS);
                                glVertex2f((position.X() - 0.02)*normalf, (position.Y() - 0.02)*normalf);
                                glVertex2f((position.X() - 0.02)*normalf, (position.Y() + 0.02)*normalf);
                                glVertex2f((position.X() + 0.02)*normalf, (position.Y() + 0.02)*normalf);
                                glVertex2f((position.X() + 0.02)*normalf, (position.Y() - 0.02)*normalf);
                            glEnd();


                            ///DRAW angle vector
                            glColor4f( 0.0,1.0,1.0,1.0);
                            glLineWidth(8.0);
                            glBegin(GL_LINES);
                                glVertex2f( position.X(), position.Y());
                                glVertex2f( position.X() + 0.04*cos(angle), position.Y() + 0.04*sin(angle) );
                            glEnd();

                            ///speed

                            glColor4f( 0.4,0.6,0.2,1.0);
                            glLineWidth(4.0);
                            glBegin(GL_LINES);
                                glVertex2f( position.X(), position.Y());
                                glVertex2f( position.X() + pObject->getXSpeed(), position.Y() + pObject->getYSpeed() );
                            glEnd();

                            ///rotation speed
                            /*
                            glColor4f( 1.0,0.0,0.0,1.0);
                            glLineWidth(8.0);
                            glBegin(GL_LINES);
                                glVertex2f( position.X(), position.Y());
                                glVertex2f( position.X(), position.Y() + pObject->getRotationSpeed()*0.0001 );
                            glEnd();
                            */
                            //MODebug2->Push(moText("rspeed:")+IntToStr( (int)(pObject->getRotationSpeed()/10.0))  );

                            /*

                            if (NF) {
                                moVector2f pu( NF->x - 0.5, (-NF->y + 0.5) * h / w);
                                glColor4f( 0.5,0.0,0.0,1.0);
                                glBegin(GL_QUADS);
                                    glVertex2f( ( pu.X() - 0.01)*normalf, (pu.Y() - 0.01)*normalf);
                                    glVertex2f((pu.X() - 0.01)*normalf, (pu.Y() + 0.01)*normalf);
                                    glVertex2f((pu.X() + 0.01)*normalf, (pu.Y() + 0.01)*normalf);
                                    glVertex2f((pu.X() + 0.01)*normalf, (pu.Y() - 0.01)*normalf);
                                glEnd();
                            }
                            */


                        }

                    }

                    TuioCursor* pCursor;

                    std::list<TuioCursor*> cursors = m_pTUIOData->getTuioCursors();

                    for (std::list<TuioCursor*>::iterator iter=cursors.begin(); iter != cursors.end(); iter++) {

                        pCursor = (*iter);
                        if (pCursor) {
                            moVector2f position( pCursor->getX(), pCursor->getY() * h / w);

                            glBindTexture(GL_TEXTURE_2D,0);
                            glColor4f(0.0, 0.0, 1.0, 1.0);
                            ///try using moP5::ellipse(x,y,w,h)

                            float r = 0.02;
                            glLineWidth(4.0);
                            glBegin(GL_LINE_LOOP);
                                for(float t = 0; t <= moMathf::TWO_PI; t += moMathf::TWO_PI / 12 )
                                    glVertex2f( r* cos(t) + position.X(), r* sin(t) + position.Y());
                            glEnd();
                        }

                    }
}
                }

            }
    }

    /*
    if (m_pTUIOData) {
        int nobjects = m_pTUIOData->getTuioObjects().size();
        //MODebug2->Push( moText(" NOBJECTS: ") + IntToStr(nobjects) );

            for (int f = 0; f < m_pTUIOData->getTuioObjects().size(); f++)
            {

            }

    }
    */

    if (m_pTrackerData ) {

        glDisable(GL_TEXTURE_2D);

        m_bTrackerInit = true;

        //SelectScriptFunction("Reset");
        //RunSelectedFunction();

        //MODebug2->Push(IntToStr(TrackerId));

        //MODebug2->Push(moText("Receiving:") + IntToStr(m_pTrackerData->GetFeaturesCount()) );
        if (m_pTrackerData->GetFeaturesCount()>0) {
            int tw = m_pTrackerData->GetVideoFormat().m_Width;
            int th = m_pTrackerData->GetVideoFormat().m_Height;
            //MODebug2->Push(moText("vformat:")+IntToStr(tw)+moText("th")+IntToStr(th));

            m_TrackerBarycenter = moVector2f( ( ( m_pTrackerData->GetBarycenter().X() ) - 0.5),
                                              ( -( m_pTrackerData->GetBarycenter().Y() ) + 0.5) * h / w );

            //MODebug2->Push(moText("Barycenter x:")+FloatToStr(m_TrackerBarycenter.X()) + moText(" y:")+FloatToStr(m_TrackerBarycenter.Y()) );

            if (drawing_features > 2 ) {

                glBindTexture(GL_TEXTURE_2D,0);
                glColor4f(0.3, 1.0, 1.0, 1.0);

                glBegin(GL_QUADS);
                    glVertex2f((m_TrackerBarycenter.X() - 0.02)*normalf, (m_TrackerBarycenter.Y() - 0.02)*normalf);
                    glVertex2f((m_TrackerBarycenter.X() - 0.02)*normalf, (m_TrackerBarycenter.Y() + 0.02)*normalf);
                    glVertex2f((m_TrackerBarycenter.X() + 0.02)*normalf, (m_TrackerBarycenter.Y() + 0.02)*normalf);
                    glVertex2f((m_TrackerBarycenter.X() + 0.02)*normalf, (m_TrackerBarycenter.Y() - 0.02)*normalf);
                glEnd();

            }


            for (int f = 0; f < m_pTrackerData->GetFeaturesCount(); f++)
            {

                moTrackerFeature* pF = m_pTrackerData->GetFeature(f);

            if (pF && pF->valid) {

                float x = (pF->x ) - 0.5;
                float y = ( -(pF->y ) + 0.5 ) * h / w;
                float tr_x = (pF->tr_x ) - 0.5 ;
                float tr_y = ( -(pF->tr_y ) + 0.5 ) * h / w;
                float v_x = (pF->v_x );
                float v_y = -(pF->v_y);
                float vel = sqrtf( v_x*v_x+v_y*v_y );
                int v = pF->val;

                //MODebug2->Log(moText("    x:")+FloatToStr(pF->x) + moText(" y:")+FloatToStr(pF->y) );

                glBindTexture(GL_TEXTURE_2D,0);
                glColor4f(1.0, 0.0, 0.0, 1.0);

                if (v >= 0) glColor4f(0.0, 1.0, 0.0, 1.0);
                else if (v == -1) glColor4f(1.0, 0.0, 1.0, 1.0);
                else if (v == -2) glColor4f(1.0, 0.0, 1.0, 1.0);
                else if (v == -3) glColor4f(1.0, 0.0, 1.0, 1.0);
                else if (v == -4) glColor4f(1.0, 0.0, 1.0, 1.0);
                else if (v == -5) glColor4f(1.0, 0.0, 1.0, 1.0);




                if ( v>=0) {
                    has_features = true;

                    if (drawing_features > 0 ) {

                        glPointSize((GLfloat)4);
                        glLineWidth((GLfloat)4.0);

                        if ( pF->is_object) {
                            glColor4f(1.0, 0.0, 0.0, 1.0);

                            //Uplas[f] >=3 !!!!!
                            int myuplas[3];
                            myuplas[0] = -1;
                            myuplas[1] = -1;
                            myuplas[2] = -1;
                            int k1,k2,o = 0;
                            for( int pp=0; pp<m_pTrackerData->nPares; pp++) {
                                k1 = m_pTrackerData->m_Pares[pp][0];
                                k2 = m_pTrackerData->m_Pares[pp][1];
                                if (o<3) if (k1==f) myuplas[o++] = k2;
                                if (o<3) if (k2==f) myuplas[o++] = k1;
                            }

                            int countv = 1;
                            moVector2f obj_v( x, y );
                            glColor4f(1.0, 0.1, 0.1, 1.0);
                            //glPointSiz((GLfloat)10);
                            glLineWidth((GLfloat)2.0);
                            for( int pp=0; pp<3; pp++ ) {
                                int e = myuplas[pp];
                                moTrackerFeature* NF = m_pTrackerData->GetFeature(e);
                                if (NF) {
                                    moVector2f av2( NF->x - 0.5, (-NF->y + 0.5 ) * h / w );
                                    //obj_v+=av2;
                                    glBegin(GL_LINES);
                                        glVertex2f(obj_v.X(),obj_v.Y());
                                        glVertex2f(av2.X(),av2.Y());
                                    glEnd();
                                    countv++;
                                }
                            }
                        }

                        glBegin(GL_QUADS);
                            glVertex2f((x - 0.008)*normalf, (y - 0.008)*normalf);
                            glVertex2f((x - 0.008)*normalf, (y + 0.008)*normalf);
                            glVertex2f((x + 0.008)*normalf, (y + 0.008)*normalf);
                            glVertex2f((x + 0.008)*normalf, (y - 0.008)*normalf);
                        glEnd();
                    }


                    if (drawing_features > 1 ) {
                        glBegin(GL_QUADS);
                            glVertex2f((tr_x - 0.008)*normalf, (tr_y - 0.008)*normalf);
                            glVertex2f((tr_x - 0.008)*normalf, (tr_y + 0.008)*normalf);
                            glVertex2f((tr_x + 0.008)*normalf, (tr_y + 0.008)*normalf);
                            glVertex2f((tr_x + 0.008)*normalf, (tr_y - 0.008)*normalf);
                        glEnd();


                        glColor4f(1.0, 1.0, 1.0, 1.0);
                        glBegin(GL_LINES);
                            glVertex2f( x*normalf, y*normalf);
                            glVertex2f( tr_x*normalf, tr_y*normalf);
                        glEnd();
                    }

                    if ( vel > 0.01 && vel < 0.1) {

                        has_motion = true;

                        if (drawing_features > 1 ) {

                            glColor4f(0.0, 0.0, 1.0, 1.0);
                            //glPointSize((GLfloat)10);
                            glLineWidth((GLfloat)2.0);

                            glBegin(GL_LINES);
                                glVertex2f( x*normalf, y*normalf);
                                glVertex2f( (x+v_x)*normalf, (y+v_y)*normalf);
                            glEnd();
                        }

                        ///ParticlesSimpleInfluence( x*normalf, y*normalf, (x+v_x)*normalf, (y+v_y)*normalf, vel*normalf );
                    }


                }

            }


            }

            ///post process:
            ///add threshold??? some number of frames for really check...
            if (has_motion) {

                if (!MotionActivity.Started()) {
                    MotionActivity.Start();
                }

            } else {
                MotionActivity.Stop();
            }

            if (has_features) {

                if (!FeatureActivity.Started()) {
                    FeatureActivity.Start();
                }

            } else {
                FeatureActivity.Stop();
            }


        }

        glEnable(GL_TEXTURE_2D);

    }

}

void moEffectParticlesSimple::Draw( moTempo* tempogral, moEffectState* parentstate)
{

    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();
    frame++;
    moFont* pFont = m_Config[moR(PARTICLES_FONT)].GetData()->Font();

    UpdateParameters();

    PreDraw( tempogral, parentstate);

    // Cambiar la proyeccion para una vista ortogonal //
/*
*/


    if (ortho) {
        glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
        glLoadIdentity();									// Reset The Projection Matrix
        glOrtho(-0.5,0.5,-0.5*h/w,0.5*h/w,-1,1);                              // Set Up An Ortho Screen
    } else {
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
        glLoadIdentity();									// Reset The Projection Matrix
        m_pResourceManager->GetGLMan()->SetPerspectiveView( w, h );
    }

    glMatrixMode(GL_PROJECTION);

    if (!ortho) {
        if ( state.stereoside == MO_STEREO_NONE ) {

            gluLookAt(		m_Physics.m_EyeVector.X(),
                            m_Physics.m_EyeVector.Y(),
                            m_Physics.m_EyeVector.Z(),
                            //m_Config[moR(PARTICLES_VIEWX)].GetData()->Fun()->Eval(state.tempo.ang)
                            m_Config.Eval( moR(PARTICLES_VIEWX), state.tempo.ang),
                            //m_Config[moR(PARTICLES_VIEWY)].GetData()->Fun()->Eval(state.tempo.ang)
                            m_Config.Eval( moR(PARTICLES_VIEWY), state.tempo.ang),
                            //m_Config[moR(PARTICLES_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang)
                            m_Config.Eval( moR(PARTICLES_VIEWZ), state.tempo.ang),
                            0, 1, 0);

        } else {
            if ( state.stereoside == MO_STEREO_LEFT ) {
               gluLookAt(	m_Physics.m_EyeVector.X()-0.1,
                            m_Physics.m_EyeVector.Y(),
                            m_Physics.m_EyeVector.Z(),
                            m_Config[moR(PARTICLES_VIEWX)].GetData()->Fun()->Eval(state.tempo.ang)-0.1,
                            m_Config[moR(PARTICLES_VIEWY)].GetData()->Fun()->Eval(state.tempo.ang),
                            m_Config[moR(PARTICLES_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang),
                            0, 1, 0);
            } else if ( state.stereoside == MO_STEREO_RIGHT ) {
                gluLookAt(	m_Physics.m_EyeVector.X()+0.1,
                            m_Physics.m_EyeVector.Y(),
                            m_Physics.m_EyeVector.Z(),
                            m_Config[moR(PARTICLES_VIEWX)].GetData()->Fun()->Eval(state.tempo.ang)+0.1,
                            m_Config[moR(PARTICLES_VIEWY)].GetData()->Fun()->Eval(state.tempo.ang),
                            m_Config[moR(PARTICLES_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang),
                            0, 1, 0);
            }

        }
    }

    if (texture_mode==PARTICLES_TEXTUREMODE_UNIT) {
      glid = m_Config.GetGLId( moR(PARTICLES_TEXTURE), &state.tempo);
    }

    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glLoadIdentity();									// Reset The View

    //glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    //glClear( GL_DEPTH_BUFFER_BIT );
    //glEnable(GL_ALPHA_TEST);


    //glDisable( GL_CULL_FACE);
    //glFrontFace( GL_CCW);

    //glDepthMask(true);

    //esto deberia ser parametrizable...
	//glEnable( GL_DEPTH_TEST);
	//glDisable( GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    //setUpLighting();

    tx = m_Config.Eval( moR(PARTICLES_TRANSLATEX), state.tempo.ang);
    ty = m_Config.Eval( moR(PARTICLES_TRANSLATEY), state.tempo.ang);
    tz = m_Config.Eval( moR(PARTICLES_TRANSLATEZ), state.tempo.ang);


    rz = m_Config.Eval( moR(PARTICLES_ROTATEZ), state.tempo.ang);

    sx = m_Config.Eval( moR(PARTICLES_SCALEX), state.tempo.ang);
    sy = m_Config.Eval( moR(PARTICLES_SCALEY), state.tempo.ang);
    sz = m_Config.Eval( moR(PARTICLES_SCALEZ), state.tempo.ang);

    glTranslatef(   tx,
                    ty,
                    tz );

    //rotation
    glRotatef(  rz, 0.0, 0.0, 1.0 );
    glRotatef(  m_Config.Eval( moR(PARTICLES_ROTATEY), state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config.Eval( moR(PARTICLES_ROTATEX), state.tempo.ang), 1.0, 0.0, 0.0 );

	//scale
	glScalef(   sx,
              sy,
              sz);

    //blending
    SetBlending( (moBlendingModes) m_Config.Int( moR(PARTICLES_BLENDING) ) );
/*
    //set image
    moTexture* pImage = (moTexture*) m_Config[moR(PARTICLES_TEXTURE)].GetData()->Pointer();
*/
    //color
    SetColor( m_Config[moR(PARTICLES_COLOR)][MO_SELECTED], m_Config[moR(PARTICLES_ALPHA)][MO_SELECTED], state );

	moText Texto = m_Config.Text( moR(PARTICLES_TEXT) );

	float r1;
	r1 = 2.0 *((double)rand() /(double)(RAND_MAX+1));



    DrawParticlesSimple( tempogral, parentstate );

    //add conditions

    DrawTracker();


    if (pFont && drawing_features>2) {
        moText infod = moText("gravitational:")+FloatToStr(m_Physics.gravitational)+moText(" viscosity:")+FloatToStr(m_Physics.viscousdrag);
        pFont->Draw( 0.0, 0.0, infod );
    }

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}

void moEffectParticlesSimple::setUpLighting()
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

   float front_emission[4] = { 1, 1, 1, 0.0 };
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


MOboolean moEffectParticlesSimple::Finish()
{
    return PreFinish();
}

//====================
//
//		CUSTOM
//
//===================


void moEffectParticlesSimple::Interaction( moIODeviceManager *IODeviceManager ) {
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
				case 0:
					//Tx = valor;
					MODebug2->Push(IntToStr(valor));
					break;
				case 1:
					//Ty = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
					MODebug2->Push(IntToStr(valor));
					break;
				case 2:
					//Sx+=((float) valor / (float) 256.0);
					MODebug2->Push(IntToStr(valor));
					break;
				case 3:
					//Sy+=((float) valor / (float) 256.0);
					MODebug2->Push(IntToStr(valor));
					break;
			}
		temp = temp->next;
		}
	}

	///MIDI CONTROLS !!!!!
    moEvent* actual = IODeviceManager->GetEvents()->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	MOint tempval;
	while(actual!=NULL) {
		//solo nos interesan los del midi q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == MO_IODEVICE_MIDI) {

		    ///actual->reservedvalue1 corresponds to CC midi code : it works as a n index in m_Codes (has to be defined in param "code" in the config file...
		    ///actual->reservedvalue2 corresponds to VAL

			int CC = actual->reservedvalue1;
			int VAL = actual->reservedvalue2;

            switch (CC) {
                case 45:
                    if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {
                        this->Shot( "FOTOS", 1 );
                        this->ReInit();
                        MODebug2->Message(moText("## Midi Shot Triggered ##"));
                    } else {
                        this->ReInit();
                    }
                    break;
                case 81:
                    midi_red = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Red:")+FloatToStr(midi_red));
                    break;
                case 82:
                    midi_green = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Green:")+FloatToStr(midi_green));
                    break;
                case 83:
                    midi_blue = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Blue:")+FloatToStr(midi_blue));
                    break;

                case 84:
                    midi_maxage = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Maxage:")+FloatToStr(midi_maxage));
                    break;
                case 85:
                    midi_emitionperiod = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Emition Period:")+FloatToStr(midi_emitionperiod));
                    break;
                case 86:
                    midi_emitionrate = (float)VAL / (float) 127.0;
                    break;
                case 87:
                    midi_randomvelocity = (float)VAL / (float) 127.0;
                    break;

                case 88:
                    midi_randommotion = (float)VAL / (float) 127.0;
                    break;

                case 107:
                    if (VAL==0 || VAL==127) {
                        m_Config.PreConfPrev();
                    }
                    break;
                case 108:
                    if (VAL==0 || VAL==127) {
                         m_Config.PreConfNext();
                    }
                    break;


                default:
                    MODebug2->Push("device in psimple!!!");
                    MODebug2->Push(IntToStr(VAL));
                    break;
            }



			actual = actual->next;
		} else actual = actual->next;
	}

}

void moEffectParticlesSimple::Update( moEventList *p_eventlist ) {

    moMoldeoObject::Update( p_eventlist );

    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("Update")) {
            SelectScriptFunction("Update");
            //passing number of particles
            AddFunctionParam( (int) ( m_rows*m_cols ) );
            if (m_pTrackerData) {
                AddFunctionParam(m_pTrackerData->GetFeaturesCount());
                AddFunctionParam(m_pTrackerData->GetValidFeatures());
                //AddFunctionParam((int)m_pTrackerData->GetVideoFormat().m_Width);
                //AddFunctionParam((int)m_pTrackerData->GetVideoFormat().m_Height);
            } else {
                AddFunctionParam( -1 );
                AddFunctionParam( -1 );
            }

            if (!RunSelectedFunction(1)) {
                MODebug2->Error( GetLabelName() + moText(" moEffectParticlesSimple::  script > ") + moText("Update function not executed") );
            }
        }
    }







}

void moEffectParticlesSimple::RegisterFunctions()
{
    moMoldeoObject::RegisterFunctions();

    RegisterBaseFunction("GetParticle"); //0
    RegisterFunction("GetParticlePosition");//1
    RegisterFunction("GetParticleSize");//2
    RegisterFunction("GetParticleScale");//3
    RegisterFunction("GetParticleVelocity");//4
    RegisterFunction("GetParticleRotation");//5

    RegisterFunction("UpdateParticle");//6
    RegisterFunction("UpdateParticlePosition");//7
    RegisterFunction("UpdateParticleSize");//8
    RegisterFunction("UpdateParticleScale");//9
    RegisterFunction("UpdateParticleVelocity");//10
    RegisterFunction("UpdateParticleRotation");//11

	RegisterFunction("UpdateForce");//12

	RegisterFunction("Shot");//13
	RegisterFunction("ReInit");//14

    RegisterFunction("DrawPoint");//15
    RegisterFunction("GetParticleIntersection");//16

    ResetScriptCalling();
}

int moEffectParticlesSimple::ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber)
{
    switch (iFunctionNumber - m_iMethodBase)
    {
        case 0:
            ResetScriptCalling();
            return luaGetParticle(vm);
        case 1:
            ResetScriptCalling();
            return luaGetParticlePosition(vm);
        case 2:
            ResetScriptCalling();
            return luaGetParticleSize(vm);
        case 3:
            ResetScriptCalling();
            return luaGetParticleScale(vm);
        case 4:
            ResetScriptCalling();
            return luaGetParticleVelocity(vm);
        case 5:
            ResetScriptCalling();
            return luaGetParticleRotation(vm);


        case 6:
            ResetScriptCalling();
            return luaUpdateParticle(vm);
        case 7:
            ResetScriptCalling();
            return luaUpdateParticlePosition(vm);
        case 8:
            ResetScriptCalling();
            return luaUpdateParticleSize(vm);
        case 9:
            ResetScriptCalling();
            return luaUpdateParticleScale(vm);
        case 10:
            ResetScriptCalling();
            return luaUpdateParticleVelocity(vm);
        case 11:
            ResetScriptCalling();
            return luaUpdateParticleRotation(vm);
        case 12:
            ResetScriptCalling();
            return luaUpdateForce(vm);


        case 13:
            ResetScriptCalling();
            return luaShot(vm);

        case 14:
            ResetScriptCalling();
            return luaReInit(vm);

        case 15:
            ResetScriptCalling();
            return luaDrawPoint(vm);

        case 16:
            ResetScriptCalling();
            return luaGetParticleIntersection(vm);

        default:
            NextScriptCalling();
            return moMoldeoObject::ScriptCalling( vm, iFunctionNumber );
	}
}



int moEffectParticlesSimple::luaDrawPoint(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOfloat x = (MOfloat) lua_tonumber (state, 1);
    MOfloat y = (MOfloat) lua_tonumber (state, 2);

    glDisable(GL_TEXTURE_2D);

    glColor4f( 1.0, 1.0, 1.0, 1.0);
    glPointSize(4.0);

    glBegin(GL_POINTS);
        glVertex2f( x, y );
    glEnd();

    return 0;
}

int moEffectParticlesSimple::luaGetParticle(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {

        lua_pushnumber(state, (lua_Number) Par->Age.Duration() );
        lua_pushnumber(state, (lua_Number) Par->Visible );
        lua_pushnumber(state, (lua_Number) Par->Mass );

    }

    return 3;
}


int moEffectParticlesSimple::luaGetParticleRotation(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {

        lua_pushnumber(state, (lua_Number) Par->Age.Duration() );
        lua_pushnumber(state, (lua_Number) Par->Visible );
        lua_pushnumber(state, (lua_Number) Par->Mass );

    }

    return 3;
}

int moEffectParticlesSimple::luaGetParticlePosition(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    moVector3f Position;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Position = Par->Pos3d;
        lua_pushnumber(state, (lua_Number) Position.X() );
        lua_pushnumber(state, (lua_Number) Position.Y() );
        lua_pushnumber(state, (lua_Number) Position.Z() );

    } else {
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 3;
}

int moEffectParticlesSimple::luaGetParticleSize(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    moVector2f Size;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Size = Par->Size;
        lua_pushnumber(state, (lua_Number) Size.X() );
        lua_pushnumber(state, (lua_Number) Size.Y() );

    } else {
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 2;
}

int moEffectParticlesSimple::luaGetParticleScale(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    double Scale;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Scale = Par->Scale;
        lua_pushnumber(state, (lua_Number) Scale );
    } else {
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 1;
}

int moEffectParticlesSimple::luaGetParticleVelocity(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    moVector3f Velocity = Par->Velocity;

    if (Par) {

        lua_pushnumber(state, (lua_Number) Velocity.X() );
        lua_pushnumber(state, (lua_Number) Velocity.Y() );
        lua_pushnumber(state, (lua_Number) Velocity.Z() );

    }

    return 3;
}


int moEffectParticlesSimple::luaGetParticleIntersection(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOfloat x = (MOfloat) lua_tonumber (state, 1);
    MOfloat y = (MOfloat) lua_tonumber (state, 2);

    moParticlesSimple* Par;

    moVector3f Position;
    int index;
    bool a = false;

    float x1,x2,y1,y2;
    float xc,yc;
    float ux,uy;
    float vx,vy;

    float sizeu,sizev;

    index = -1;

    /// slow system
    ///iterate and calculate

    ///particle system in ortho mode....

    for( int i=0; i<m_ParticlesSimpleArray.Count(); i++ ) {

        Par = m_ParticlesSimpleArray[i];

        if (Par->Visible) {
            Position = Par->Pos3d;

            ///check for each particle if it intersects with x,y on screen

            xc = Par->Pos3d.X()*sx*cos(rz/180) + tx;
            yc = Par->Pos3d.Y()*sy*sin(rz/180) + ty;
            x1 = ( Par->Pos3d.X() - Par->Size.X()/2 ) * sx + tx;
            x2 = ( Par->Pos3d.X() + Par->Size.X()/2 ) * sx + tx;
            y1 = ( Par->Pos3d.Y() - Par->Size.Y()/2 ) * sy + ty;
            y2 = ( Par->Pos3d.Y() + Par->Size.Y()/2 ) * sy + ty;

            ux = ( x1 - xc );
            uy = ( y1 - yc );
            vx = ( x2 - xc );
            vy = ( y2 - yc );
            sizeu = Par->Size.X()/2 * sx;
            sizev = Par->Size.Y()/2 * sx;

            moVector2f feat( x, y );
            moVector2f u( ux, uy );
            moVector2f v( vx, vy );
            moVector2f featuv;
            featuv = moVector2f ( fabs( feat.Dot(u)), fabs( feat.Dot(v) ) );
            if ( featuv.X() < sizeu && featuv.Y() < sizev  ) {
                index = i;
            }
        }
    }

    lua_pushnumber(state, (lua_Number) index );

    return 1;
}


int moEffectParticlesSimple::luaUpdateParticle( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOint age = (MOint) lua_tonumber (state, 2);
    MOint visible = (MOint) lua_tonumber (state, 3);
    MOint mass = (MOint) lua_tonumber (state, 4);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Age.SetDuration( age);
        Par->Visible = visible;
        Par->Mass = mass;
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticlePosition( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat x = (MOfloat) lua_tonumber (state, 2);
    MOfloat y = (MOfloat) lua_tonumber (state, 3);
    MOfloat z = (MOfloat) lua_tonumber (state, 4);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Pos3d = moVector3f( x, y, z );
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleSize( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat x = (MOfloat) lua_tonumber (state, 2);
    MOfloat y = (MOfloat) lua_tonumber (state, 3);


    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Size = moVector2f( x, y );
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleScale( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat scale = (MOfloat) lua_tonumber (state, 2);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Scale = scale;
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleVelocity( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat x = (MOfloat) lua_tonumber (state, 2);
    MOfloat y = (MOfloat) lua_tonumber (state, 3);
    MOfloat z = (MOfloat) lua_tonumber (state, 4);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Velocity = moVector3f( x, y, z );
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleRotation( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat rx = (MOfloat) lua_tonumber (state, 2);
    MOfloat ry = (MOfloat) lua_tonumber (state, 3);
    MOfloat rz = (MOfloat) lua_tonumber (state, 4);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Rotation = moVector3f( rx, ry, rz );
    }

    return 0;

}


int moEffectParticlesSimple::luaUpdateForce( moLuaVirtualMachine& vm ) {


    return 0;

}



int moEffectParticlesSimple::luaShot( moLuaVirtualMachine& vm) {

    lua_State *state = (lua_State *) vm;

	if (lua_isboolean(state,1)) {
		bool vb = lua_toboolean(state,1);
		vb ? MODebug2->Push(moText("true")) : MODebug2->Push(moText("false"));
	} else {
		char *text = (char *) lua_tostring (state, 1);
		int shot_type = lua_tonumber (state, 2);
		int shot_file = lua_tonumber (state, 3);

		moText source_to_shot = moText(text);
		this->Shot( source_to_shot, shot_type, shot_file );
	}

    return 0;
}

int moEffectParticlesSimple::luaReInit(moLuaVirtualMachine& vm ) {

    lua_State *state = (lua_State *) vm;

    this->ReInit();

    return 0;
}
