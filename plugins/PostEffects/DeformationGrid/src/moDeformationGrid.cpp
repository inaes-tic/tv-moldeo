/*******************************************************************************

                               moDeformationGrid.cpp

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

#include "moDeformationGrid.h"

//========================
//  Factory
//========================

moDeformationGridFactory* m_DeformationGridFactory = NULL;

MO_PLG_API moPostEffectFactory* CreatePostEffectFactory(){
	if (m_DeformationGridFactory==NULL)
		m_DeformationGridFactory = new moDeformationGridFactory();
	return (moPostEffectFactory*) m_DeformationGridFactory;
}

MO_PLG_API void DestroyPostEffectFactory(){
	delete m_DeformationGridFactory;
	m_DeformationGridFactory = NULL;
}

moPostEffect*  moDeformationGridFactory::Create() {
	return new moDeformationGrid();
}

void moDeformationGridFactory::Destroy(moPostEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moDeformationGrid::moDeformationGrid() {

	SetName("deformationgrid");

}

moDeformationGrid::~moDeformationGrid() {
	Finish();
}

MOboolean moDeformationGrid::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( DEFORMATIONGRID_ALPHA, moText("alpha") );
	moDefineParamIndex( DEFORMATIONGRID_COLOR, moText("color") );
	moDefineParamIndex( DEFORMATIONGRID_SYNC, moText("syncro") );
	moDefineParamIndex( DEFORMATIONGRID_PHASE, moText("phase") );
	moDefineParamIndex( DEFORMATIONGRID_TEXTURE, moText("texture") );

	moDefineParamIndex( DEFORMATIONGRID_WIDTH, moText("width") );
	moDefineParamIndex( DEFORMATIONGRID_HEIGHT, moText("height") );
	moDefineParamIndex( DEFORMATIONGRID_POINTS, moText("points") );
	moDefineParamIndex( DEFORMATIONGRID_TEXCOORD_X1, moText("texcoord_x1") );
	moDefineParamIndex( DEFORMATIONGRID_TEXCOORD_Y1, moText("texcoord_y1") );
	moDefineParamIndex( DEFORMATIONGRID_TEXCOORD_X2, moText("texcoord_x2") );
	moDefineParamIndex( DEFORMATIONGRID_TEXCOORD_Y2, moText("texcoord_y2") );

	moDefineParamIndex( DEFORMATIONGRID_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( DEFORMATIONGRID_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( DEFORMATIONGRID_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( DEFORMATIONGRID_ROTATEX, moText("rotatex") );
	moDefineParamIndex( DEFORMATIONGRID_ROTATEY, moText("rotatey") );
	moDefineParamIndex( DEFORMATIONGRID_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( DEFORMATIONGRID_SCALEX, moText("scalex") );
	moDefineParamIndex( DEFORMATIONGRID_SCALEY, moText("scaley") );
	moDefineParamIndex( DEFORMATIONGRID_SCALEZ, moText("scalez") );

	moDefineParamIndex( DEFORMATIONGRID_SHOWGRID, moText("showgrid") );
	moDefineParamIndex( DEFORMATIONGRID_CLEAR, moText("clear") );
	moDefineParamIndex( DEFORMATIONGRID_PRECISION, moText("precision") );
	moDefineParamIndex( DEFORMATIONGRID_MODE, moText("mode") );

    m_Width = 0;
    m_Height = 0;
    pImage = NULL;
    m_Points = NULL;
    m_TPoints = NULL;

    UpdateParameters();

    m_Selector_I = 0;
    m_Selector_J = 0;
    m_Modo = (moDeformationGridMode)m_Config.Int( moR(DEFORMATIONGRID_MODE) );

	return true;
}

void moDeformationGrid::UpdateParameters() {

    int p_Width;
    int p_Height;

    p_Width = m_Width;
    p_Height = m_Height;

    m_Width = m_Config.Int( moR(DEFORMATIONGRID_WIDTH) );
    m_Height = m_Config.Int( moR(DEFORMATIONGRID_HEIGHT) );

    m_Precision =  m_Config.Double( moR(DEFORMATIONGRID_PRECISION) );
    showgrid = m_Config.Int(moR(DEFORMATIONGRID_SHOWGRID));
    clear = m_Config.Int( moR(DEFORMATIONGRID_CLEAR) );

    pImage = (moTexture*) m_Config[moR(DEFORMATIONGRID_TEXTURE)].GetData()->Pointer();
    PosTextX0 = m_Config[moR(DEFORMATIONGRID_TEXCOORD_X1)].GetData()->Fun()->Eval(state.tempo.ang);
    PosTextX1 = m_Config[moR(DEFORMATIONGRID_TEXCOORD_X2)].GetData()->Fun()->Eval(state.tempo.ang) * ( pImage!=NULL ? pImage->GetMaxCoordS() :  1.0 );
    PosTextY0 = m_Config[moR(DEFORMATIONGRID_TEXCOORD_Y1)].GetData()->Fun()->Eval(state.tempo.ang);
    PosTextY1 = m_Config[moR(DEFORMATIONGRID_TEXCOORD_Y2)].GetData()->Fun()->Eval(state.tempo.ang) * ( pImage!=NULL ? pImage->GetMaxCoordT() :  1.0 );


    moParam& PointParam = m_Config[moR(DEFORMATIONGRID_POINTS)];
    int nsaved = PointParam.GetValue().GetSubValueCount();

    if (m_Width!=p_Width || m_Height!=p_Height) {

        if ( nsaved == 0 || nsaved!=m_Width*m_Height*4 ) {

            MODebug2->Log(moText("moDeformationGrid::UpdateParameters ") + moText(" craeting new set of points") + IntToStr(m_Width*m_Height) );

            ///Es una nueva configuracion , generamos los puntos que modificaremos....

            ///creamos el nuevo array pero ojo!!!!!
            ///si ya existen datos y qrmos generar mas puntos , deberiamos interpolar aquellos nuevos a los viejos!!!!
            if (m_Points) {
                delete [] m_Points;
            }
            m_Points = new moVector2f [ m_Width * m_Height ];
            if (m_TPoints) {
                delete [] m_TPoints;
            }
            m_TPoints = new moVector2f [ m_Width * m_Height ];

            if ( nsaved>0 && nsaved!=m_Width*m_Height*4 ) {
                ///copy points????
                ///interpolating?!!?! ugggglyyyy

                ///reset for now or Duplicate.... i like duplicate!!! or divide!!!!
                ResetPuntos();
            } else {
                ResetPuntos();
            }

            ///saving to a new value in the config file!!!!
            moValue myPoints;
            moValueBase mBase;
            mBase.SetType( MO_VALUE_NUM_FLOAT );

            for(int j =0 ; j< m_Height; j++) {
                for(int i =0 ; i< m_Width; i++) {
                    mBase.SetFloat(  m_Points[i+j*m_Width].X() );
                    myPoints.AddSubValue( mBase );

                    mBase.SetFloat( m_Points[i+j*m_Width].Y() );
                    myPoints.AddSubValue( mBase );

                    mBase.SetFloat( m_TPoints[i+j*m_Width].X() );
                    myPoints.AddSubValue( mBase );

                    mBase.SetFloat( m_TPoints[i+j*m_Width].Y() );
                    myPoints.AddSubValue( mBase );

                }
            }
            ///Finally we add the points
            PointParam.AddValue( myPoints );
            m_PointsActualIndex = PointParam.GetValuesCount() - 1;
            PointParam.SetIndexValue( m_PointsActualIndex );

            ///And save!!! or not ?!

        } else if ( nsaved==m_Width*m_Height*4 ) {

            MODebug2->Log(moText("moDeformationGrid::UpdateParameters ") + moText(" loading points from config file: ") + IntToStr(m_Width*m_Height) );

            if (m_Points) {
                delete [] m_Points;
            }
            m_Points = new moVector2f [ m_Width * m_Height ];
            if (m_TPoints) {
                delete [] m_TPoints;
            }
            m_TPoints = new moVector2f [ m_Width * m_Height ];

            moValue& myPoints = PointParam.GetValue();
            m_PointsActualIndex = PointParam.GetIndexValue();

            ///LOAD from config file!!!!! surely from first time, it could be that we didnt match te correcte param value with our width and height too
            ///if you have a width and height, and you choose a correct config value, it will works???
            for(int j =0 ; j< m_Height; j++) {
                for(int i =0 ; i< m_Width; i++) {
                    m_Points[i+j*m_Width].X()= myPoints.GetSubValue( (i + j*m_Width)*4 ).Float();
                    m_Points[i+j*m_Width].Y()= myPoints.GetSubValue( (i + j*m_Width)*4 + 1 ).Float();
                    m_TPoints[i+j*m_Width].X()= myPoints.GetSubValue( (i + j*m_Width)*4 + 2 ).Float();
                    m_TPoints[i+j*m_Width].Y()= myPoints.GetSubValue( (i + j*m_Width)*4 + 3 ).Float();
                }
            }


        }
    }



}

#define KLT_TRACKED           0
#define KLT_NOT_FOUND        -1
#define KLT_SMALL_DET        -2
#define KLT_MAX_ITERATIONS   -3
#define KLT_OOB              -4
#define KLT_LARGE_RESIDUE    -5

void moDeformationGrid::Draw( moTempo* tempogral, moEffectState* parentstate )
{
    MOint indeximage;

    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();



    UpdateParameters();

    PreDraw( tempogral, parentstate);

    if (clear) {
        glClearColor(0.0,0.0,0.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(-0.5,0.5,-0.5*h/w,0.5*h/w,-1,1);            // Set Up An Ortho Screen


/*
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

*/
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View



	glTranslatef(   m_Config[moR(DEFORMATIONGRID_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(DEFORMATIONGRID_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(DEFORMATIONGRID_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));

	glRotatef(  m_Config[moR(DEFORMATIONGRID_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(DEFORMATIONGRID_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(DEFORMATIONGRID_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );

	glScalef(   m_Config[moR(DEFORMATIONGRID_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(DEFORMATIONGRID_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(DEFORMATIONGRID_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang));


    SetColor( m_Config[moR(DEFORMATIONGRID_COLOR)][MO_SELECTED], m_Config[moR(DEFORMATIONGRID_ALPHA)][MO_SELECTED], state );

    glEnable( GL_TEXTURE_2D );

    glBindTexture( GL_TEXTURE_2D,m_Config[moR(DEFORMATIONGRID_TEXTURE)].GetData()->GetGLId(&state.tempo) );

    if (m_Points && m_TPoints)
    for(int j=0; j< (m_Height-1); j++) {
			glBegin(GL_QUAD_STRIP);
			for( int i=0; i < m_Width; i++ ) {

				glTexCoord2f( m_TPoints[i + j * m_Width].X(), m_TPoints[i + j * m_Width].Y() );
				glVertex2f( m_Points[i + j * m_Width].X(), m_Points[i + j * m_Width].Y() );

				glTexCoord2f( m_TPoints[i + (j+1) * m_Width].X(), m_TPoints[i + (j+1) * m_Width].Y() );
				glVertex2f( m_Points[i + (j+1) * m_Width].X(), m_Points[i + (j+1) * m_Width].Y() );
			}
			glEnd();
	}

			if (showgrid>0) {
			    glDisable( GL_TEXTURE_2D );
			    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                for(int j=0; j< m_Height; j++) {
                    for( int i=0; i < m_Width; i++ ) {

                        glColor4f( 0.0, 1.0, 0.0, 1.0);
                        glBegin(GL_QUADS);
                            glVertex2f( m_Points[i + j * m_Width].X() -0.002, m_Points[i + j * m_Width].Y() +0.002);
                            glVertex2f( m_Points[i + j * m_Width].X()+0.002, m_Points[i + j * m_Width].Y() +0.002);
                            glVertex2f( m_Points[i + j * m_Width].X()+0.002, m_Points[i + j * m_Width].Y()  -0.002 );
                            glVertex2f( m_Points[i + j * m_Width].X() -0.002, m_Points[i + j * m_Width].Y()  -0.002 );
                        glEnd();


                        if ( m_Selector_I == i && m_Selector_J  == j ) {
                            glColor4f( 1.0, 1.0, 0.0, 1.0);
                            glBegin(GL_QUADS);
                                glVertex2f( m_Points[i + j * m_Width].X() -0.01, m_Points[i + j * m_Width].Y() +0.01);
                                glVertex2f( m_Points[i + j * m_Width].X()+0.01, m_Points[i + j * m_Width].Y() +0.01);
                                glVertex2f( m_Points[i + j * m_Width].X()+0.01, m_Points[i + j * m_Width].Y()  -0.01 );
                                glVertex2f( m_Points[i + j * m_Width].X() -0.01, m_Points[i + j * m_Width].Y()  -0.01 );
                            glEnd();
                        }

                    }
                }
                glEnable( GL_TEXTURE_2D );
			}

/*
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
    */

    //MODebug2->Push(IntToStr(showgrid));

    if (showgrid>0) {

            ///DRAW THE REFERENCE GRID TO SEE THE DEFORMATION RESULT

            /***
            *   for each vertex in the value index selected for the POINTS parameters
            *   draw
            */

            glColor4f(1.0,1.0,1.0,1.0);
            glDisable( GL_TEXTURE_2D );

            glLineWidth( 1.0 );

           glBegin (GL_LINES);
              glVertex3f (-0.25, h/w / 4, 0);
              glVertex3f (0.25, -h/w / 4, 0);
           glEnd ();

           glBegin (GL_LINES);
              glVertex3f (h/w / 4, h/w / 4, 0);
              glVertex3f (-h/w / 4, -h/w / 4, 0);
           glEnd ();

            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

            glBegin (GL_QUADS);
              glVertex3f (-0.5, h/w / 2, 0);
              glVertex3f (0.5, h/w / 2, 0);
              glVertex3f (0.5, -h/w / 2, 0);
              glVertex3f (-0.5, -h/w / 2, 0);

            glEnd ();
            glScalef(1.2,1.2,0);
            glBegin (GL_QUADS);
              glVertex3f (-0.5, h/w / 2, 0);
              glVertex3f (0.5, h/w / 2, 0);
              glVertex3f (0.5, -h/w / 2, 0);
              glVertex3f (-0.5, -h/w / 2, 0);

            glEnd ();
    }

    glLineWidth( 1.0 );

    glEnable( GL_TEXTURE_2D );

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

}

MOboolean moDeformationGrid::Finish()
{
    return PreFinish();
}


moConfigDefinition *
moDeformationGrid::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, DEFORMATIONGRID_TEXTURE, moValue("Default", "TXT") );

    p_configdefinition->Add( moText("width"), MO_PARAM_NUMERIC, DEFORMATIONGRID_WIDTH, moValue("5", "INT").Ref() );
    p_configdefinition->Add( moText("height"), MO_PARAM_NUMERIC, DEFORMATIONGRID_HEIGHT, moValue("5", "INT").Ref() );
	p_configdefinition->Add( moText("points"), MO_PARAM_NUMERIC, DEFORMATIONGRID_POINTS);

	p_configdefinition->Add( moText("texcoord_x1"), MO_PARAM_FUNCTION, DEFORMATIONGRID_TEXCOORD_X1, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("texcoord_y1"), MO_PARAM_FUNCTION, DEFORMATIONGRID_TEXCOORD_X2, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("texcoord_x2"), MO_PARAM_FUNCTION, DEFORMATIONGRID_TEXCOORD_Y1, moValue("1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("texcoord_y2"), MO_PARAM_FUNCTION, DEFORMATIONGRID_TEXCOORD_Y2, moValue("1.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, DEFORMATIONGRID_TRANSLATEX, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, DEFORMATIONGRID_TRANSLATEY, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, DEFORMATIONGRID_TRANSLATEZ, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, DEFORMATIONGRID_ROTATEX, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, DEFORMATIONGRID_ROTATEY, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, DEFORMATIONGRID_ROTATEZ, moValue("0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, DEFORMATIONGRID_SCALEX, moValue("1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, DEFORMATIONGRID_SCALEY, moValue("1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, DEFORMATIONGRID_SCALEZ, moValue("1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("showgrid"), MO_PARAM_NUMERIC, DEFORMATIONGRID_SHOWGRID, moValue("1.0", "NUM").Ref() );
	p_configdefinition->Add( moText("clear"), MO_PARAM_NUMERIC, DEFORMATIONGRID_CLEAR, moValue("1", "NUM").Ref() );
	p_configdefinition->Add( moText("precision"), MO_PARAM_NUMERIC, DEFORMATIONGRID_PRECISION, moValue("0.01", "FLOAT").Ref() );
	p_configdefinition->Add( moText("mode"), MO_PARAM_NUMERIC, DEFORMATIONGRID_MODE, moValue("0", "NUM").Ref() );

	return p_configdefinition;
}

void moDeformationGrid::EscalarPuntos( bool horizontal, float escala ) {

        float x = 0.0,y = 0.0;
        float newX = 0.0,newY = 0.0;
        float sumaX = 0.0,sumaY = 0.0;


        if (horizontal) {

            ///calculamos el centro de la linea
            for(int i=0; i < m_Width; i++ ) {
                sumaX+= m_Points[ i + m_Width * m_Selector_J ].X();
            }
            sumaX/= m_Width;

            ///escalamos
            for( int i = 0; i < m_Width; i++ ) {

                x = m_Points[ i + m_Width * m_Selector_J ].X();
                y = m_Points[ i + m_Width * m_Selector_J ].Y();

                ///escalamos
                newX = ( x - sumaX ) * escala;
                m_Points[ i + m_Width * m_Selector_J ] = moVector2f( sumaX + newX, y );
                SavePoint( i, m_Selector_J );

            }
        } else {

            ///calculamos el centro de la linea
            for(int j=0; j < m_Height; j++ ) {
                sumaY+= m_Points[ m_Selector_I + m_Width * j ].Y();
            }
            sumaY/= m_Height;

            ///escalamos
            for( int j = 0, index = m_Selector_I; j < m_Height; j++, index = m_Selector_I + m_Width * j) {

                x = m_Points[ index ].X();
                y = m_Points[ index ].Y();

                ///escalamos
                newY = ( y - sumaY ) * escala;
                m_Points[ index ] = moVector2f( x,  sumaY + newY );
                SavePoint( m_Selector_I, j );

            }

        }


}

void moDeformationGrid::ResetPuntos() {

    float cx,incx;
    float cy,incy;

    float inctx,incty;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    bool saveto = false;

    inctx = (PosTextX1 - PosTextX0) / (float)(m_Width-1);
    incty = (PosTextY1 - PosTextY0) / (float)(m_Height-1);

    cx = -1.0 / 2;
    incx =  1.0 / (float)( m_Width - 1);

    cy = -(1.0 * h / w ) / 2;
    incy =  (1.0 * h / w) / (float)(m_Height - 1);

    if (m_Config[moR(DEFORMATIONGRID_POINTS)].GetValue().GetSubValueCount()>0) saveto = true;

    for(int j=0; j < m_Height; j++)
        for(int i=0; i < m_Width; i++) {

            m_Points[ i + j*m_Width ] = moVector2f( cx + i*incx, cy + j*incy );
            m_TPoints[ i + j*m_Width ] = moVector2f( PosTextX0 + inctx*i, PosTextY0 + incty*j );

            if (saveto) SavePoint( i, j );

        }

}


void moDeformationGrid::SavePoint( int i, int j ) {

    moValue& myPoints = m_Config[moR(DEFORMATIONGRID_POINTS)].GetValue();
    int index = (i + j*m_Width)*4;
    int index2 = i + j*m_Width;

    myPoints.GetSubValue( index).SetFloat(m_Points[index2].X());
    myPoints.GetSubValue( index+ 1 ).SetFloat(m_Points[index2].Y());
    myPoints.GetSubValue( index + 2).SetFloat(m_TPoints[index2].X());
    myPoints.GetSubValue( index + 3).SetFloat(m_TPoints[index2].Y());

}

void moDeformationGrid::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moVector2f promedio(0.0,0.0);
    float sumax = 0.0;
    moVector2f New;
    float x;


	///este lo comentamos, sino llama nuevamente a Update(Events*) ....
	///moEffect::Interaction( IODeviceManager );

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

				case DEFORMATIONGRID_MODO_1:
                    m_Modo = PUNTO;
					MODebug2->Push(moText("DEFORMATIONGRID MODO 1"));
					break;
				case DEFORMATIONGRID_MODO_2:
                    m_Modo = LINEA;
					MODebug2->Push(moText("DEFORMATIONGRID MODO 2"));
					break;
				case DEFORMATIONGRID_MODO_3:
					MODebug2->Push(moText("DEFORMATIONGRID MODO 3"));
					break;

				case DEFORMATIONGRID_SEL_IZQ: /// A
					( m_Selector_I >0 ) ? m_Selector_I-- : m_Selector_I = m_Selector_I;
					MODebug2->Push( moText("Selector Izquierda I: ") + IntToStr(m_Selector_I));
					break;
				case DEFORMATIONGRID_SEL_DER: /// D
					( m_Selector_I < (m_Width-1) ) ? m_Selector_I++ : m_Selector_I = m_Selector_I;
					MODebug2->Push( moText("Selector Derecha I: ") + IntToStr(m_Selector_I));
					break;
				case DEFORMATIONGRID_SEL_ARR: /// W
					( m_Selector_J < (m_Height-1)  ) ? m_Selector_J++ : m_Selector_J = m_Selector_J;
					MODebug2->Push( moText("Selector Arriba J: ") + IntToStr(m_Selector_J));
					break;
				case DEFORMATIONGRID_SEL_ABA: /// S
					( m_Selector_J >0  ) ? m_Selector_J-- : m_Selector_J = m_Selector_J;
					MODebug2->Push( moText("Selector Abajo J: ") + IntToStr(m_Selector_J));
					break;



                case DEFORMATIONGRID_CURSOR_IZQ: /// <-
                    switch( m_Modo ) {
                        case PUNTO:
                            m_Points[ m_Selector_I + m_Width * m_Selector_J ].X()-= m_Precision;
                            SavePoint( m_Selector_I, m_Selector_J );
                            break;
                        ///proporcional
                        case LINEA:
                            EscalarPuntos( true, 1.0 - m_Precision );
                            break;
                    }
                    New = m_Points[ m_Selector_I + m_Width * m_Selector_J ];
					MODebug2->Push( moText("Cursor Izquierda: ") + FloatToStr(New.X()) );
					break;
				case DEFORMATIONGRID_CURSOR_DER: /// ->
                    switch( m_Modo ) {
                        case PUNTO:
                            m_Points[ m_Selector_I + m_Width * m_Selector_J ].X()+= m_Precision;
                            SavePoint( m_Selector_I, m_Selector_J );
                            break;
                        ///proporcional
                        case LINEA:
                            EscalarPuntos( true, 1.0 + m_Precision );
                            break;
                    }
					New = m_Points[ m_Selector_I + m_Width * m_Selector_J ];
					MODebug2->Push( moText("Cursor Derecha: ") + FloatToStr(New.X()) );
					break;
				case DEFORMATIONGRID_CURSOR_ARR: /// up arrow
                    switch( m_Modo ) {
                        case PUNTO:
                            m_Points[ m_Selector_I + m_Width * m_Selector_J ].Y()+= m_Precision;
                            SavePoint( m_Selector_I, m_Selector_J );
                            break;
                        ///proporcional
                        case LINEA:
                            EscalarPuntos( false, 1.0 - m_Precision );
                            break;
                    }
                    New = m_Points[ m_Selector_I + m_Width * m_Selector_J ];
					MODebug2->Push( moText("Cursor Arriba: ") + FloatToStr(New.Y()) );
					break;
				case DEFORMATIONGRID_CURSOR_ABA: /// down arrow
                    switch( m_Modo ) {
                        case PUNTO:
                            m_Points[ m_Selector_I + m_Width * m_Selector_J ].Y()-= m_Precision;
                            SavePoint( m_Selector_I, m_Selector_J );
                            break;
                        ///proporcional
                        case LINEA:
                            EscalarPuntos( false, 1.0 + m_Precision );
                            break;
                    }
					New = m_Points[ m_Selector_I + m_Width * m_Selector_J ];
					MODebug2->Push( moText("Cursor Abajo: ") + FloatToStr(New.Y()) );
					break;

                case DEFORMATIONGRID_GUARDAR:
                    if (m_Config.SaveConfig()==MO_CONFIG_OK) {
                        MODebug2->Push( moText("Guardose exitosamente la configuracion"));
                    } else {
                        MODebug2->Error( moText("Hubo un problema al cargar el salvar el config"));
                    }
                    //salvar
                    break;

                case DEFORMATIONGRID_RESET:
                    ResetPuntos();
                    MODebug2->Push( moText("Reset Puntos: "));
                    break;
			}
		temp = temp->next;
		}
	}

}


void
moDeformationGrid::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);


}

