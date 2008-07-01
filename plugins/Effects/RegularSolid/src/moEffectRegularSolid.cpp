/*******************************************************************************

                              moEffectRegularSolid.cpp

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

#include "moEffectRegularSolid.h"

//========================
//  Factory
//========================

moEffectRegularSolidFactory *m_EfectoSolidoRegularFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){ 
	if(m_EfectoSolidoRegularFactory==NULL)
		m_EfectoSolidoRegularFactory = new moEffectRegularSolidFactory();
	return(moEffectFactory*) m_EfectoSolidoRegularFactory;
} 

MO_PLG_API void DestroyEffectFactory(){ 
	delete m_EfectoSolidoRegularFactory;
	m_EfectoSolidoRegularFactory = NULL;
} 

moEffect*  moEffectRegularSolidFactory::Create() {
	return new moEffectRegularSolid();
}

void moEffectRegularSolidFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectRegularSolid::moEffectRegularSolid() {
	devicecode = NULL;
	ncodes = 0;

	m_Name = "solid";
}

moEffectRegularSolid::~moEffectRegularSolid() {
	Finish();
}

MOboolean moEffectRegularSolid::Init() {

    if (!PreInit()) return false;

    // Hacer la inicializacion de este efecto en particular.
	solid = m_Config.GetParamIndex("solid"); 
    color = m_Config.GetParamIndex("color");
    texture = m_Config.GetParamIndex("textura");
	//cintas =  m_Config.GetParamIndex("cinta");

	scalex =  m_Config.GetParamIndex("scalex");
	scaley =  m_Config.GetParamIndex("scaley");
	scalez =  m_Config.GetParamIndex("scalez");
	//axisangle = m_Config.GetParamIndex("axisangle");

    //Seteos de Texturas.
	Images.MODebug = MODebug;
	Images.Init(GetConfig(),texture,MOTextures);

	//PLANETA
	/*
	m_Config.SetCurrentParam(cintas);
	m_Config.FirstValue();
	ncintas = m_Config.GetValueCount(cintas);
	Cintas = new  MOtexture [ncintas];	
	for( i = 0; i < ncintas; i++) {
		Cintas[i] = MOTextures->GetTexture(m_Config.GetParam().GetValueStr(0));
		m_Config.NextValue();
	}
	*/

	//REFLEJO
	//Reflejo = MOTextures->GetTexture("emergente2/reflejo.tga");

	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj,GLU_FILL);
	gluQuadricNormals(qobj,GLU_SMOOTH);
	gluQuadricTexture(qobj,GL_TRUE);
	
	//Image = Images[0];
    if(preconfig.GetPreConfNum() > 0)
        preconfig.PreConfFirst( GetConfig());

	return true;
}

