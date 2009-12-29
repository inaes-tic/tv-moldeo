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
	moDefineParamIndex( CALIBRATIONGRID_SYNC, moText("syncro") );
	moDefineParamIndex( CALIBRATIONGRID_PHASE, moText("phase") );
	moDefineParamIndex( CALIBRATIONGRID_TEXTURE, moText("texture") );
	moDefineParamIndex( CALIBRATIONGRID_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( CALIBRATIONGRID_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( CALIBRATIONGRID_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( CALIBRATIONGRID_ROTATEX, moText("rotatex") );
	moDefineParamIndex( CALIBRATIONGRID_ROTATEY, moText("rotatey") );
	moDefineParamIndex( CALIBRATIONGRID_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( CALIBRATIONGRID_SCALEX, moText("scalex") );
	moDefineParamIndex( CALIBRATIONGRID_SCALEY, moText("scaley") );
	moDefineParamIndex( CALIBRATIONGRID_SCALEZ, moText("scalez") );
	moDefineParamIndex( CALIBRATIONGRID_SHOWGRID, moText("showgrid") );

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

    int showgrid = 0;

    showgrid = m_Config[moR(CALIBRATIONGRID_SHOWGRID)][MO_SELECTED][0].GetData()->Int();

    PreDraw( tempogral, parentstate);

    glClearColor(0.0,0.0,0.0,1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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



	glTranslatef(   m_Config[moR(CALIBRATIONGRID_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(CALIBRATIONGRID_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(CALIBRATIONGRID_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));

	glRotatef(  m_Config[moR(CALIBRATIONGRID_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(CALIBRATIONGRID_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(CALIBRATIONGRID_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
	glScalef(   m_Config[moR(CALIBRATIONGRID_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(CALIBRATIONGRID_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(CALIBRATIONGRID_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang));


    SetColor( m_Config[moR(CALIBRATIONGRID_COLOR)][MO_SELECTED], m_Config[moR(CALIBRATIONGRID_ALPHA)][MO_SELECTED], state );

    glEnable( GL_TEXTURE_2D );

    moTexture* pImage = (moTexture*) m_Config[moR(CALIBRATIONGRID_TEXTURE)].GetData()->Pointer();

    PosTextX0 = 0.0;
	PosTextX1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordS() :  1.0 );
    PosTextY0 = 0.0;
    PosTextY1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordT() :  1.0 );

    glBindTexture( GL_TEXTURE_2D, m_Config[moR(CALIBRATIONGRID_TEXTURE)].GetData()->GetGLId(&state.tempo) );

    glBegin (GL_QUADS);
          glTexCoord2f( PosTextX0, PosTextY1);
          glVertex3f (-7.69, 5.77, 0);
		glTexCoord2f( PosTextX1, PosTextY1);
          glVertex3f (7.69, 5.77, 0);
		glTexCoord2f( PosTextX1, PosTextY0);
          glVertex3f (7.69, -5.77, 0);
		glTexCoord2f( PosTextX0, PosTextY0);
          glVertex3f (-7.69, -5.77, 0);
    glEnd();

    //MODebug2->Push(IntToStr(showgrid));

    if (showgrid>0) {

            ///DRAW THE REFERENCE GRID TO SEE THE DEFORMATION RESULT

            glColor4f(1.0,1.0,1.0,1.0);
            glDisable( GL_TEXTURE_2D );

            glLineWidth( 8.0 );

           glBegin (GL_LINES);
              glVertex3f (-3.8, 3.8, 0);
              glVertex3f (3.8, -3.8, 0);
           glEnd ();

           glBegin (GL_LINES);
              glVertex3f (3.8, 3.8, 0);
              glVertex3f (-3.8, -3.8, 0);
           glEnd ();

            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

            glBegin (GL_QUADS);
              glVertex3f (-7.69, 5.77, 0);
              glVertex3f (7.69, 5.77, 0);
              glVertex3f (7.69, -5.77, 0);
              glVertex3f (-7.69, -5.77, 0);

            glEnd ();
            glBegin (GL_QUADS);
              glVertex3f (-7.69*0.5, 5.77*0.5, 0);
              glVertex3f (7.69*0.5, 5.77*0.5, 0);
              glVertex3f (7.69*0.5, -5.77*0.5, 0);
              glVertex3f (-7.69*0.5, -5.77*0.5, 0);

            glEnd ();
    }

    glLineWidth( 1.0 );

    glEnable( GL_TEXTURE_2D );

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
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, CALIBRATIONGRID_TEXTURE, moValue("Default", "TXT") );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, CALIBRATIONGRID_TRANSLATEX, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, CALIBRATIONGRID_TRANSLATEY, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, CALIBRATIONGRID_TRANSLATEZ, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, CALIBRATIONGRID_ROTATEX, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, CALIBRATIONGRID_ROTATEY, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, CALIBRATIONGRID_ROTATEZ, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, CALIBRATIONGRID_SCALEX, moValue("1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, CALIBRATIONGRID_SCALEY, moValue("1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, CALIBRATIONGRID_SCALEZ, moValue("1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("showgrid"), MO_PARAM_NUMERIC, CALIBRATIONGRID_SHOWGRID, moValue("1.0", "NUM").Ref() );
	return p_configdefinition;
}
