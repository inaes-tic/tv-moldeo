/*******************************************************************************

                                moEffectImage.cpp

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

#include "moEffectImage.h"

//========================
//  Factory
//========================

moEffectImageFactory *m_EffectImageFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectImageFactory==NULL)
		m_EffectImageFactory = new moEffectImageFactory();
	return(moEffectFactory*) m_EffectImageFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectImageFactory;
	m_EffectImageFactory = NULL;
}

moEffect*  moEffectImageFactory::Create() {
	return new moEffectImage();
}

void moEffectImageFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectImage::moEffectImage() {
	SetName("image");
}

moEffectImage::~moEffectImage() {
	Finish();
}

moConfigDefinition *
moEffectImage::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, IMAGE_TEXTURE, moValue( "Default", "TXT").Ref() );
	p_configdefinition->Add( moText("filters"), MO_PARAM_FILTER, IMAGE_FILTER );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, IMAGE_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("pos_text_x"), MO_PARAM_FUNCTION, IMAGE_POSTEXX, moValue( "0.0", "FUNCTION" ).Ref() );
	p_configdefinition->Add( moText("pos_text_y"), MO_PARAM_FUNCTION, IMAGE_POSTEXY, moValue( "0.0", "FUNCTION" ).Ref() );
	p_configdefinition->Add( moText("anc_text_x"), MO_PARAM_FUNCTION, IMAGE_ANCTEXX, moValue( "1.0", "FUNCTION" ).Ref() );
	p_configdefinition->Add( moText("alt_text_y"), MO_PARAM_FUNCTION, IMAGE_ALTTEXY, moValue( "1.0", "FUNCTION" ).Ref() );
	p_configdefinition->Add( moText("pos_cuad_x"), MO_PARAM_FUNCTION, IMAGE_POSCUADX, moValue( "0.0", "FUNCTION" ).Ref() );
	p_configdefinition->Add( moText("pos_cuad_y"), MO_PARAM_FUNCTION, IMAGE_POSCUADY, moValue( "0.0", "FUNCTION" ).Ref() );
	p_configdefinition->Add( moText("anc_cuad_x"), MO_PARAM_FUNCTION, IMAGE_ANCCUADX, moValue( "1.0", "FUNCTION" ).Ref() );
	p_configdefinition->Add( moText("alt_cuad_y"), MO_PARAM_FUNCTION, IMAGE_ALTCUADY, moValue( "1.0", "FUNCTION" ).Ref() );
	return p_configdefinition;
}

MOboolean
moEffectImage::Init() {

    if (!PreInit()) return false;

    // Hacer la inicializacion de este efecto en particular.
	moDefineParamIndex( IMAGE_ALPHA, moText("alpha") );
	moDefineParamIndex( IMAGE_COLOR, moText("color") );
	moDefineParamIndex( IMAGE_SYNC, moText("syncro") );
	moDefineParamIndex( IMAGE_PHASE, moText("phase") );
	moDefineParamIndex( IMAGE_BLENDING, moText("blending") );
	moDefineParamIndex( IMAGE_TEXTURE, moText("texture") );
	moDefineParamIndex( IMAGE_FILTER, moText("filters") );
	moDefineParamIndex( IMAGE_POSTEXX, moText("pos_text_x") );
	moDefineParamIndex( IMAGE_POSTEXY, moText("pos_text_y") );
	moDefineParamIndex( IMAGE_ANCTEXX, moText("anc_text_x") );
	moDefineParamIndex( IMAGE_ALTTEXY, moText("alt_text_y") );
	moDefineParamIndex( IMAGE_POSCUADX, moText("pos_cuad_x") );
	moDefineParamIndex( IMAGE_POSCUADY, moText("pos_cuad_y") );
	moDefineParamIndex( IMAGE_ANCCUADX, moText("anc_cuad_x") );
	moDefineParamIndex( IMAGE_ALTCUADY, moText("alt_cuad_y") );
	moDefineParamIndex( IMAGE_INLET, moText("inlet") );
	moDefineParamIndex( IMAGE_OUTLET, moText("outlet") );
/*
	int idx = m_Config.GetParamIndex(moText("filters"));
	if (MO_PARAM_NOT_FOUND != idx)
	{
		Filters.Init(&m_Config, idx, m_pResourceManager->GetGLMan(), m_pResourceManager->GetFBMan(), m_pResourceManager->GetShaderMan(), m_pResourceManager->GetTextureMan(), m_pResourceManager->GetRenderMan() );
		FilterParams = MOEmptyTexFilterParam;
		using_filter = true;
	}
	else using_filter = false;
*/
	/*
    //TRACKER
	m_bTrackerInit = false;
	m_pTrackerData = NULL;
	m_pTrackerGpuData = NULL;
	fcount = 0;
*/

	return true;
}

