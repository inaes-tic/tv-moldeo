/*******************************************************************************

                        moPreEffectMirrorMorphing.cpp

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

#include "moPreEffectMirrorMorphing.h"

//========================
//  Factory
//========================

moPreEffectMirrorMorphingFactory *m_PreEffectMirrorMorphingFactory = NULL;

MO_PLG_API moPreEffectFactory* CreatePreEffectFactory(){ 
	if(m_PreEffectMirrorMorphingFactory==NULL)
		m_PreEffectMirrorMorphingFactory = new moPreEffectMirrorMorphingFactory();
	return(moPreEffectFactory*) m_PreEffectMirrorMorphingFactory;
} 

MO_PLG_API void DestroyPreEffectFactory(){ 
	delete m_PreEffectMirrorMorphingFactory;
	m_PreEffectMirrorMorphingFactory = NULL;
} 

moPreEffect*  moPreEffectMirrorMorphingFactory::Create() {
	return new moPreEffectMirrorMorphing();
}

void moPreEffectMirrorMorphingFactory::Destroy(moPreEffect* fx) {
	delete fx;
}

//========================
//  PreEfecto
//========================

moPreEffectMirrorMorphing::moPreEffectMirrorMorphing() {
	
	devicecode = NULL;
	ncodes = 0;
	m_Name = "mirrormorphing";

}

moPreEffectMirrorMorphing::~moPreEffectMirrorMorphing() {
	Finish();
}

MOboolean moPreEffectMirrorMorphing::Init()
{
    if (!PreInit()) return false;

    color = m_Config.GetParamIndex("color");

	radius = m_Config.GetParamIndex("radius"); 
	aberration = m_Config.GetParamIndex("aberration");
	configuration = m_Config.GetParamIndex("configuration");
	blending = m_Config.GetParamIndex("blending");

    transx = m_Config.GetParamIndex("TranslateX");
    transy = m_Config.GetParamIndex("TranslateY");
    transz = m_Config.GetParamIndex("TranslateZ");
    rotatx = m_Config.GetParamIndex("RotateX");
    rotaty = m_Config.GetParamIndex("RotateY");
    rotatz = m_Config.GetParamIndex("RotateZ");
    scalex = m_Config.GetParamIndex("ScaleX");
    scaley = m_Config.GetParamIndex("ScaleY");
    scalez = m_Config.GetParamIndex("ScaleZ");


	Animate = MO_OFF;
	cfprevious = 0;
	cf = 0;
	Steps = 0;

    // Hacer la inicializacion de este efecto en particular.
    // Seteos de OpenGL.
    // Seteos de Texturas.

	int i,j,it,jt;

	MO_MESHA_TEX = 32;
	MO_MESHA_WIDTH = MORenderMan->RenderWidth() / MO_MESHA_TEX;
	MO_MESHA_HEIGHT = MORenderMan->RenderHeight() / MO_MESHA_TEX;

	MESHAMORPH = new TmoMirrorPoint*[MO_MESHA_WIDTH+1];
	for(i=0;i<(MO_MESHA_WIDTH+1);i++) MESHAMORPH[i] = new TmoMirrorPoint[MO_MESHA_HEIGHT+1];

	for(i=0,it=0;i<(MO_MESHA_WIDTH+1);i++,it+=MO_MESHA_TEX) {
	for(j=0,jt=0;j<(MO_MESHA_HEIGHT+1);j++,jt+=MO_MESHA_TEX) {
		MESHAMORPH[i][j].I = it;
		MESHAMORPH[i][j].J = jt;		
		MESHAMORPH[i][j].P0X =(float) it;
		MESHAMORPH[i][j].P0Y =(float) jt;
		MESHAMORPH[i][j].P0Z =(float) 0.0;
		MESHAMORPH[i][j].DISTANCE = sqrt(MESHAMORPH[i][j].P0X*MESHAMORPH[i][j].P0X+MESHAMORPH[i][j].P0Y*MESHAMORPH[i][j].P0Y);		
		MESHAMORPH[i][j].DISTANCEC = sqrt((MESHAMORPH[i][j].P0X-MORenderMan->RenderWidth()/2)*(MESHAMORPH[i][j].P0X-MORenderMan->RenderWidth()/2) +
			                              (MESHAMORPH[i][j].P0Y-MORenderMan->RenderHeight()/2)*(MESHAMORPH[i][j].P0Y-MORenderMan->RenderHeight()/2));		
		
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
moPreEffectMirrorMorphing::LoadConfiguration() {
int i,j;
	switch(cf) {
	case 0:
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
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
	for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
				MESHAMORPH[i][j].NSteps = 60;//se puede hacer que cada punto se anime a distinta velocidad
				MESHAMORPH[i][j].INCDX =(MESHAMORPH[i][j].D1X-MESHAMORPH[i][j].D0X)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCDY =(MESHAMORPH[i][j].D1Y-MESHAMORPH[i][j].D0Y)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCDZ =(MESHAMORPH[i][j].D1Z-MESHAMORPH[i][j].D0Z)/MESHAMORPH[i][j].NSteps;

				MESHAMORPH[i][j].INCZX =(MESHAMORPH[i][j].Z1X-MESHAMORPH[i][j].Z0X)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCZY =(MESHAMORPH[i][j].Z1Y-MESHAMORPH[i][j].Z0Y)/MESHAMORPH[i][j].NSteps;
				MESHAMORPH[i][j].INCZZ =(MESHAMORPH[i][j].Z1Z-MESHAMORPH[i][j].Z0Z)/MESHAMORPH[i][j].NSteps;

				MESHAMORPH[i][j].Animate = MO_ON;
		}
	}

	Steps = 0;
	NSteps = 60;
	Animate = MO_ON;
}


void
moPreEffectMirrorMorphing::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    MOint w = MORenderMan->RenderWidth();
    MOint h = MORenderMan->RenderHeight();
	MOint it, jt;
	MOfloat s0, s1, t0, t1;

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,w,0,h,-1,1);							// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    /*
    glTranslatef(   movx(m_Config.GetParam(transx), tempo),
                    movy(m_Config.GetParam(transy), tempo),
                    movz(m_Config.GetParam(transz), tempo));
    glRotatef(  movx(m_Config.GetParam(rotatx), tempo), 1.0, 0.0, 0.0 );
    glRotatef(  movy(m_Config.GetParam(rotaty), tempo), 0.0, 1.0, 0.0 );
    glRotatef(  movz(m_Config.GetParam(rotatz), tempo), 0.0, 0.0, 1.0 );
    glScale(    movx(m_Config.GetParam(scalex), tempo),
                movy(m_Config.GetParam(scaley), tempo),
                movz(m_Config.GetParam(scalez), tempo));
*/
	
    glColor4f(  m_Config.GetParam(color).GetValueFlt(MO_RED)*state.tintr,
                m_Config.GetParam(color).GetValueFlt(MO_GREEN)*state.tintg,
                m_Config.GetParam(color).GetValueFlt(MO_BLUE)*state.tintb,
                m_Config.GetParam(color).GetValueFlt(MO_ALPHA)*state.alpha);
    
	//DETECTA UN CAMBIO
	cf = m_Config.GetParam(configuration).GetValueFlt(0);
	
	if(cfprevious!=cf) {
		LoadConfiguration();
		cfprevious = cf;
	}

	cfprevious = cf;

	int i,j,bl;
	float ra,ab;

	ra = m_Config.GetParam(radius).GetValueFlt(0);
	ab = m_Config.GetParam(aberration).GetValueFlt(0);
	bl = m_Config.GetParam(blending).GetValueFlt(0);


    // Aca van los comandos OpenGL del efecto.
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	//glEnable(GL_BLEND);
	switch(bl) {
		case 0: glDisable(GL_BLEND);
				glDisable(GL_ALPHA);
			break;
		case 1: glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);				
			break;
		case 2: glEnable(GL_BLEND);
				glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 3: glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ZERO);
			break;
		case 4: glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
			break;
		case 5: glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 6: glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 7: glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 8: glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 9: glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;
	}
	
	//
	//glBlendFunc(GL_ONE,GL_DST_COLOR); //FLASHERO
	//glBlendFunc(GL_SRC_ALPHA,GL_DST_COLOR); //CUALQUIERA
	//glBlendFunc(GL_SRC_COLOR,GL_ONE);
	//glBlendFunc(GL_SRC_COLOR,GL_ONE);

	//APLICO MORPHING
	if((Animate==MO_ON) &&(Steps<=NSteps)) {
		Steps++;
		for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
			for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {					
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


	//APLICA la transformacion con los dx,dy,dz, y zx,zy,zz
	//aca aplico la aberracion y el radius ?? ?
	for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
		for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {
			if(i>0 && i<MO_MESHA_WIDTH) MESHAMORPH[i][j].P1X = MESHAMORPH[i][j].P0X*(1.0+MESHAMORPH[i][j].Z0X*(2.0+0.5*sin(state.tempo.ang)))+MESHAMORPH[i][j].D0X*(2.0+0.5*cos(state.tempo.ang));
			else MESHAMORPH[i][j].P1X = MESHAMORPH[i][j].P0X;
			if(j>0 && j<MO_MESHA_HEIGHT) MESHAMORPH[i][j].P1Y = MESHAMORPH[i][j].P0Y*(1.0+MESHAMORPH[i][j].Z0Y*(2.0+0.5*sin(state.tempo.ang)))+MESHAMORPH[i][j].D0Y*(2.0+0.5*cos(state.tempo.ang));
			else MESHAMORPH[i][j].P1Y = MESHAMORPH[i][j].P0Y;
			MESHAMORPH[i][j].P1Z = MESHAMORPH[i][j].P0Z;
		}
	}

	for(i=0,it=((w/2)-(MO_MESHA_WIDTH/2)*MO_MESHA_TEX); i<MO_MESHA_WIDTH; i++,it+=MO_MESHA_TEX) {
	    for(j=0,jt=((h/2)-(MO_MESHA_HEIGHT/2)*MO_MESHA_TEX); j<MO_MESHA_HEIGHT; j++,jt+=MO_MESHA_TEX) {
			glColor4f(0.97*(0.95+cos(state.tempo.ang/8)/20.0), 0.97*(0.95+cos(state.tempo.ang/8)/20.0), 0.97*(0.95+sin(state.tempo.ang/8)/20.0), 1.0*state.alpha);

            // Calculating texture coordinates.
    	    s0 = (float)it / (float)w;
			t0 = (float)jt / (float)h;
		 	s1 = (float)(it + MO_MESHA_TEX) / (float)w;
			t1 = (float)(jt + MO_MESHA_TEX) / (float)h;

			glBindTexture(GL_TEXTURE_2D, MORenderMan->RenderTexGLId(MO_EFFECTS_TEX));
			glBegin(GL_TRIANGLE_STRIP);
     			glTexCoord2f(s0, t0); 
			    glVertex3f(MESHAMORPH[i][j].P1X,MESHAMORPH[i][j].P1Y,MESHAMORPH[i][j].P1Z);

				glTexCoord2f(s0, t1); 
				glVertex3f(MESHAMORPH[i][j+1].P1X,MESHAMORPH[i][j+1].P1Y,MESHAMORPH[i][j+1].P1Z);

				glTexCoord2f(s1, t0); 
				glVertex3f(MESHAMORPH[i+1][j].P1X,MESHAMORPH[i+1][j].P1Y,MESHAMORPH[i+1][j].P1Z);

				glTexCoord2f(s1, t1); 
				glVertex3f(MESHAMORPH[i+1][j+1].P1X,MESHAMORPH[i+1][j+1].P1Y,MESHAMORPH[i+1][j+1].P1Z);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}	
}

MOboolean moPreEffectMirrorMorphing::Finish()
{
    if (MESHAMORPH != NULL)
	{
        for(int i=0;i<(MO_MESHA_WIDTH+1);i++) delete MESHAMORPH[i];
		delete[] MESHAMORPH;
		MESHAMORPH = NULL;
	}
	return PreFinish();
}
