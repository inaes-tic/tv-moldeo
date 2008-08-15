/*******************************************************************************

                      moPostEffectVideoWall.cpp

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

#include "moPostEffectVideoWall.h"

//========================
//  Factory
//========================

moPostEffectVideoWallFactory *m_PostEffectVideoWallFactory = NULL;

MO_PLG_API moPostEffectFactory* CreatePostEffectFactory(){
	if(m_PostEffectVideoWallFactory==NULL)
		m_PostEffectVideoWallFactory = new moPostEffectVideoWallFactory();
	return(moPostEffectFactory*) m_PostEffectVideoWallFactory;
}

MO_PLG_API void DestroyPostEffectFactory(){
	delete m_PostEffectVideoWallFactory;
	m_PostEffectVideoWallFactory = NULL;
}

moPostEffect*  moPostEffectVideoWallFactory::Create() {
	return new moPostEffectVideoWall();
}

void moPostEffectVideoWallFactory::Destroy(moPostEffect* fx) {
	delete fx;
}

//========================
//  PostEfecto
//========================
moPostEffectVideoWall::moPostEffectVideoWall() {
	SetName("videowall");
}

moPostEffectVideoWall::~moPostEffectVideoWall() {
	Finish();
}

MOboolean
moPostEffectVideoWall::Init()
{
    if (!PreInit()) return false;

    moDefineParamIndex( VIDEOWALL_ALPHA, moText("alpha") );
	moDefineParamIndex( VIDEOWALL_COLOR, moText("color") );
	moDefineParamIndex( VIDEOWALL_CONFIGURATION, moText("configuration") );
	moDefineParamIndex( VIDEOWALL_TRANSLATEZ, moText("translatez") );

	return true;
}

void moPostEffectVideoWall::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();


	glColor4f(  m_Config[moR(VIDEOWALL_COLOR)][MO_SELECTED][MO_RED].Fun()->Eval(state.tempo.ang) * state.tintr,
                m_Config[moR(VIDEOWALL_COLOR)][MO_SELECTED][MO_GREEN].Fun()->Eval(state.tempo.ang) * state.tintg,
                m_Config[moR(VIDEOWALL_COLOR)][MO_SELECTED][MO_BLUE].Fun()->Eval(state.tempo.ang) * state.tintb,
				m_Config[moR(VIDEOWALL_COLOR)][MO_SELECTED][MO_ALPHA].Fun()->Eval(state.tempo.ang) *
				m_Config[moR(VIDEOWALL_ALPHA)].GetData()->Fun()->Eval(state.tempo.ang) * state.alpha);

	MOint cf,i,j;

	cf = m_Config.GetParam(moR(VIDEOWALL_CONFIGURATION)).GetValue().GetSubValue(0).Int();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    MOint w = m_pResourceManager->GetRenderMan()->RenderWidth();
    MOint h = m_pResourceManager->GetRenderMan()->RenderHeight();
	MOfloat s0 = 0.35;
	MOfloat s1 = 0.55;
	MOfloat t0 = 0.35;
	MOfloat t1 = 0.55;

	glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetRenderMan()->RenderTexGLId(MO_EFFECTS_TEX));

 	if(cf==0) {
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	//glRotatef(state.tempo.ang * 30.0, 1, 0, 0);
	//glRotatef(state.tempo.ang * 40.0, 0, 1, 0);
	//glRotatef(state.tempo.ang * 50.0, 0, 0, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(s0, t0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(s0, t1); glVertex2f(-0.5,  0.5);
    glTexCoord2f(s1, t1); glVertex2f( 0.5,  0.5);
    glTexCoord2f(s1, t0); glVertex2f( 0.5, -0.5);
    glEnd();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, 1.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glBegin(GL_QUADS);
    glTexCoord2f(s0, t0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(s0, t1); glVertex2f(-0.5,  0.5);
    glTexCoord2f(s1, t1); glVertex2f( 0.5,  0.5);
    glTexCoord2f(s1, t0); glVertex2f( 0.5, -0.5);
    glEnd();

	// Update the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(1.0, 0.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glBegin(GL_QUADS);
    glTexCoord2f(s0, t0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(s0, t1); glVertex2f(-0.5,  0.5);
    glTexCoord2f(s1, t1); glVertex2f( 0.5,  0.5);
    glTexCoord2f(s1, t0); glVertex2f( 0.5, -0.5);
    glEnd();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(1.0, 1.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glBegin(GL_QUADS);
    glTexCoord2f(s0, t0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(s0, t1); glVertex2f(-0.5,  0.5);
    glTexCoord2f(s1, t1); glVertex2f( 0.5,  0.5);
    glTexCoord2f(s1, t0); glVertex2f( 0.5, -0.5);
    glEnd();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(1.0, -1.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glBegin(GL_QUADS);
    glTexCoord2f(s0, t0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(s0, t1); glVertex2f(-0.5,  0.5);
    glTexCoord2f(s1, t1); glVertex2f( 0.5,  0.5);
    glTexCoord2f(s1, t0); glVertex2f( 0.5, -0.5);
    glEnd();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(-1.0, 1.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glBegin(GL_QUADS);
    glTexCoord2f(s0, t0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(s0, t1); glVertex2f(-0.5,  0.5);
    glTexCoord2f(s1, t1); glVertex2f( 0.5,  0.5);
    glTexCoord2f(s1, t0); glVertex2f( 0.5, -0.5);
    glEnd();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(-1.0, -1.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glBegin(GL_QUADS);
    glTexCoord2f(s0, t0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(s0, t1); glVertex2f(-0.5,  0.5);
    glTexCoord2f(s1, t1); glVertex2f( 0.5,  0.5);
    glTexCoord2f(s1, t0); glVertex2f( 0.5, -0.5);
    glEnd();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(-1.0, 0.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glBegin(GL_QUADS);
    glTexCoord2f(s0, t0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(s0, t1); glVertex2f(-0.5,  0.5);
    glTexCoord2f(s1, t1); glVertex2f( 0.5,  0.5);
    glTexCoord2f(s1, t0); glVertex2f( 0.5, -0.5);
    glEnd();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, -1.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glBegin(GL_QUADS);
    glTexCoord2f(s0, t0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(s0, t1); glVertex2f(-0.5,  0.5);
    glTexCoord2f(s1, t1); glVertex2f( 0.5,  0.5);
    glTexCoord2f(s1, t0); glVertex2f( 0.5, -0.5);
    glEnd();
	}


/*version cuatro reflejados*/
	if(cf==1) {
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glRotatef(state.tempo.ang*2*15.0,0.0,0.0,1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(s0, t0); glVertex2f(-1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0,  1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0,  1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glTexCoord2f(s0, t0); glVertex2f(-1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0,  -1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  -1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0,  -1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  -1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glEnd();
	}

	if(cf==2) {
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glRotatef(state.tempo.ang*2.0*15,0.0,0.0,1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(s0, t0); glVertex2f(-1.0-1.0, 0.0+1.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0-1.0,  1.0+1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0-1.0,  1.0+1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0-1.0, 0.0+1.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0+1.0, 0.0+1.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0+1.0,  1.0+1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0+1.0,  1.0+1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0+1.0, 0.0+1.0);
	glTexCoord2f(s0, t0); glVertex2f(-1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0,  1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0,  1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glTexCoord2f(s0, t0); glVertex2f(-1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0,  -1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  -1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0,  -1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  -1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);
	glTexCoord2f(s0, t0); glVertex2f(-1.0-1.0, 0.0-1.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0-1.0,  -1.0-1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0-1.0,  -1.0-1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0-1.0, 0.0-1.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0+1.0, 0.0-1.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0+1.0,  -1.0-1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0+1.0,  -1.0-1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0+1.0, 0.0-1.0);

	glEnd();
	}

	if(cf==3) {
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glRotatef(state.tempo.ang*2.0*15,0.0,0.0,1.0);
	glBegin(GL_TRIANGLES);
	for(i=0;i<4;i++) {
		for(j=0;j<4;j++) {
			if((i % 2)==0 &&(j % 2)==0) {
				glTexCoord2f(s1, t1); glVertex2f(-2.0+1.0*i,2.0-1.0*j);
				glTexCoord2f(s1, t0); glVertex2f(-2.0+1.0*i,2.0-1.0*(j+1));
				glTexCoord2f(s0, t0); glVertex2f(-2.0+1.0*(i+1), 2.0-1.0*(j+1));

				glTexCoord2f(s1, t1); glVertex2f(-2.0+1.0*i,2.0-1.0*j);
				glTexCoord2f(s0, t0); glVertex2f(-2.0+1.0*(i+1), 2.0-1.0*(j+1));
				glTexCoord2f(s1, t0); glVertex2f(-2.0+1.0*(i+1),2.0-1.0*j);
			}
			else if((i % 2)==1 &&(j % 2)==0) {
				glTexCoord2f(s1, t0); glVertex2f(-2.0+1.0*i,2.0-1.0*j);
				glTexCoord2f(s0, t0); glVertex2f(-2.0+1.0*i,2.0-1.0*(j+1));
				glTexCoord2f(s1, t1); glVertex2f(-2.0+1.0*(i+1), 2.0-1.0*j);

				glTexCoord2f(s1, t1); glVertex2f(-2.0+1.0*(i+1),2.0-1.0*j);
				glTexCoord2f(s1, t0); glVertex2f(-2.0+1.0*(i+1), 2.0-1.0*(j+1));
				glTexCoord2f(s0, t0); glVertex2f(-2.0+1.0*i,2.0-1.0*(j+1));
			}
			else if((i % 2)==0 &&(j % 2)==1) {
				glTexCoord2f(s1, t0); glVertex2f(-2.0+1.0*i,2.0-1.0*j);
				glTexCoord2f(s1, t1); glVertex2f(-2.0+1.0*i,2.0-1.0*(j+1));
				glTexCoord2f(s0, t0); glVertex2f(-2.0+1.0*(i+1), 2.0-1.0*j);

				glTexCoord2f(s0, t0); glVertex2f(-2.0+1.0*(i+1),2.0-1.0*j);
				glTexCoord2f(s1, t0); glVertex2f(-2.0+1.0*(i+1), 2.0-1.0*(j+1));
				glTexCoord2f(s1, t1); glVertex2f(-2.0+1.0*i,2.0-1.0*(j+1));
			}
			else if((i % 2)==1 &&(j % 2)==1) {
				glTexCoord2f(s0, t0); glVertex2f(-2.0+1.0*i,2.0-1.0*j);
				glTexCoord2f(s1, t0); glVertex2f(-2.0+1.0*i,2.0-1.0*(j+1));
				glTexCoord2f(s1, t1); glVertex2f(-2.0+1.0*(i+1), 2.0-1.0*(j+1));

				glTexCoord2f(s0, t0); glVertex2f(-2.0+1.0*i,2.0-1.0*j);
				glTexCoord2f(s1, t1); glVertex2f(-2.0+1.0*(i+1), 2.0-1.0*(j+1));
				glTexCoord2f(s1, t0); glVertex2f(-2.0+1.0*(i+1),2.0-1.0*j);
			}
		}
	}
	glEnd();
	}

	float an,an2,rad;

	rad = 2.0 * state.amplitude;

	if(cf==4) {
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glRotatef(state.tempo.ang*2.0*15,0.0,0.0,1.0);
	glBegin(GL_TRIANGLE_FAN);
	//el centro
	glTexCoord2f(s0, t0); glVertex2f(0.0,0.0);
	//los bordes
		for(i=0;i<23;i++) {
			an =(float)(i) *(moMathf::PI / 12.0f);
			an2 =(float)(i+1) *(moMathf::PI / 12.0f);
			if((i % 2) ==0) {
				glTexCoord2f(s0, t1); glVertex2f(rad*cos(an),2.0*sin(an));
				glTexCoord2f(s1, t1); glVertex2f(rad*cos(an2),2.0*sin(an2));
			} else {
				glTexCoord2f(s1, t1); glVertex2f(rad*cos(an),2.0*sin(an));
				glTexCoord2f(s0, t1); glVertex2f(rad*cos(an2),2.0*sin(an2));
			}

		}
	glTexCoord2f(s0, t1); glVertex2f(rad*cos(0.0),rad*sin(0.0));
	glEnd();
	}

	if(cf==5) {
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glRotatef(state.tempo.ang*2.0*15,1.0,1.0,1.0);
	glBegin(GL_TRIANGLE_FAN);
	//el centro
	glTexCoord2f(s0, t0); glVertex2f(0.0,0.0);
	//los bordes
		for(i=0;i<23;i++) {
			an =(float)(i) *(moMathf::PI / 12.0f);
			an2 =(float)(i+1) *(moMathf::PI / 12.0f);
			if((i % 2) ==0) {
				glTexCoord2f(s0, t1); glVertex2f(rad*cos(an),2.0*sin(an));
				glTexCoord2f(s1, t1); glVertex2f(rad*cos(an2),2.0*sin(an2));
			} else {
				glTexCoord2f(s1, t1); glVertex2f(rad*cos(an),2.0*sin(an));
				glTexCoord2f(s0, t1); glVertex2f(rad*cos(an2),2.0*sin(an2));
			}

		}
	glTexCoord2f(s0, t1); glVertex2f(rad*cos(0.0),rad*sin(0.0));
	glEnd();
	}

	if(cf==6) {
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.5+m_Config[moR(VIDEOWALL_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));
	glRotatef(state.tempo.ang*togra,1.0,0.0,1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(s0, t0); glVertex2f(-1.0-1.0, 0.0+1.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0-1.0,  1.0+1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0-1.0,  1.0+1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0-1.0, 0.0+1.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0+1.0, 0.0+1.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0+1.0,  1.0+1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0+1.0,  1.0+1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0+1.0, 0.0+1.0);
	glTexCoord2f(s0, t0); glVertex2f(-1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0,  1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0,  1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glTexCoord2f(s0, t0); glVertex2f(-1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0,  -1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  -1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0, 0.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0,  -1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0,  -1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0, 0.0);
	glTexCoord2f(s0, t0); glVertex2f(-1.0-1.0, 0.0-1.0);
    glTexCoord2f(s0, t1); glVertex2f(-1.0-1.0,  -1.0-1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0-1.0,  -1.0-1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0-1.0, 0.0-1.0);

	glTexCoord2f(s0, t0); glVertex2f(1.0+1.0, 0.0-1.0);
    glTexCoord2f(s0, t1); glVertex2f(1.0+1.0,  -1.0-1.0);
    glTexCoord2f(s1, t1); glVertex2f( 0.0+1.0,  -1.0-1.0);
    glTexCoord2f(s1, t0); glVertex2f( 0.0+1.0, 0.0-1.0);

	glEnd();
	}

}

MOboolean moPostEffectVideoWall::Finish()
{
    return PreFinish();
}

moConfigDefinition *
moPostEffectVideoWall::GetDefinition( moConfigDefinition *p_configdefinition ) {
	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("configuration"), MO_PARAM_NUMERIC, VIDEOWALL_CONFIGURATION, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_FUNCTION, VIDEOWALL_TRANSLATEZ );
	return p_configdefinition;
}