void moEffectRegularSolid::Draw( moTempo* tempogral,moEffectState* parentstate)
{
	MOint Solido, Image;

    PreDraw( tempogral, parentstate);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glColor4f(  m_Config.GetParam(color).GetValueFlt(MO_RED)*state.tint,
				m_Config.GetParam(color).GetValueFlt(MO_GREEN)*state.tint,
                m_Config.GetParam(color).GetValueFlt(MO_BLUE)*state.tint,
                m_Config.GetParam(color).GetValueFlt(MO_ALPHA)*state.alpha);
			

	Solido = m_Config.GetCurrentValue(solid);
	Image = m_Config.GetCurrentValue(texture);
	//Cinta = m_Config.GetCurrentValue(cintas);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT);

	//DIBUJO LA ESFERA(planeta)
	/*

	*/

	/*gluSphere(
    GLUquadric          *qobj, 
    GLdouble            radius, 
    GLint               slices, 
    GLint               stacks);	
	*/

	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
	glTranslatef(0.0,0.0,-2);
	glRotatef( state.tempo.ang*togra*0.25, 0.0, 1.0, 0.0 );	
	glScalef(   mov.movz(m_Config.GetParam(scalex), state.tempo),
            mov.movz(m_Config.GetParam(scaley), state.tempo),
            mov.movz(m_Config.GetParam(scalez), state.tempo));
	glScalef(0.5,0.5,0.5);
	glBindTexture(GL_TEXTURE_2D, Images.GetGLId(Image));	

	if(Solido==0) { 
		  glBegin(GL_QUADS);
			glTexCoord2f(1,0);
			glVertex3f(-1,-1,-1);
			glTexCoord2f(0,0);
			glVertex3f(1,-1,-1);
			glTexCoord2f(0,1);
			glVertex3f(1,1,-1);
			glTexCoord2f(1,1);
			glVertex3f(-1,1,-1);
		  glEnd();

		  glBegin(GL_QUADS);
			glTexCoord2f(1,0);
			glVertex3f(-1,1,-1);
			glTexCoord2f(0,0);
			glVertex3f(1,1,-1);
			glTexCoord2f(0,1);
			glVertex3f(1,1,1);
			glTexCoord2f(1,1);
			glVertex3f(-1,1,1);
		  glEnd();

		  glBegin(GL_QUADS);
			glTexCoord2f(1,1);
			glVertex3f(-1,1,1);
			glTexCoord2f(0,1);
			glVertex3f(-1,1,-1);
			glTexCoord2f(0,0);
			glVertex3f(-1,-1,-1);
			glTexCoord2f(1,0);
			glVertex3f(-1,-1,1);
		  glEnd();

		  glBegin(GL_QUADS);
			glTexCoord2f(0,1);
			glVertex3f(1,1,1);
			glTexCoord2f(1,1);
			glVertex3f(1,1,-1);
			glTexCoord2f(1,0);
			glVertex3f(1,-1,-1);
			glTexCoord2f(0,0);
			glVertex3f(1,-1,1);
		  glEnd();

		  glBegin(GL_QUADS);
			glTexCoord2f(1,1);
			glVertex3f(-1,-1,-1);
			glTexCoord2f(0,1);
			glVertex3f(1,-1,-1);
			glTexCoord2f(0,0);
			glVertex3f(1,-1,1);
			glTexCoord2f(1,0);
			glVertex3f(-1,-1,1);
		  glEnd();

		  glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(-1,-1,1);
			glTexCoord2f(1,0);
			glVertex3f(1,-1,1);
			glTexCoord2f(1,1);
			glVertex3f(1,1,1);
			glTexCoord2f(0,1);
			glVertex3f(-1,1,1);
		  glEnd();
		  } else {
		  glRotatef( 90, 1.0, 0.0, 0.0 );
		  //base
		  glBegin(GL_QUADS);
			glTexCoord2f(1,0);
			glVertex3f(-1,-1,-1);
			glTexCoord2f(0,0);
			glVertex3f(1,-1,-1);
			glTexCoord2f(0,1);
			glVertex3f(1,1,-1);
			glTexCoord2f(1,1);
			glVertex3f(-1,1,-1);
		  glEnd();

		  //lado1
		  glBegin(GL_TRIANGLES);
			glTexCoord2f(1,0);
			glVertex3f(-1,-1,-1);
			glTexCoord2f(0,0);
			glVertex3f(1,-1,-1);
			glTexCoord2f(0,1);
			glVertex3f(0,0,0.40);

			glTexCoord2f(0,0);
			glVertex3f(1,-1,-1);
			glTexCoord2f(1,1);
			glVertex3f(1,1,-1);
			glTexCoord2f(0,1);
			glVertex3f(0,0,0.40);

			glTexCoord2f(0,1);
			glVertex3f(1,1,-1);
			glTexCoord2f(1,1);
			glVertex3f(-1,1,-1);
			glTexCoord2f(0,1);
			glVertex3f(0,0,0.40);

			glTexCoord2f(1,1);
			glVertex3f(-1,1,-1);
			glTexCoord2f(1,0);
			glVertex3f(-1,-1,-1);
			glTexCoord2f(0,0);
			glVertex3f(0,0,0.40);

		  glEnd();
	}

	//DIBUJO LA CINTA
/*
	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
	
	float ag,ageje;
	int i;

	ageje = m_Config.GetParam(axisangle).GetValueFlt(0);
	
	glBindTexture(GL_TEXTURE_2D, Cintas[Cinta]);
	glTranslatef(0.0,0.0,-2);
	glRotatef(state.tempo.ang*togra, 0.0, 1.0, 0.0 );	
	glRotatef(15+ageje*cos(state.tempo.ang*0.5),1.0,0.0,1.0);	
	glRotatef(state.tempo.ang*togra*0.25,0.0,1.0,0.0);//la cinta gira mas despacio
	glScalef(   mov.movz(m_Config.GetParam(scalex), state.tempo),
            mov.movz(m_Config.GetParam(scaley), state.tempo),
            mov.movz(m_Config.GetParam(scalez), state.tempo));

	glBegin( GL_QUAD_STRIP);
		for(i=0;i<31;i++) {
			ag =(float) i *((float)(2*pi / 30.0));
			glTexCoord2f( 1.0 - i*(1.0/30.0), 0.0);
			glVertex3f( 1.0*cos(ag), 0.12, 1.0*sin(ag));
			glTexCoord2f( 1.0 - i*(1.0/30.0), 1.0);
			glVertex3f( 1.0*cos(ag), -0.12, 1.0*sin(ag));
		}
	glEnd();
*/	
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
}

MOboolean moEffectRegularSolid::Finish()
{
	Images.Finish();
    return PreFinish();
}