void moEffectImage::Draw( moTempo* tempogral,moEffectState* parentstate)
{

    PreDraw( tempogral, parentstate);

    MOint Blending;
	MOdouble PosTextX,  AncTextX,  PosTextY,  AltTextY;
    MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    MOdouble PosCuadX,  AncCuadX,  PosCuadY,  AltCuadY;
    MOdouble PosCuadX0, PosCuadX1, PosCuadY0, PosCuadY1;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();


	/*if (using_filter)
		Filters.Apply(&state.tempo, FilterParams);
*/
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    moTexture* pImage = (moTexture*) m_Config[moR(IMAGE_TEXTURE)].GetData()->Pointer();

	//DIBUJAR
	glEnable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing

	SetBlending( (moBlendingModes) m_Config[moR(IMAGE_BLENDING)][MO_SELECTED][0].Int() );

	SetColor( MOValue( IMAGE_COLOR, MO_SELECTED ), MOValue( IMAGE_ALPHA, MO_SELECTED ), state  );

	//source: GL_ZERO, GL_ONE, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, and GL_SRC_ALPHA_SATURATE
	//destination: GL_ZERO, GL_ONE, GL_SCR_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR, GL_DST_ALPHA, and GL_ONE_MINUS_DST_ALPHA.


	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,1.0,0,1.0,-1,1);  							// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glBindTexture( GL_TEXTURE_2D, m_Config[moR(IMAGE_TEXTURE)].GetData()->GetGLId(&state.tempo, 1, &FilterParams ) );


	PosTextX = m_Config[moR(IMAGE_POSTEXX)].GetData()->Fun()->Eval(state.tempo.ang);
    AncTextX = m_Config[moR(IMAGE_ANCTEXX)].GetData()->Fun()->Eval(state.tempo.ang);
	PosTextY = m_Config[moR(IMAGE_POSTEXY)].GetData()->Fun()->Eval(state.tempo.ang);
    AltTextY = m_Config[moR(IMAGE_ALTTEXY)].GetData()->Fun()->Eval(state.tempo.ang);

	PosCuadX = m_Config[moR(IMAGE_POSCUADX)].GetData()->Fun()->Eval(state.tempo.ang);
    AncCuadX = m_Config[moR(IMAGE_ANCCUADX)].GetData()->Fun()->Eval(state.tempo.ang);
	PosCuadY = m_Config[moR(IMAGE_POSCUADY)].GetData()->Fun()->Eval(state.tempo.ang);
    AltCuadY = m_Config[moR(IMAGE_ALTCUADY)].GetData()->Fun()->Eval(state.tempo.ang);

	if (pImage) {
		PosTextX0 = PosTextX * pImage->GetMaxCoordS();
		PosTextX1 =(PosTextX + AncTextX)* pImage->GetMaxCoordS();
		PosTextY0 =(1 - PosTextY)*pImage->GetMaxCoordT();
		PosTextY1 =(1 - PosTextY - AltTextY)*pImage->GetMaxCoordT();
	} else {
		PosTextX0 = PosTextX;
		PosTextX1 =(PosTextX + AncTextX);
		PosTextY0 =(1 - PosTextY);
		PosTextY1 =(1 - PosTextY - AltTextY);
	}
	PosCuadX0 = PosCuadX;
	PosCuadX1 = PosCuadX + AncCuadX;
	PosCuadY1 = PosCuadY;
	PosCuadY0 = PosCuadY + AltCuadY;

	glBegin(GL_QUADS);
		glTexCoord2f( PosTextX0, PosTextY1);
		glVertex2f ( PosCuadX0, PosCuadY0);

		glTexCoord2f( PosTextX1, PosTextY1);
		glVertex2f ( PosCuadX1, PosCuadY0);

        glTexCoord2f( PosTextX1, PosTextY0);
		glVertex2f ( PosCuadX1, PosCuadY1);

		glTexCoord2f( PosTextX0, PosTextY0);
		glVertex2f ( PosCuadX0, PosCuadY1);
	glEnd();

	//if (m_bTrackerInit)
	//	DrawTrackerFeatures();


    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

}

void moEffectImage::Interaction( moIODeviceManager *IODeviceManager ) {
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
				case MO_FILTER_PAR1:
					FilterParams.par_flt1 = ((float) valor / (float) 800.0);
					//MODebug->Push("par1: " + FloatToStr(valor));
					break;

			}
		temp = temp->next;
		}
	}
}


