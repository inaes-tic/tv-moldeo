/*******************************************************************************

                      moPostEffectVideoPanel.cpp

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

#include "moPostEffectVideoPanel.h"

//========================
//  Factory
//========================

moPostEffectVideoPanelFactory *m_PostEffectVideoPanelFactory = NULL;

MO_PLG_API moPostEffectFactory* CreatePostEffectFactory(){ 
	if(m_PostEffectVideoPanelFactory==NULL)
		m_PostEffectVideoPanelFactory = new moPostEffectVideoPanelFactory();
	return(moPostEffectFactory*) m_PostEffectVideoPanelFactory;
} 

MO_PLG_API void DestroyPostEffectFactory(){ 
	delete m_PostEffectVideoPanelFactory;
	m_PostEffectVideoPanelFactory = NULL;
} 

moPostEffect*  moPostEffectVideoPanelFactory::Create() {
	return new moPostEffectVideoPanel();
}

void moPostEffectVideoPanelFactory::Destroy(moPostEffect* fx) {
	delete fx;
}

//========================
//  PostEfecto
//========================
moPostEffectVideoPanel::moPostEffectVideoPanel() {
	devicecode = NULL;
	ncodes = 0;
	m_Name = "VideoPanel";
}

moPostEffectVideoPanel::~moPostEffectVideoPanel() {
	Finish();
}

MOboolean
moPostEffectVideoPanel::Init()
{
    if (!PreInit()) return false;

    color = m_Config.GetParamIndex("color");

	radius = m_Config.GetParamIndex("radio"); 
	aberration = m_Config.GetParamIndex("aberracion");
	configuration = m_Config.GetParamIndex("configuracion");
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

	return true;
}

void moPostEffectVideoPanel::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();


    glColor4f(  m_Config.GetParam(color).GetValueFlt(MO_RED)*state.tintr,
                m_Config.GetParam(color).GetValueFlt(MO_GREEN)*state.tintg,
                m_Config.GetParam(color).GetValueFlt(MO_BLUE)*state.tintb,
                m_Config.GetParam(color).GetValueFlt(MO_ALPHA)*state.alpha);

	MOint cf;

	cf = m_Config.GetParam(configuration).GetValueFlt(0);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, MORenderMan->RenderTexGLId(MO_EFFECTS_TEX) );
	

	//CAJA PANELES VERTICALES
 	if(cf==0) {

		glMatrixMode( GL_MODELVIEW );	
		glLoadIdentity();
		glTranslatef( -3, 0.0, -5.0);
		glBegin(GL_QUADS);
		glTexCoord2f( 0.0,  0.0); glVertex2f(-0.5, -1.0);
		glTexCoord2f( 0.0,  1.0); glVertex2f(-0.5,  1.0);
		glTexCoord2f( 1.0,  1.0); glVertex2f( 0.5,  1.0);
		glTexCoord2f( 1.0,  0.0); glVertex2f( 0.5, -1.0);
		glEnd();

		glMatrixMode( GL_MODELVIEW );	
		glLoadIdentity();
		glTranslatef( -1, 0.0, -5.0 );
		glBegin(GL_QUADS);	
		glTexCoord2f( 0.0,  0.0); glVertex2f(-0.5, -1.0);
		glTexCoord2f( 0.0,  1.0); glVertex2f(-0.5,  1.0);
		glTexCoord2f( 1.0,  1.0); glVertex2f( 0.5,  1.0);
		glTexCoord2f( 1.0,  0.0); glVertex2f( 0.5, -1.0);
		glEnd();

		glMatrixMode( GL_MODELVIEW );	
		glLoadIdentity();
		glTranslatef(1, 0.0, -5.0 );
		glBegin(GL_QUADS);	
		glTexCoord2f( 0.0,  0.0); glVertex2f(-0.5, -1.0);
		glTexCoord2f( 0.0,  1.0); glVertex2f(-0.5,  1.0);
		glTexCoord2f( 1.0,  1.0); glVertex2f( 0.5,  1.0);
		glTexCoord2f( 1.0,  0.0); glVertex2f( 0.5, -1.0);
		glEnd();

		glMatrixMode( GL_MODELVIEW );	
		glLoadIdentity();
		glTranslatef(3, 1.0, -5.0);
		glBegin(GL_QUADS);	
 		glTexCoord2f( 0.0,  0.0); glVertex2f(-0.5, -1.0);
		glTexCoord2f( 0.0,  1.0); glVertex2f(-0.5,  1.0);
		glTexCoord2f( 1.0,  1.0); glVertex2f( 0.5,  1.0);
		glTexCoord2f( 1.0,  0.0); glVertex2f( 0.5, -1.0);
		glEnd();

	}

	//CAJA CON PERSPECTIVA
	if(cf==1) {
		MOfloat tZ = -4.1;
		vector_3d box_vertex[8];
		vector_2d box_texcor[8];
		box_vertex[0][0] = -4.0; box_vertex[0][1] =  3.0; box_vertex[0][2] =  0.0; box_texcor[0][0] = 0.0;			box_texcor[0][1] = 1.0*0.58;		
		box_vertex[1][0] =  4.0; box_vertex[1][1] =  3.0; box_vertex[1][2] =  0.0; box_texcor[1][0] = 1.0*0.78;		box_texcor[1][1] = 1.0*0.58;
		box_vertex[2][0] = -3.5; box_vertex[2][1] =  2.0; box_vertex[2][2] = -4.0; box_texcor[2][0] = 0.25*0.78;	box_texcor[2][1] = 0.75*0.58;
		box_vertex[3][0] =  3.5; box_vertex[3][1] =  2.0; box_vertex[3][2] = -4.0; box_texcor[3][0] = 0.75*0.78;	box_texcor[3][1] = 0.75*0.58;
		box_vertex[4][0] = -4.0; box_vertex[4][1] = -3.0; box_vertex[4][2] =  0.0; box_texcor[4][0] = 0.0;			box_texcor[4][1] = 0.0;
		box_vertex[5][0] = -3.5; box_vertex[5][1] = -3.0; box_vertex[5][2] = -4.0; box_texcor[5][0] = 0.25*0.78;	box_texcor[5][1] = 0.0;
		box_vertex[6][0] =  3.5; box_vertex[6][1] = -3.0; box_vertex[6][2] = -4.0; box_texcor[6][0] = 0.75*0.78;	box_texcor[6][1] = 0.0;
		box_vertex[7][0] =  4.0; box_vertex[7][1] = -3.0; box_vertex[7][2] =  0.0; box_texcor[7][0] = 1.0*0.78;		box_texcor[7][1] = 0.0;

		//panel izquierdo:  vx 4, 0, 2, 5
		glMatrixMode( GL_MODELVIEW );	
		glLoadIdentity();
		glTranslatef( 0.0, 0.0, tZ );
		glBegin(GL_QUADS);	
		glTexCoord2f( box_texcor[4][0],  box_texcor[4][1]); glVertex3f( box_vertex[4][0], box_vertex[4][1], box_vertex[4][2]);
		glTexCoord2f( box_texcor[0][0],  box_texcor[0][1]); glVertex3f( box_vertex[0][0], box_vertex[0][1], box_vertex[0][2]);
		glTexCoord2f( box_texcor[2][0],  box_texcor[2][1]); glVertex3f( box_vertex[2][0], box_vertex[2][1], box_vertex[2][2]);
		glTexCoord2f( box_texcor[5][0],  box_texcor[5][1]); glVertex3f( box_vertex[5][0], box_vertex[5][1], box_vertex[5][2]);
		glEnd();

		//panel fondo:   vx 5, 2, 3, 6
		glMatrixMode( GL_MODELVIEW );	
		glLoadIdentity();
		glTranslatef( 0.0, 0.0, tZ );
		glBegin(GL_QUADS);	
		glTexCoord2f( box_texcor[5][0],  box_texcor[5][1]); glVertex3f( box_vertex[5][0], box_vertex[5][1], box_vertex[5][2]);
		glTexCoord2f( box_texcor[2][0],  box_texcor[2][1]); glVertex3f( box_vertex[2][0], box_vertex[2][1], box_vertex[2][2]);
		glTexCoord2f( box_texcor[3][0],  box_texcor[3][1]); glVertex3f( box_vertex[3][0], box_vertex[3][1], box_vertex[3][2]);
		glTexCoord2f( box_texcor[6][0],  box_texcor[6][1]); glVertex3f( box_vertex[6][0], box_vertex[6][1], box_vertex[6][2]);
		glEnd();

		//panel derecho:   vx 6, 3, 1, 7
		glMatrixMode( GL_MODELVIEW );	
		glLoadIdentity();
		glTranslatef( 0.0, 0.0, tZ);
		glBegin(GL_QUADS);	
		glTexCoord2f( box_texcor[6][0],  box_texcor[6][1]); glVertex3f( box_vertex[6][0], box_vertex[6][1], box_vertex[6][2]);
		glTexCoord2f( box_texcor[3][0],  box_texcor[3][1]); glVertex3f( box_vertex[3][0], box_vertex[3][1], box_vertex[3][2]);
		glTexCoord2f( box_texcor[1][0],  box_texcor[1][1]); glVertex3f( box_vertex[1][0], box_vertex[1][1], box_vertex[1][2]);
		glTexCoord2f( box_texcor[7][0],  box_texcor[7][1]); glVertex3f( box_vertex[7][0], box_vertex[7][1], box_vertex[7][2]);
		glEnd();

		//panel arriba: vx 2, 0, 1, 3
		glMatrixMode( GL_MODELVIEW );	
		glLoadIdentity();
		glTranslatef( 0.0, 0.0, tZ);
		glColor4f(  m_Config.GetParam(color).GetValueFlt(MO_RED)*state.tintr*0.5,
            m_Config.GetParam(color).GetValueFlt(MO_GREEN)*state.tintg*0.5,
            m_Config.GetParam(color).GetValueFlt(MO_BLUE)*state.tintb*0.5,
            m_Config.GetParam(color).GetValueFlt(MO_ALPHA)*state.alpha);
		glBegin(GL_QUADS);
		glTexCoord2f( box_texcor[2][0],  box_texcor[2][1]); glVertex3f( box_vertex[2][0], box_vertex[2][1], box_vertex[2][2]);
		glTexCoord2f( box_texcor[0][0],  box_texcor[0][1]); glVertex3f( box_vertex[0][0], box_vertex[0][1], box_vertex[0][2]);
		glTexCoord2f( box_texcor[1][0],  box_texcor[1][1]); glVertex3f( box_vertex[1][0], box_vertex[1][1], box_vertex[1][2]);
		glTexCoord2f( box_texcor[3][0],  box_texcor[3][1]); glVertex3f( box_vertex[3][0], box_vertex[3][1], box_vertex[3][2]);
		glEnd();

		
		//panel abajo:   vx 4, 5, 6, 7
		glColor4f( 0.0, 0.0, 0.0, m_Config.GetParam(color).GetValueFlt(MO_ALPHA)*state.alpha);
		glMatrixMode( GL_MODELVIEW );	
		glLoadIdentity();
		glTranslatef( 0.0, 0.0, tZ);
		glBegin(GL_QUADS);	
		glTexCoord2f( box_texcor[4][0],  box_texcor[4][1]); glVertex3f( box_vertex[4][0], box_vertex[4][1], box_vertex[4][2]);
		glTexCoord2f( box_texcor[5][0],  box_texcor[5][1]); glVertex3f( box_vertex[5][0], box_vertex[5][1], box_vertex[5][2]);
		glTexCoord2f( box_texcor[6][0],  box_texcor[6][1]); glVertex3f( box_vertex[6][0], box_vertex[6][1], box_vertex[6][2]);
		glTexCoord2f( box_texcor[7][0],  box_texcor[7][1]); glVertex3f( box_vertex[7][0], box_vertex[7][1], box_vertex[7][2]);
		glEnd();
	}

	if(cf==2) {
	}

	if(cf==3) {
	}

	if(cf==4) {
	}

	if(cf==5) {
	}

	if(cf==6) {
	}




}

MOboolean moPostEffectVideoPanel::Finish()
{
    return PreFinish();
}

