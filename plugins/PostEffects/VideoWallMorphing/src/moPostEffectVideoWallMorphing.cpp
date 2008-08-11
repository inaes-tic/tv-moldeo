/*******************************************************************************

                     moPostEffectVideoWallMorphing.cpp

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

#include "moPostEffectVideoWallMorphing.h"

//========================
//  Factory
//========================

moPostEffectVideoWallMorphingFactory *m_PostEffectVideoWallMorphingFactory = NULL;

MO_PLG_API moPostEffectFactory* CreatePostEffectFactory(){
	if(m_PostEffectVideoWallMorphingFactory==NULL)
		m_PostEffectVideoWallMorphingFactory = new moPostEffectVideoWallMorphingFactory();
	return(moPostEffectFactory*) m_PostEffectVideoWallMorphingFactory;
}

MO_PLG_API void DestroyPostEffectFactory(){
	delete m_PostEffectVideoWallMorphingFactory;
	m_PostEffectVideoWallMorphingFactory = NULL;
}

moPostEffect*  moPostEffectVideoWallMorphingFactory::Create() {
	return new moPostEffectVideoWallMorphing();
}

void moPostEffectVideoWallMorphingFactory::Destroy(moPostEffect* fx) {
	delete fx;
}

//========================
//  PostEfecto
//========================
moPostEffectVideoWallMorphing::moPostEffectVideoWallMorphing() {
	SetName("videowallmorphing");
}

moPostEffectVideoWallMorphing::~moPostEffectVideoWallMorphing() {
	Finish();
}

MOboolean
moPostEffectVideoWallMorphing::Init() {

    if (!PreInit()) return false;

	MORenderMan = m_pResourceManager->GetRenderMan();

	moDefineParamIndex( VIDEOWALLMORPHING_ALPHA, moText("alpha") );
	moDefineParamIndex( VIDEOWALLMORPHING_COLOR, moText("color") );
	moDefineParamIndex( VIDEOWALLMORPHING_CONFIGURATION, moText("configuration") );
	moDefineParamIndex( VIDEOWALLMORPHING_BLENDING, moText("blending") );
	moDefineParamIndex( VIDEOWALLMORPHING_RADIUS, moText("radius") );
	moDefineParamIndex( VIDEOWALLMORPHING_ABERRATION, moText("aberration") );
	moDefineParamIndex( VIDEOWALLMORPHING_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( VIDEOWALLMORPHING_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( VIDEOWALLMORPHING_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( VIDEOWALLMORPHING_SCALEX, moText("scalex") );
	moDefineParamIndex( VIDEOWALLMORPHING_SCALEY, moText("scaley") );
	moDefineParamIndex( VIDEOWALLMORPHING_SCALEZ, moText("scalez") );
	moDefineParamIndex( VIDEOWALLMORPHING_ROTATEX, moText("rotatex") );
	moDefineParamIndex( VIDEOWALLMORPHING_ROTATEY, moText("rotatey") );
	moDefineParamIndex( VIDEOWALLMORPHING_ROTATEZ, moText("rotatez") );



	Animate = MO_OFF;
	cfprevious = 0;
	cf = 0;
	Steps = 0;

//glEnable(GL_BLEND);
float tita,fi,R;
int i,j;


	MO_MESHA_TEX2 = 32;
	MO_MESHA_WIDTH4 = MORenderMan->RenderWidth() / MO_MESHA_TEX2;
	MO_MESHA_HEIGHT4 = MORenderMan->RenderHeight() / MO_MESHA_TEX2;

    MESHAMORPH = new TmoVideowallPoint*[MO_MESHA_WIDTH4+1];
	for(i=0;i<(MO_MESHA_WIDTH4+1);i++) MESHAMORPH[i] = new TmoVideowallPoint[MO_MESHA_HEIGHT4+1];

R = 200.0;

for(i=0,tita=0.0;i<(MO_MESHA_WIDTH4+1);i++,tita+=2*moMathf::PI/(float)(MO_MESHA_WIDTH4)) {
	for(j=0,fi=0.0;j<(MO_MESHA_HEIGHT4+1);j++,fi+=moMathf::PI/(float)(MO_MESHA_HEIGHT4)) {
		MESHAMORPH[i][j].P0X =(float) cos(tita)*R*sin(fi);
		MESHAMORPH[i][j].P0Y =(float) sin(tita)*R*sin(fi);
		MESHAMORPH[i][j].P0Z =(float) R*cos(fi);
		//MESHAMORPH[i][j].P0X =(float)(it-(MORenderMan->RenderWidth()/2));
		//MESHAMORPH[i][j].P0Y =(float) -((MORenderMan->RenderHeight()/2)-jt);
		//MESHAMORPH[i][j].P0Z =(float) -480;
		MESHAMORPH[i][j].DISTANCE = sqrt(MESHAMORPH[i][j].P0X*MESHAMORPH[i][j].P0X+MESHAMORPH[i][j].P0Y*MESHAMORPH[i][j].P0Y);
		MESHAMORPH[i][j].DISTANCEC = sqrt((MESHAMORPH[i][j].P0X-MORenderMan->RenderWidth()/2)*(MESHAMORPH[i][j].P0X-MORenderMan->RenderWidth()/2)+(MESHAMORPH[i][j].P0Y-MORenderMan->RenderHeight()/2)*(MESHAMORPH[i][j].P0Y-MORenderMan->RenderHeight()/2));

		MESHAMORPH[i][j].Z0X = 0.0;
		MESHAMORPH[i][j].Z0Y = 0.0;
		MESHAMORPH[i][j].Z0Z = 0.0;
		MESHAMORPH[i][j].Z1X = MESHAMORPH[i][j].Z0X;
		MESHAMORPH[i][j].Z1Y = MESHAMORPH[i][j].Z0Y;
		MESHAMORPH[i][j].Z1Z = MESHAMORPH[i][j].Z0Z;


		MESHAMORPH[i][j].D0X = 0.0;
		MESHAMORPH[i][j].D0Y = 0.0;
		MESHAMORPH[i][j].D0Z = 0.0;
		MESHAMORPH[i][j].D1X = MESHAMORPH[i][j].D0X;
		MESHAMORPH[i][j].D1Y = MESHAMORPH[i][j].D0Y;
		MESHAMORPH[i][j].D1Z = MESHAMORPH[i][j].D0Z;

		MESHAMORPH[i][j].P1X = MESHAMORPH[i][j].P0X*(1.0+MESHAMORPH[i][j].Z0X)+MESHAMORPH[i][j].D0X;
		MESHAMORPH[i][j].P1Y = MESHAMORPH[i][j].P0Y*(1.0+MESHAMORPH[i][j].Z0Y)+MESHAMORPH[i][j].D0Y;
		MESHAMORPH[i][j].P1Z = MESHAMORPH[i][j].P0Z*(1.0+MESHAMORPH[i][j].Z0Z)+MESHAMORPH[i][j].D0Z;
		MESHAMORPH[i][j].NX = 0.0;
		MESHAMORPH[i][j].NY = 0.0;
		MESHAMORPH[i][j].NZ = 0.1;
	}
	}

	return true;
}

void
moPostEffectVideoWallMorphing::LoadConfiguration() {
int i,j;
	switch(cf) {
	case 0:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = 0.01;
				MESHAMORPH[i][j].Z1Y = 0.0;
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = 0.01;
				MESHAMORPH[i][j].D1Y = 0.01;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 1:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = 0.01;
				MESHAMORPH[i][j].Z1Y = -0.01;
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = -0.05;
				MESHAMORPH[i][j].D1Y = 0.05;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 2:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -0.04;
				MESHAMORPH[i][j].Z1Y = 0.04;
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = 0.0;
				MESHAMORPH[i][j].D1Y = 0.0;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 3:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -0.04;
				MESHAMORPH[i][j].Z1Y = -0.04;
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = 0.0;
				MESHAMORPH[i][j].D1Y = 0.0;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 4:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = 0.04;
				MESHAMORPH[i][j].Z1Y = -0.04;
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = 0.0;
				MESHAMORPH[i][j].D1Y = 0.0;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;

	case 5:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = 0.001;
				MESHAMORPH[i][j].Z1Y = 0.001;
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = 0.004*i;
				MESHAMORPH[i][j].D1Y = 0.005*j;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 6:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = 0.05;
				MESHAMORPH[i][j].Z1Y = 0.05;
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = -0.05;
				MESHAMORPH[i][j].D1Y = -0.15;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 7:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = 0.001;
				MESHAMORPH[i][j].Z1Y = 0.001;
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = MESHAMORPH[i][j].P0Y*5.0/MESHAMORPH[i][j].DISTANCE;
				MESHAMORPH[i][j].D1Y = -MESHAMORPH[i][j].P0X*5.0/MESHAMORPH[i][j].DISTANCE;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;

	case 8:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X =(0.05*(MESHAMORPH[i][j].P0X-(MORenderMan->RenderWidth()/2))/(MORenderMan->RenderWidth()/2));
				MESHAMORPH[i][j].Z1Y =(0.05*(MESHAMORPH[i][j].P0Y-(MORenderMan->RenderHeight()/2))/(MORenderMan->RenderHeight()/2));
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X =(MESHAMORPH[i][j].P0Y-MORenderMan->RenderHeight()/2)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Y = -(MESHAMORPH[i][j].P0X-MORenderMan->RenderWidth()/2)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 9:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -(0.05*(MESHAMORPH[i][j].P0X-(MORenderMan->RenderWidth()/2))/(MORenderMan->RenderWidth()/2));
				MESHAMORPH[i][j].Z1Y = -(0.05*(MESHAMORPH[i][j].P0Y-(MORenderMan->RenderHeight()/2))/(MORenderMan->RenderHeight()/2));
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = -(MESHAMORPH[i][j].P0Y-MORenderMan->RenderHeight()/2)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Y =(MESHAMORPH[i][j].P0X-MORenderMan->RenderWidth()/2)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 10:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -(0.05*(MESHAMORPH[i][j].P0X-(MORenderMan->RenderWidth()/2))/(MORenderMan->RenderWidth()/2));
				MESHAMORPH[i][j].Z1Y = -(0.05*(MESHAMORPH[i][j].P0Y-(MORenderMan->RenderHeight()/2))/(MORenderMan->RenderHeight()/2));
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X =(MESHAMORPH[i][j].P0Y-MORenderMan->RenderHeight()/2)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Y =(MESHAMORPH[i][j].P0X-MORenderMan->RenderWidth()/2)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 11:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -(0.05*(MESHAMORPH[i][j].P0X-(MORenderMan->RenderWidth()/2))/(MORenderMan->RenderWidth()/2));
				MESHAMORPH[i][j].Z1Y = -(0.05*(MESHAMORPH[i][j].P0Y-(MORenderMan->RenderHeight()/2))/(MORenderMan->RenderHeight()/2));
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X =(MESHAMORPH[i][j].P0Y-100.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Y =(MESHAMORPH[i][j].P0X-200.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 12:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -(0.05*(MESHAMORPH[i][j].P0X-(MORenderMan->RenderWidth()/2))/(MORenderMan->RenderWidth()/2));
				MESHAMORPH[i][j].Z1Y = -(0.05*(MESHAMORPH[i][j].P0Y-(MORenderMan->RenderHeight()/2))/(MORenderMan->RenderHeight()/2));
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X =(MESHAMORPH[i][j].P0Y-200.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Y =(MESHAMORPH[i][j].P0X-100.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 13:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -(0.05*(MESHAMORPH[i][j].P0X-(MORenderMan->RenderWidth()/2))/(MORenderMan->RenderWidth()/2));
				MESHAMORPH[i][j].Z1Y = -(0.05*(MESHAMORPH[i][j].P0Y-(MORenderMan->RenderHeight()/2))/(MORenderMan->RenderHeight()/2));
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = -(MESHAMORPH[i][j].P0X-200.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Y =(MESHAMORPH[i][j].P0Y-100.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 14:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -(0.05*(MESHAMORPH[i][j].P0X-(MORenderMan->RenderWidth()/2))/(MORenderMan->RenderWidth()/2));
				MESHAMORPH[i][j].Z1Y = -(0.05*(MESHAMORPH[i][j].P0Y-(MORenderMan->RenderHeight()/2))/(MORenderMan->RenderHeight()/2));
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X =(MESHAMORPH[i][j].P0Y-200.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Y = -(MESHAMORPH[i][j].P0X-100.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 15:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -(0.05*(MESHAMORPH[i][j].P0X-(MORenderMan->RenderWidth()/2))/(MORenderMan->RenderWidth()/2));
				MESHAMORPH[i][j].Z1Y = -(0.05*(MESHAMORPH[i][j].P0Y-(MORenderMan->RenderHeight()/2))/(MORenderMan->RenderHeight()/2));
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = -(MESHAMORPH[i][j].P0X-200.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Y = -(MESHAMORPH[i][j].P0X-100.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;
	case 16:
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].Z1X = -(0.05*(MESHAMORPH[i][j].P0X-(MORenderMan->RenderWidth()/2))/(MORenderMan->RenderWidth()/2));
				MESHAMORPH[i][j].Z1Y = -(0.05*(MESHAMORPH[i][j].P0Y-(MORenderMan->RenderHeight()/2))/(MORenderMan->RenderHeight()/2));
				MESHAMORPH[i][j].Z1Z = 0.0;
				MESHAMORPH[i][j].D1X = -(MESHAMORPH[i][j].P0X-500.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Y =(MESHAMORPH[i][j].P0X-100.0)*5.0/MESHAMORPH[i][j].DISTANCEC;
				MESHAMORPH[i][j].D1Z = 0.0;
			}
		}
		break;

	default:
		break;
	}

	//por defecto para todos
	for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
				MESHAMORPH[i][j].NSteps = 60;//se puede hacer que cada punto se anime a distinta velocidad
				MESHAMORPH[i][j].INCDX =(MESHAMORPH[i][j].D1X-MESHAMORPH[i][j].D0X)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCDY =(MESHAMORPH[i][j].D1Y-MESHAMORPH[i][j].D0Y)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCDZ =(MESHAMORPH[i][j].D1Z-MESHAMORPH[i][j].D0Z)/MESHAMORPH[i][j].NSteps;

				MESHAMORPH[i][j].INCZX =(MESHAMORPH[i][j].Z1X-MESHAMORPH[i][j].Z0X)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCZY =(MESHAMORPH[i][j].Z1Y-MESHAMORPH[i][j].Z0Y)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCZZ =(MESHAMORPH[i][j].Z1Z-MESHAMORPH[i][j].Z0Z)/MESHAMORPH[i][j].NSteps;

				MESHAMORPH[i][j].INCPX =(MESHAMORPH[i][j].P1X-MESHAMORPH[i][j].P0X)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCPY =(MESHAMORPH[i][j].P1Y-MESHAMORPH[i][j].P0Y)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCPZ =(MESHAMORPH[i][j].P1Z-MESHAMORPH[i][j].P0Z)/MESHAMORPH[i][j].NSteps;

				MESHAMORPH[i][j].Animate = MO_ON;
		}
	}

	Steps = 0;
	NSteps = 60;
	Animate = MO_ON;
}


void moPostEffectVideoWallMorphing::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    MOint w = MORenderMan->RenderWidth();
    MOint h = MORenderMan->RenderHeight();

	MOfloat s0 = 0.35;
	MOfloat s1 = 0.55;
	MOfloat t0 = 0.35;
	MOfloat t1 = 0.55;

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();


	MOfloat r = m_Config.GetParam(moR(VIDEOWALLMORPHING_COLOR)).GetValue().GetSubValue(MO_RED).GetData()->Fun()->Eval(state.tempo.ang)*state.tintr;
    MOfloat g = m_Config.GetParam(moR(VIDEOWALLMORPHING_COLOR)).GetValue().GetSubValue(MO_GREEN).GetData()->Fun()->Eval(state.tempo.ang)*state.tintg;
    MOfloat b = m_Config.GetParam(moR(VIDEOWALLMORPHING_COLOR)).GetValue().GetSubValue(MO_BLUE).GetData()->Fun()->Eval(state.tempo.ang)*state.tintb;
	MOfloat a = m_Config.GetParam(moR(VIDEOWALLMORPHING_COLOR)).GetValue().GetSubValue(MO_ALPHA).GetData()->Fun()->Eval(state.tempo.ang)*state.alpha;
    glColor4f(r, g, b, a);

	//DETECTA UN CAMBIO
	cf = m_Config.GetParam(moR(VIDEOWALLMORPHING_CONFIGURATION)).GetValue().GetSubValue(0).Int();

	if(cfprevious!=cf) {
		LoadConfiguration();
		cfprevious = cf;
	}

	cfprevious = cf;

	glBindTexture(GL_TEXTURE_2D, MORenderMan->RenderTexGLId(MO_EFFECTS_TEX) );

    MOint i,j;

	//glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);

	glTranslatef(   m_Config[moR(VIDEOWALLMORPHING_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
                    m_Config[moR(VIDEOWALLMORPHING_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
                    m_Config[moR(VIDEOWALLMORPHING_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));

	glRotatef(  m_Config[moR(VIDEOWALLMORPHING_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(VIDEOWALLMORPHING_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(VIDEOWALLMORPHING_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
	glScalef(   m_Config[moR(VIDEOWALLMORPHING_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(VIDEOWALLMORPHING_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(VIDEOWALLMORPHING_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang));


    // Agregado el parametro state.
    SetColor( m_Config[moR(VIDEOWALLMORPHING_COLOR)][MO_SELECTED], m_Config[moR(VIDEOWALLMORPHING_ALPHA)][MO_SELECTED], state );

	SetBlending( (moBlendingModes)m_Config[moR(VIDEOWALLMORPHING_BLENDING)].GetData()->Int() );

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	/*
	glTranslatef(0.0,0.0,-300.0+10*cos(state.tempo.ang));
	glRotatef(state.tempo.ang*5.0,0.0,0.0,1.0);
	glRotatef(state.tempo.ang*5.0,0.0,1.0,1.0);
	glRotatef(state.tempo.ang*5.0,1.0,1.0,1.0);
*/


	//hacemos el morphing(animacion)
	if((Animate==MO_ON) &&(Steps<=NSteps)) {
		Steps++;
		for(i=0;i<(MO_MESHA_WIDTH4+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT4+1);j++) {
					MESHAMORPH[i][j].P0X = MESHAMORPH[i][j].P0X + MESHAMORPH[i][j].INCPX;
					MESHAMORPH[i][j].P0Y = MESHAMORPH[i][j].P0Y + MESHAMORPH[i][j].INCPY;
					MESHAMORPH[i][j].P0Z = MESHAMORPH[i][j].P0Z + MESHAMORPH[i][j].INCPZ;

					MESHAMORPH[i][j].D0X = MESHAMORPH[i][j].D0X + MESHAMORPH[i][j].INCDX;
					MESHAMORPH[i][j].D0Y = MESHAMORPH[i][j].D0Y + MESHAMORPH[i][j].INCDY;
					MESHAMORPH[i][j].D0Z = MESHAMORPH[i][j].D0Z + MESHAMORPH[i][j].INCDZ;

					MESHAMORPH[i][j].Z0X = MESHAMORPH[i][j].Z0X + MESHAMORPH[i][j].INCZX;
					MESHAMORPH[i][j].Z0Y = MESHAMORPH[i][j].Z0Y + MESHAMORPH[i][j].INCZY;
					MESHAMORPH[i][j].Z0Z = MESHAMORPH[i][j].Z0Z + MESHAMORPH[i][j].INCZZ;
			}
		}
	} else {
		Animate = MO_OFF;
		Steps = 0;
	}

	//aplica la transformacion con los dx,dy,dz, y zx,zy,zz
	for(i=0;i<MO_MESHA_WIDTH4+1;i++) {
		for(j=0;j<MO_MESHA_HEIGHT4+1;j++) {
			MESHAMORPH[i][j].PAUXX = MESHAMORPH[i][j].P0X*(1.0+MESHAMORPH[i][j].Z0X)+MESHAMORPH[i][j].D0X;
			MESHAMORPH[i][j].PAUXY = MESHAMORPH[i][j].P0Y*(1.0+MESHAMORPH[i][j].Z0Y)+MESHAMORPH[i][j].D0Y;
			MESHAMORPH[i][j].PAUXZ = MESHAMORPH[i][j].P0Z*(1.0+MESHAMORPH[i][j].Z0Z)+MESHAMORPH[i][j].D0Z;
		}
	}

	float tita,fi;

	if(cf==0) {
	for(i=0,tita=0.0;i<(MO_MESHA_WIDTH4);i++,tita+=2*moMathf::PI/(float)(MO_MESHA_WIDTH4)) {
	for(j=0,fi=0.0;j<(MO_MESHA_HEIGHT4);j++,fi+=moMathf::PI/(float)(MO_MESHA_HEIGHT4)) {
				glColor4f(0.97*(0.95+cos(state.tempo.ang/8)/20.0), 0.97*(0.95+cos(state.tempo.ang/8)/20.0), 0.97*(0.95+sin(state.tempo.ang/8)/20.0), 1.0*state.alpha);
				glBegin(GL_TRIANGLE_STRIP);
					//glNormal3f(MESHAG[i][j].NX,MESHAG[i][j].NY,MESHAG[i][j].NZ);
					glTexCoord2f(s0, t0); glVertex3f(MESHAMORPH[i][j].PAUXX,MESHAMORPH[i][j].PAUXY,MESHAMORPH[i][j].PAUXZ);
					//glNormal3f(MESHAMORPH[i][j+1].NX,MESHAMORPH[i][j+1].NY,MESHAMORPH[i][j+1].NZ);
					glTexCoord2f(s0, t1); glVertex3f(MESHAMORPH[i][j+1].PAUXX,MESHAMORPH[i][j+1].PAUXY,MESHAMORPH[i][j+1].PAUXZ);
					//glNormal3f(MESHAMORPH[i+1][j].NX,MESHAMORPH[i+1][j].NY,MESHAMORPH[i+1][j].NZ);
					glTexCoord2f(s1, t0); glVertex3f(MESHAMORPH[i+1][j].PAUXX,MESHAMORPH[i+1][j].PAUXY,MESHAMORPH[i+1][j].PAUXZ);
					//glNormal3f(MESHAMORPH[i+1][j+1].NX,MESHAMORPH[i+1][j+1].NY,MESHAMORPH[i+1][j+1].NZ);
					glTexCoord2f(s1, t1); glVertex3f(MESHAMORPH[i+1][j+1].PAUXX,MESHAMORPH[i+1][j+1].PAUXY,MESHAMORPH[i+1][j+1].PAUXZ);
				glEnd();
			}
		}
	} else {
	for(i=0,tita=0.0;i<(MO_MESHA_WIDTH4);i++,tita+=2*moMathf::PI/(float)(MO_MESHA_WIDTH4)) {
	for(j=0,fi=0.0;j<(MO_MESHA_HEIGHT4);j++,fi+=moMathf::PI/(float)(MO_MESHA_HEIGHT4)) {
				glPushMatrix();
				glTranslatef(MESHAMORPH[i][j].PAUXX*(1.0+cos(state.tempo.ang)/50),MESHAMORPH[i][j].PAUXY*(1.0+cos(state.tempo.ang)/50),MESHAMORPH[i][j].PAUXZ*(1.0+cos(state.tempo.ang)/50));
				glRotatef(tita*180.0/(float)moMathf::PI,0.0,1.0,0.0);
				glRotatef(fi*180.0/(float)moMathf::PI,1.0,0.0,0.0);
				glScalef(30.0,30.0,30.0);
				glBegin(GL_TRIANGLE_STRIP);
					glTexCoord2f(s0, t0); glVertex2f(0.0,0.0);
					glTexCoord2f(s0, t1); glVertex2f(0.0,1.0);
					glTexCoord2f(s1, t0); glVertex2f(1.0,0.0);
					glTexCoord2f(s1, t1); glVertex2f(1.0,1.0);
				glEnd();
				glPopMatrix();
			}
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();

}

MOboolean moPostEffectVideoWallMorphing::Finish()
{
    if (MESHAMORPH != NULL)
	{
        for(int i=0;i<(MO_MESHA_WIDTH4+1);i++) delete MESHAMORPH[i];
		delete[] MESHAMORPH;
		MESHAMORPH = NULL;
	}
	return PreFinish();
}

moConfigDefinition *
moPostEffectVideoWallMorphing::GetDefinition( moConfigDefinition *p_configdefinition ) {
	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("configuration"), MO_PARAM_NUMERIC, VIDEOWALLMORPHING_CONFIGURATION );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, VIDEOWALLMORPHING_BLENDING );
	p_configdefinition->Add( moText("radius"), MO_PARAM_FUNCTION, VIDEOWALLMORPHING_RADIUS );
	p_configdefinition->Add( moText("aberration"), MO_PARAM_FUNCTION, VIDEOWALLMORPHING_ABERRATION );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, VIDEOWALLMORPHING_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, VIDEOWALLMORPHING_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, VIDEOWALLMORPHING_TRANSLATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, VIDEOWALLMORPHING_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, VIDEOWALLMORPHING_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, VIDEOWALLMORPHING_SCALEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, VIDEOWALLMORPHING_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, VIDEOWALLMORPHING_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, VIDEOWALLMORPHING_ROTATEZ );

	return p_configdefinition;
}