MOboolean moEffectImage::Finish()
{
    return PreFinish();
}

/*
void
moEffectImage::Update( moEventList *p_EventList ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(p_EventList);

	moEvent* actual,*tmp;

	actual = p_EventList->First;

	while(actual!=NULL) {
		tmp = actual->next;
		if (actual->deviceid == MO_IODEVICE_TRACKER) {
			NF = actual->devicecode;
			TrackerX = actual->reservedvalue0;
			TrackerY = actual->reservedvalue1;
			Variance = actual->reservedvalue2;
			MODebug->Push( moText("NF:")+IntToStr(NF)+moText("X:")+IntToStr(TrackerX)+moText("Y:")+IntToStr(TrackerY)+moText("V:")+IntToStr(Variance));
		}
		actual = tmp;
	}


	//Procesar Inlets

	for(int i=0; i<m_Inlets.Count(); i++) {
		moInlet* pInlet = m_Inlets[i];

		if (pInlet->Updated() && pInlet->GetConnectorLabelName()==moText("TRACKERKLT")) {

			m_pTrackerData = (moTrackerKLTSystemData *)pInlet->GetData()->Pointer();
			if (m_pTrackerData && !m_bTrackerInit) {
				m_bTrackerInit = true;
			}
		} else if (pInlet->Updated() && pInlet->GetConnectorLabelName()==moText("TRACKERGPUKLT")) {
			m_pTrackerGpuData = (moTrackerGpuKLTSystemData *)pInlet->GetData()->Pointer();
			if (m_pTrackerGpuData && !m_bTrackerInit) {
				m_bTrackerInit = true;
			}
		}
	}

}
*/


