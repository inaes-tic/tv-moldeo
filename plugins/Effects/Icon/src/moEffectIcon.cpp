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
  Andr�s Colubri

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
	moDefineParamIndex( ICON_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( ICON_ROTATEX, moText("rotatex") );
	moDefineParamIndex( ICON_ROTATEY, moText("rotatey") );
	moDefineParamIndex( ICON_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( ICON_SCALEX, moText("scalex") );
	moDefineParamIndex( ICON_SCALEY, moText("scaley") );
	moDefineParamIndex( ICON_SCALEZ, moText("scalez") );
	moDefineParamIndex( ICON_INLET, moText("inlet") );
	moDefineParamIndex( ICON_OUTLET, moText("outlet") );

    Tx = Ty = Tz = Rx = Ry = Rz = 0.0;
	Sx = Sy = Sz = 1.0;

	return true;
}

#define KLT_TRACKED           0
#define KLT_NOT_FOUND        -1
#define KLT_SMALL_DET        -2
#define KLT_MAX_ITERATIONS   -3
#define KLT_OOB              -4
#define KLT_LARGE_RESIDUE    -5

void moEffectIcon::Draw( moTempo* tempogral, moEffectState* parentstate )
{
    MOint indeximage;
	MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    PreDraw( tempogral, parentstate);


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
	glTranslatef(  ( m_Config[moR(ICON_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang)+Tx )*w,
                   ( m_Config[moR(ICON_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang)+Ty )*h,
					m_Config[moR(ICON_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang)+Tz);

	glRotatef(  m_Config[moR(ICON_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(ICON_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(ICON_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
	glScalef(   m_Config[moR(ICON_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang)*Sx,
                m_Config[moR(ICON_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang)*Sy,
                m_Config[moR(ICON_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang)*Sz);

    SetColor( m_Config[moR(ICON_COLOR)][MO_SELECTED], m_Config[moR(ICON_ALPHA)][MO_SELECTED], state );

    SetBlending( (moBlendingModes) m_Config[moR(ICON_BLENDING)][MO_SELECTED][0].Int() );

    moTexture* pImage = (moTexture*) m_Config[moR(ICON_TEXTURE)].GetData()->Pointer();

    glBindTexture( GL_TEXTURE_2D, m_Config[moR(ICON_TEXTURE)].GetData()->GetGLId(&state.tempo) );

    PosTextX0 = 0.0;
	PosTextX1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordS() :  1.0 );
    PosTextY0 = 0.0;
    PosTextY1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordT() :  1.0 );

	//ancho = (int)m_Config[ moR(ICON_WIDTH) ].GetData()->Fun()->Eval(state.tempo.ang)* (float)(w/800.0);
	//alto = (int)m_Config[ moR(ICON_HEIGHT) ].GetData()->Fun()->Eval(state.tempo.ang)* (float)(h/600.0);

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


    moTrackerSystemData* m_pTrackerData = NULL;
    bool m_bTrackerInit = false;
	//Procesar Inlets

	for(int i=0; i<m_Inlets.Count(); i++) {
		moInlet* pInlet = m_Inlets[i];
		if (pInlet->Updated() && ( pInlet->GetConnectorLabelName()==moText("TRACKERKLT") || pInlet->GetConnectorLabelName()==moText("TRACKERGPUKLT") || pInlet->GetConnectorLabelName()==moText("TRACKERGPUKLT2")) ) {

			m_pTrackerData = (moTrackerSystemData *)pInlet->GetData()->Pointer();
			if (m_pTrackerData && !m_bTrackerInit) {
				m_bTrackerInit = true;

				//SelectScriptFunction("Reset");
				//RunSelectedFunction();

				//MODebug2->Push(IntToStr(TrackerId));

				//MODebug2->Push(moText("Receiving:") + IntToStr(m_pTrackerData->GetFeaturesCount()) );
				if (m_pTrackerData->GetFeaturesCount()>0) {
                    int tw = m_pTrackerData->GetVideoFormat().m_Width;
                    int th = m_pTrackerData->GetVideoFormat().m_Height;
                    //MODebug2->Push(moText("vformat:")+IntToStr(tw)+moText("th")+IntToStr(th));

                    for (i = 0; i < m_pTrackerData->GetFeaturesCount(); i++)
                    {

                        moTrackerFeature* pF = m_pTrackerData->GetFeature(i);

                        //if (pF && pF->valid) {

                        float x = (pF->x / (float)tw) - 0.5;
                        float y = -(pF->y / (float)th) + 0.5;
                        float tr_x = (pF->tr_x / (float)tw) - 0.5;
                        float tr_y = -(pF->tr_y / (float)th) + 0.5;
                        float v_x = (pF->v_x / (float)tw);
                        float v_y = -(pF->v_y / (float)th);
                        float vel = sqrtf( v_x*v_x+v_y*v_y );
                        int v = pF->val;



                        //MODebug2->Log(moText("    x:")+FloatToStr(pF->x) + moText(" y:")+FloatToStr(pF->y) );

                        glBindTexture(GL_TEXTURE_2D,0);
                        glColor4f(1.0, 0.0, 0.0, 1.0);

                        if (v >= KLT_TRACKED) glColor4f(0.0, 1.0, 0.0, 1.0);
                        else if (v == KLT_NOT_FOUND) glColor4f(1.0, 0.0, 1.0, 1.0);
                        else if (v == KLT_SMALL_DET) glColor4f(1.0, 0.0, 1.0, 1.0);
                        else if (v == KLT_MAX_ITERATIONS) glColor4f(1.0, 0.0, 1.0, 1.0);
                        else if (v == KLT_OOB) glColor4f(1.0, 0.0, 1.0, 1.0);
                        else if (v == KLT_LARGE_RESIDUE) glColor4f(1.0, 0.0, 1.0, 1.0);


                        if ( pF->valid ) {

                            glBegin(GL_QUADS);
                                glVertex2f((tr_x - 0.008)*w, (tr_y - 0.008)*h);
                                glVertex2f((tr_x - 0.008)*w, (tr_y + 0.008)*h);
                                glVertex2f((tr_x + 0.008)*w, (tr_y + 0.008)*h);
                                glVertex2f((tr_x + 0.008)*w, (tr_y - 0.008)*h);
                            glEnd();

                            glBegin(GL_QUADS);
                                glVertex2f((x - 0.008)*w, (y - 0.008)*h);
                                glVertex2f((x - 0.008)*w, (y + 0.008)*h);
                                glVertex2f((x + 0.008)*w, (y + 0.008)*h);
                                glVertex2f((x + 0.008)*w, (y - 0.008)*h);
                            glEnd();

                            glDisable(GL_TEXTURE_2D);
                            glColor4f(1.0, 1.0, 1.0, 1.0);
                            glBegin(GL_LINES);
                                glVertex2f( x*w, y*h);
                                glVertex2f( tr_x*w, tr_y*h);
                            glEnd();

                            if ( vel > 0.001 ) {
                                glDisable(GL_TEXTURE_2D);
                                glColor4f(0.0, 0.0, 1.0, 1.0);
                                //glPointSize((GLfloat)10);
                                glLineWidth((GLfloat)10.0);
                                glBegin(GL_LINES);
                                    glVertex2f( x*w, y*h);
                                    glVertex2f( x*w+v_x*w, y*h+v_y*h);
                                glEnd();
                            }


                        }


                    }

                }

			}
		}
	}

}

MOboolean moEffectIcon::Finish()
{
    return PreFinish();
}

void moEffectIcon::Interaction( moIODeviceManager *IODeviceManager ) {

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
				case MO_ICON_TRANSLATE_X:
					Tx = valor;
					MODebug->Push(IntToStr(valor));
					break;
				case MO_ICON_TRANSLATE_Y:
					Ty = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
					MODebug->Push(IntToStr(valor));
					break;
				case MO_ICON_SCALE_X:
					Sx+=((float) valor / (float) 256.0);
					MODebug->Push(IntToStr(valor));
					break;
				case MO_ICON_SCALE_Y:
					Sy+=((float) valor / (float) 256.0);
					MODebug->Push(IntToStr(valor));
					break;
			}
		temp = temp->next;
		}
	}

}

moConfigDefinition *
moEffectIcon::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, ICON_TEXTURE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, ICON_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, ICON_WIDTH, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, ICON_HEIGHT, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, ICON_TRANSLATEX, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, ICON_TRANSLATEY, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, ICON_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, ICON_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, ICON_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, ICON_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, ICON_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, ICON_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, ICON_SCALEZ );
	return p_configdefinition;
}



void
moEffectIcon::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);


}
