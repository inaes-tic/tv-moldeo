/*******************************************************************************

                               moCalibrationGrid.cpp

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
  Gustavo Orrillo

*******************************************************************************/

#include "moCalibrationGrid.h"

//========================
//  Factory
//========================

moCalibrationGridFactory* m_CalibrationGridFactory = NULL;

MO_PLG_API moPostEffectFactory* CreatePostEffectFactory(){
	if (m_CalibrationGridFactory==NULL)
		m_CalibrationGridFactory = new moCalibrationGridFactory();
	return (moPostEffectFactory*) m_CalibrationGridFactory;
}

MO_PLG_API void DestroyPostEffectFactory(){
	delete m_CalibrationGridFactory;
	m_CalibrationGridFactory = NULL;
}

moPostEffect*  moCalibrationGridFactory::Create() {
	return new moCalibrationGrid();
}

void moCalibrationGridFactory::Destroy(moPostEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moCalibrationGrid::moCalibrationGrid() {

	SetName("calibrationgrid");

}

moCalibrationGrid::~moCalibrationGrid() {
	Finish();
}

MOboolean moCalibrationGrid::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( CALIBRATIONGRID_ALPHA, moText("alpha") );
	moDefineParamIndex( CALIBRATIONGRID_COLOR, moText("color") );

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

void moCalibrationGrid::Draw( moTempo* tempogral, moEffectState* parentstate )
{
    MOint indeximage;
	MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    PreDraw( tempogral, parentstate);

    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
//	glOrtho(0,w,0,h,-1,1);                              // Set Up An Ortho Screen
    m_pResourceManager->GetGLMan()->SetPerspectiveView( w, h );

    glMatrixMode(GL_PROJECTION);
	gluLookAt(		0,
					0,
					10.0,
					0,
					0,
					0,
					0, 1, 0);


    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View

    glColor4f(1.0,1.0,1.0,1.0);
    glDisable( GL_TEXTURE_2D );

   glBegin (GL_LINES);
      glVertex3f (-0.5, 0.5, 0);
      glVertex3f (0.5, -0.5, 0);

   glEnd ();

   glBegin (GL_LINES);
      glVertex3f (0.5, 0.5, 0);
      glVertex3f (-0.5, -0.5, 0);
   glEnd ();

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glBegin (GL_QUADS);
      glVertex3f (-7.69, 5.77, 0);
      glVertex3f (7.69, 5.77, 0);
      glVertex3f (7.69, -5.77, 0);
      glVertex3f (-7.69, -5.77, 0);
    /*
      glVertex3f (-5, 5.7 , 0);
      glVertex3f (5.7, 5.7 , 0);
      glVertex3f (5.7, -5 , 0);
      glVertex3f (-5, -5, 0);
    */
    glEnd ();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

}

MOboolean moCalibrationGrid::Finish()
{
    return PreFinish();
}


moConfigDefinition *
moCalibrationGrid::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moPostEffect::GetDefinition( p_configdefinition );
	return p_configdefinition;
}