/*
void moEffectImage::DrawTrackerFeatures()
{
	int i, j, tw, th, v, v2;
    float x, y;
	float x2, y2, d;


	if (m_pTrackerData) {
		tw = m_pTrackerData->m_VideoFormat.m_Width;
		th = m_pTrackerData->m_VideoFormat.m_Height;


		//SelectScriptFunction("Draw");
		//AddFunctionParam(tw);
		//AddFunctionParam(th);
		//RunSelectedFunction();


		for (i = 0; i < m_pTrackerData->m_NFeatures; i++)
		{
			x = 800.0 * m_pTrackerData->m_FeatureList->feature[i]->x / tw;
			y = 600.0 * m_pTrackerData->m_FeatureList->feature[i]->y / th;
			v = m_pTrackerData->m_FeatureList->feature[i]->val;
			glDisable(GL_TEXTURE_2D);


			if (v == KLT_TRACKED) glColor4f(0.5, 1.0, 0.5, 1.0);
			//else if (v == KLT_NOT_FOUND) glColor4f(1.0, 1.0, 1.0, 1.0);
			//else if (v == KLT_SMALL_DET) glColor4f(1.0, 0.0, 0.0, 1.0);
			//else if (v == KLT_MAX_ITERATIONS) glColor4f(0.0, 1.0, 0.0, 1.0);
			//else if (v == KLT_OOB) glColor4f(0.0, 0.0, 1.0, 1.0);
			//else if (v == KLT_LARGE_RESIDUE) glColor4f(1.0, 1.0, 0.0, 1.0);

			if (v == KLT_TRACKED)
			glBegin(GL_QUADS);
				glVertex2f(x - 5, y - 5);
				glVertex2f(x - 5, y + 5);
				glVertex2f(x + 5, y + 5);
				glVertex2f(x + 5, y - 5);
			glEnd();

			fcount++;
			if (m_pTrackerData->m_FeatureTable)
			for(int z=0; z<m_pTrackerData->m_FeatureTable->nFeatures; z++) {
				for(int k=fcount; (fcount+5)>k && k<m_pTrackerData->m_FeatureTable->nFrames; k++) {
					x = 800.0 * m_pTrackerData->m_FeatureTable->feature[z][k]->x / tw;
					y = 600.0 * m_pTrackerData->m_FeatureTable->feature[z][k]->y / th;
					v = m_pTrackerData->m_FeatureTable->feature[z][k]->val;
					//if (v==KLT_TRACKED) {
					glColor4f(0.0, 0.0, 1.0, 1.0);
						glBegin(GL_QUADS);
							glVertex2f(x - 5, y - 5);
							glVertex2f(x - 5, y + 5);
							glVertex2f(x + 5, y + 5);
							glVertex2f(x + 5, y - 5);
						glEnd();
					//}
				}
			}
		}
	} else if (m_pTrackerGpuData) {
		tw = m_pTrackerGpuData->m_VideoFormat.m_Width;
		th = m_pTrackerGpuData->m_VideoFormat.m_Height;


		//SelectScriptFunction("Draw");
		//AddFunctionParam(tw);
		//AddFunctionParam(th);
		//RunSelectedFunction();


		for (i = 0; i < m_pTrackerGpuData->m_NFeatures; i++)
		{
			x = 800.0 * m_pTrackerGpuData->m_FeatureList->_list[i]->x / tw;
			y = 600.0 * m_pTrackerGpuData->m_FeatureList->_list[i]->y / th;
			v = m_pTrackerGpuData->m_FeatureList->_list[i]->valid;

			glColor4f(1.0, 1.0, 1.0, 1.0);
			//if (v == KLT_TRACKED) glColor4f(0.5, 1.0, 0.5, 1.0);
			//else if (v == KLT_NOT_FOUND) glColor4f(0.0, 0.0, 0.0, 1.0);
			//else if (v == KLT_SMALL_DET) glColor4f(1.0, 0.0, 0.0, 1.0);
			//else if (v == KLT_MAX_ITERATIONS) glColor4f(0.0, 1.0, 0.0, 1.0);
			//else if (v == KLT_OOB) glColor4f(0.0, 0.0, 1.0, 1.0);
			//else if (v == KLT_LARGE_RESIDUE) glColor4f(1.0, 1.0, 0.0, 1.0);
			if ((bool)v==true) glColor4f(1.0, 1.0, 1.0, 1.0);
			else glColor4f(0.8, 0.0, 0.0, 1.0);
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
				glVertex2f(x - 5, y - 5);
				glVertex2f(x - 5, y + 5);
				glVertex2f(x + 5, y + 5);
				glVertex2f(x + 5, y - 5);
			glEnd();

			//if ( m_pTrackerGpuData->m_FeatureList->_list[i]->valid) {

			//	std::vector<Point2D*> pTracks = m_pTrackerGpuData->m_FeatureList->_list[i]->track;
			//	float xa,ya;
			//	x2 = x;
			//	y2 = y;
				//for (j = 0; j <pTracks.size(); j++)
			//	{
			//		xa = x2;
			//		ya = y2;
			//		x2 = 800.0 * m_pTrackerGpuData->m_FeatureList->_list[i]->tr_x / (float)tw;
			//		y2 = 600.0 * m_pTrackerGpuData->m_FeatureList->_list[i]->tr_y / (float)th;
			//		glLineWidth(0.5);
			//		glColor4f( 0.0, 0.0, 1.0, 1.0);
			//		glBegin(GL_LINES);
			//			glVertex2f(xa, ya);
			//			glVertex2f(x2, y2);
			//		glEnd();
			//	}
			//}


			for (j = 0; j < m_pTrackerGpuData->m_NFeatures; j++)
			{
				x2 = 800.0 * m_pTrackerGpuData->m_FeatureList->_list[j]->x / tw;
				y2 = 600.0 * m_pTrackerGpuData->m_FeatureList->_list[j]->y / th;
				v2 = m_pTrackerGpuData->m_FeatureList->_list[j]->valid;
				d = sqrt(( x - x2 )*( x - x2 ) + ( y - y2 )*( y - y2 ));
				if (d<150.0) {
					glLineWidth(1.0);
					glColor4f( 1.0, 1.0, 1.0, 1.0);
					glBegin(GL_LINES);
						glVertex2f(x, y);
						glVertex2f(x2, y2);
					glEnd();
				}
			}

		}

	}
}
*/

/*
void moEffectImage::RegisterFunctions()
{
    m_iMethodBase = RegisterFunction("GetFeature");

	RegisterFunction("GetTicks");

	RegisterFunction("DrawLine");

	RegisterFunction("PushDebugString");
}

int moEffectImage::GetFeature(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    float x, y, v;
	if (m_pTrackerData) {
		x = m_pTrackerData->m_FeatureList->feature[i]->x;
		y = m_pTrackerData->m_FeatureList->feature[i]->y;
		v = m_pTrackerData->m_FeatureList->feature[i]->val;
	} else if (m_pTrackerGpuData) {
		x = m_pTrackerGpuData->m_FeatureList->_list[i]->x;
		y = m_pTrackerGpuData->m_FeatureList->_list[i]->y;
		v = m_pTrackerGpuData->m_FeatureList->_list[i]->valid;
	}

	lua_pushnumber(state, (lua_Number)x);
	lua_pushnumber(state, (lua_Number)y);
	lua_pushnumber(state, (lua_Number)v);

    return 3;
}
*/
