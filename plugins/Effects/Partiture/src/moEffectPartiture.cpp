/*******************************************************************************

                                moEffectPartiture.cpp

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
  Andr� Colubri

*******************************************************************************/

#include "moEffectPartiture.h"


//========================
//  Factory
//========================

moPartitureFactory *m_EffectPartitureFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectPartitureFactory==NULL)
		m_EffectPartitureFactory = new moPartitureFactory();
	return(moEffectFactory*) m_EffectPartitureFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectPartitureFactory;
	m_EffectPartitureFactory = NULL;
}

moEffect*  moPartitureFactory::Create() {
	return new moEffectPartiture();
}

void moPartitureFactory::Destroy(moEffect* fx) {
	delete fx;
}



//========================
//  Efecto
//========================
moEffectPartiture::moEffectPartiture() {

	SetName("partiture");

	m_pFont = NULL;

}

moEffectPartiture::~moEffectPartiture() {
	Finish();
}

MOboolean moEffectPartiture::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( PARTITURE_ALPHA, moText("alpha") );
	moDefineParamIndex( PARTITURE_COLOR, moText("color") );
	moDefineParamIndex( PARTITURE_SYNC, moText("syncro") );
	moDefineParamIndex( PARTITURE_PHASE, moText("phase") );
	moDefineParamIndex( PARTITURE_TEXTURE, moText("texture") );
	moDefineParamIndex( PARTITURE_FONT, moText("font") );
	moDefineParamIndex( PARTITURE_SCROLLTIME, moText("scrolltime") );
    moDefineParamIndex( PARTITURE_SPECULAR, moText("specular") );
	moDefineParamIndex( PARTITURE_DIFFUSE, moText("diffuse") );
	moDefineParamIndex( PARTITURE_AMBIENT, moText("ambient") );
	moDefineParamIndex( PARTITURE_PENTAGRAMABACK, moText("pentragramaback") );
	moDefineParamIndex( PARTITURE_PENTAGRAMAFRONT, moText("pentagramafront") );
	moDefineParamIndex( PARTITURE_PENTAGRAMATEXTURE, moText("pentragramatexture") );
	moDefineParamIndex( PARTITURE_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( PARTITURE_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( PARTITURE_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( PARTITURE_ROTATEX, moText("rotatex") );
	moDefineParamIndex( PARTITURE_ROTATEY, moText("rotatey") );
	moDefineParamIndex( PARTITURE_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( PARTITURE_SCALEX, moText("scalex") );
	moDefineParamIndex( PARTITURE_SCALEY, moText("scaley") );
	moDefineParamIndex( PARTITURE_SCALEZ, moText("scalez") );
	moDefineParamIndex( PARTITURE_EYEX, moText("eyex") );
	moDefineParamIndex( PARTITURE_EYEY, moText("eyey") );
	moDefineParamIndex( PARTITURE_EYEZ, moText("eyez") );
	moDefineParamIndex( PARTITURE_VIEWX, moText("viewx") );
	moDefineParamIndex( PARTITURE_VIEWY, moText("viewy") );
	moDefineParamIndex( PARTITURE_VIEWZ, moText("viewz") );
	moDefineParamIndex( PARTITURE_LIGHTX, moText("lightx") );
	moDefineParamIndex( PARTITURE_LIGHTY, moText("lighty") );
	moDefineParamIndex( PARTITURE_LIGHTZ, moText("lightz") );
	moDefineParamIndex( PARTITURE_INLET, moText("inlet") );
	moDefineParamIndex( PARTITURE_OUTLET, moText("outlet") );

	Tx = 1; Ty = 1; Tz = 1;
	Sx = 1; Sy = 1; Sz = 1;

	g_ViewMode = GL_TRIANGLES;

    m_pFont = NULL;

    for( int ii=0; ii<PARTITURE_MAXTRACKS ; ii++ ) {

        CTracks[ii].Init( GetConfig() );
        CTracks[ii].SetMode( DYNAMIC_MODE, 300 );

    }

	return true;
}

void moEffectPartiture::Draw( moTempo* tempogral,moEffectState* parentstate)
{

    PreDraw( tempogral, parentstate);


    //m_pResourceManager->GetGLMan()->SetOrthographicView( m_pResourceManager->GetRenderMan()->ScreenWidth(), m_pResourceManager->GetRenderMan()->ScreenHeight() );
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	m_pResourceManager->GetGLMan()->SetPerspectiveView( m_pResourceManager->GetRenderMan()->ScreenWidth(), m_pResourceManager->GetRenderMan()->ScreenHeight() );

    glMatrixMode(GL_PROJECTION);


	gluLookAt(		m_Config[moR(PARTITURE_EYEX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(PARTITURE_EYEY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(PARTITURE_EYEZ)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(PARTITURE_VIEWX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(PARTITURE_VIEWY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(PARTITURE_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang),
					0, 1, 0);

    //MODebug2->Push( moText("viewz:") + FloatToStr( m_Config[moR(PARTITURE_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang) ) );
	//m_pResourceManager->GetGuiMan()->DisplayInfoWindow( 20 , 20, 200, 20,  Textos);

	m_pFont = m_Config[moR(PARTITURE_FONT)][MO_SELECTED][0].GetData()->Font();
	//m_pFont = m_pResourceManager->GetFontMan()->GetFont(0);

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    float hPent, yPent;

    hPent = (float) m_pResourceManager->GetRenderMan()->ScreenHeight() / 8.0;
    yPent = (float) m_pResourceManager->GetRenderMan()->ScreenHeight() / 4.0 - (float) hPent ;

    MOulong scrolltime = m_Config[moR(PARTITURE_SCROLLTIME)][MO_SELECTED][0].Int();

    for(int t=0; t<2; t++) {

        glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
        glPushMatrix(); // Store The Modelview Matrix
        glLoadIdentity();   // Reset The View

        glTranslatef(   m_Config[moR(PARTITURE_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
                        m_Config[moR(PARTITURE_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
                        m_Config[moR(PARTITURE_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
        glRotatef(  m_Config[moR(PARTITURE_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
        glRotatef(  m_Config[moR(PARTITURE_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
        glRotatef(  m_Config[moR(PARTITURE_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
        glScalef(   m_Config[moR(PARTITURE_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                    m_Config[moR(PARTITURE_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                    m_Config[moR(PARTITURE_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang));

        //Dibujamos las notas.....

        CTracks[t].Draw( 0,
                    yPent+t*hPent*2.5,
                    m_pResourceManager->GetRenderMan()->ScreenWidth(),
                    hPent,
                    state,
                    scrolltime );

    }

    // Dejamos todo como lo encontramos //
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}

void moEffectPartiture::Update( moEventList* p_EventList ) {

    //MODebug->Push( moText( "Updateing partiture" ));

    //reseteamos los inlets, queremos informacion FRESCA!!!!!!
    for(int i=0; i<GetInlets()->Count();i++) {
        moInlet* pInlet = GetInlets()->Get(i);
        if (pInlet) {
            pInlet->Update(false);
        }
    }

    moMoldeoObject::Update(p_EventList);

    moInlet* pMessages = GetInlets()->Get(0);

    int header = 0;
    MOulong timecode = 0;
    int track = 0;
    int note = 0;
    int velocity = 0;
    moText dynamic = "";
    int modulator = 0;
    int tiempo = 0;

    if ( pMessages->Updated() ) {

        moDataMessages* pMsgs = pMessages->GetData()->Messages();

        MODebug2->Push( moText("Partiture: message received:") + IntToStr( pMsgs->Count()) );

        for( int i=0; i<pMsgs->Count(); i++ ) {

            moDataMessage&  message( pMsgs->Get(i) );

            track = message.Get(0).GetData()->Int();
            header = message.Get(1).GetData()->Int();
            note = message.Get(2).GetData()->Int();
            velocity = message.Get(3).GetData()->Int();
            dynamic = message.Get(4).GetData()->Text();
            modulator = message.Get(5).GetData()->Int();
            tiempo = message.Get(6).GetData()->Int();


            //check the first one:
                //if they are less than then:
            //...........
            int maxnotes = 200;

            /*
            //borra las notas si llegamos al limite de 200
            if ( tracks[track].Count() > maxnotes ) {
                    moCNote* pCNote = tracks[track].Get(0);
                    if (pCNote) {
                        tracks[track].Remove(0);
                        delete pCNote;
                    }
            }
            */

            /*
            if ( CTracks[track].Notes. > maxnotes ) {
                    moCNote* pCNote = CTracks[track].Get(0);
                    if (pCNote) {
                        tracks[track].Remove(0);
                        delete pCNote;
                    }
            }
            */

            timecode = this->state.tempo.ticks;

            //1.0: right top most
            moCNote* pCNote = new moCNote( 1.0, timecode, track, header, note, velocity, dynamic, modulator, tiempo );

            if ( 0<=track && track<PARTITURE_MAXTRACKS) {
                if (pCNote) {
                    CTracks[track].AddNote( pCNote );
                    MODebug2->Push( moText("Header:") + IntToStr( pCNote->m_header) +
                    moText("Track:") + IntToStr( pCNote->m_track) +
                    moText("Note:") + IntToStr( pCNote->m_note) +
                    moText("Velocity:") + IntToStr( pCNote->m_velocity) +
                    moText("Dynamic:") + moText( pCNote->m_dynamic) +
                    moText("Modulator:") + IntToStr( pCNote->m_modulador) +
                    moText("Tiempo:") + IntToStr( pCNote->m_tiempo) );
                }
            }

        }
    }

}


MOboolean moEffectPartiture::Finish()
{
	Models3d.Finish();

    return PreFinish();
}

void moEffectPartiture::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp,*aux;
	MOint did,cid,state,e,valor,estaux,elaux;
	moText txt;

	if (devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);
			if (state)
			switch(i) {
				case MO_PARTITURE_TRANSLATE_X:
					Tx = valor;
					txt = FloatToStr(Tx);
					MODebug->Push(moText("Tx: ") + (moText)txt);
					break;
				case MO_PARTITURE_TRANSLATE_Y:
					Ty = valor;
					txt = FloatToStr(Ty);
					MODebug->Push(moText("Ty: ") + (moText)txt);
					break;
				case MO_PARTITURE_TRANSLATE_Z:
					Tz = valor;
					txt = FloatToStr(Tz);
					MODebug->Push(moText("Tz: ") + (moText)txt);
					break;
				case MO_PARTITURE_SCALE_X:
					Sx+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sx);
					MODebug->Push(moText("Sx: ") + (moText) txt);
					break;
				case MO_PARTITURE_SCALE_Y:
					Sy+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sy);
					MODebug->Push(moText("Sy: ") + (moText)txt);
				case MO_PARTITURE_SCALE_Z:
					Sz+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sz);
					MODebug->Push(moText("Sz: ") + (moText)txt);
					break;
			}
		temp = temp->next;
		}
	}

}


moConfigDefinition *
moEffectPartiture::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, PARTITURE_TEXTURE );
    p_configdefinition->Add( moText("font"), MO_PARAM_FONT, PARTITURE_FONT, moValue( "fonts/arial.ttf", "TXT") );
	p_configdefinition->Add( moText("scrolltime"), MO_PARAM_NUMERIC, PARTITURE_SCROLLTIME, moValue("4000","NUM") );
	p_configdefinition->Add( moText("specular"), MO_PARAM_COLOR, PARTITURE_SPECULAR );
	p_configdefinition->Add( moText("diffuse"), MO_PARAM_COLOR, PARTITURE_DIFFUSE );
	p_configdefinition->Add( moText("ambient"), MO_PARAM_COLOR, PARTITURE_AMBIENT );
	p_configdefinition->Add( moText("pentagramaback"), MO_PARAM_COLOR, PARTITURE_PENTAGRAMABACK );
    p_configdefinition->Add( moText("pentagramafront"), MO_PARAM_COLOR, PARTITURE_PENTAGRAMAFRONT );
    p_configdefinition->Add( moText("pentagramatexture"), MO_PARAM_TEXTURE, PARTITURE_PENTAGRAMATEXTURE );

	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, PARTITURE_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, PARTITURE_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, PARTITURE_TRANSLATEZ );

	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, PARTITURE_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, PARTITURE_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, PARTITURE_ROTATEZ );

	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, PARTITURE_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, PARTITURE_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, PARTITURE_SCALEZ );

	p_configdefinition->Add( moText("eyex"), MO_PARAM_FUNCTION, PARTITURE_EYEX );
	p_configdefinition->Add( moText("eyey"), MO_PARAM_FUNCTION, PARTITURE_EYEY );
	p_configdefinition->Add( moText("eyez"), MO_PARAM_FUNCTION, PARTITURE_EYEZ );
	p_configdefinition->Add( moText("viewx"), MO_PARAM_FUNCTION, PARTITURE_VIEWX );
	p_configdefinition->Add( moText("viewy"), MO_PARAM_FUNCTION, PARTITURE_VIEWY );
	p_configdefinition->Add( moText("viewz"), MO_PARAM_FUNCTION, PARTITURE_VIEWZ );
	p_configdefinition->Add( moText("lightx"), MO_PARAM_FUNCTION, PARTITURE_LIGHTX );
	p_configdefinition->Add( moText("lighty"), MO_PARAM_FUNCTION, PARTITURE_LIGHTY );
	p_configdefinition->Add( moText("lightz"), MO_PARAM_FUNCTION, PARTITURE_LIGHTZ );
	return p_configdefinition;
}
