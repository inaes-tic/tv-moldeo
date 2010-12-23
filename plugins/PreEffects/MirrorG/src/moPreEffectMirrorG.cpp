/*******************************************************************************

                            moPreEffectMirrorG.cpp

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

#include "moPreEffectMirrorG.h"

//========================
//  Factory
//========================

moPreEffectMirrorGFactory *m_PreEffectMirrorGFactory = NULL;

MO_PLG_API moPreEffectFactory* CreatePreEffectFactory(){
	if(m_PreEffectMirrorGFactory==NULL)
		m_PreEffectMirrorGFactory = new moPreEffectMirrorGFactory();
	return(moPreEffectFactory*) m_PreEffectMirrorGFactory;
}

MO_PLG_API void DestroyPreEffectFactory(){
	delete m_PreEffectMirrorGFactory;
	m_PreEffectMirrorGFactory = NULL;
}

moPreEffect*  moPreEffectMirrorGFactory::Create() {
	return new moPreEffectMirrorG();
}

void moPreEffectMirrorGFactory::Destroy(moPreEffect* fx) {
	delete fx;
}

//========================
//  PreEfecto
//========================

MOfloat
moPointG::SetDXDY(MOfloat _x, MOfloat _y) {
		MOfloat dist,vx,vy,vdx,vdy;
		vdx =(x-_x);
		vdy =(y-_y);
		dist = sqrt(vdx*vdx+vdy*vdy);
		if(dist!=0) vx = vdx / dist;//direccion en x normalizada
		else vx = 0;
		if(dist!=0) vy = vdy / dist;//direccion en y normalizada
		else vy = 0;
		if (atenuationfunction==moText("L")) atenuationfunctioni = 0;
		else if (atenuationfunction==moText("E")) atenuationfunctioni = 1;
		else if (atenuationfunction==moText("I")) atenuationfunctioni = 2;
		else atenuationfunctioni = -1;
		switch(atenuationfunctioni) {
			case 0:	  dx = amplitude*vx + amplitudeatenuation*vdx;
					  dy = amplitude*vy + amplitudeatenuation*vdy;
					  break;
			case 1:	  dx = amplitude*vx + amplitudeatenuation*vdx*dist;
					  dy = amplitude*vy + amplitudeatenuation*vdy*dist;
					  break;
			case 2:	  dx = amplitude*vx + amplitudeatenuation*vx/dist;
					  dy = amplitude*vy + amplitudeatenuation*vy/dist;
					  break;
			default:
					dx = 0.0;
					dy = 0.0;
				break;
		}
		return dist;
}


MOfloat
moLineG::SetDXDY(MOfloat _x, MOfloat _y) {
		MOfloat dist;
		MOfloat ps;//ps = producto scalar PO> et V>
		MOfloat dirx,diry,dirxn,diryn;

		ps =( _x - positionx ) * directionx +( _y - positiony ) * directiony ;
		dirx =( _x - positionx ) - directionx * ps;
		diry =( _y - positiony ) - directiony * ps;

		dist = sqrt(dirx*dirx+diry*diry);

		if(dist!=0) dirxn = dirx / dist;//direccion en x normalizada
		else dirxn = 0;
		if(dist!=0) diryn = diry / dist;//direccion en y normalizada
		else diryn = 0;
		if (atenuationfunction==moText("L")) atenuationfunctioni = 0;
		else if (atenuationfunction==moText("E")) atenuationfunctioni = 1;
		else atenuationfunctioni = -1;
		switch(atenuationfunctioni) {
			case 0:	  dx = amplitude*dirxn + amplitudeatenuation*dirx;
					  dy = amplitude*diryn + amplitudeatenuation*diry;
					  break;
			case 1:	  dx = amplitude*dirxn + amplitudeatenuation*dirx*dist;
					  dy = amplitude*diryn + amplitudeatenuation*diry*dist;
					  break;

			default:
					dx = 0.0;
					dy = 0.0;
				break;
		}
		return dist;
}

void
moLineG::Normalizar() {
	float dist;

	dist = sqrt(directionx*directionx + directiony*directiony);
	if(dist!=0.0f) {
		directionx = directionx / dist;
		directiony = directiony / dist;
	} else {
		directionx = 0;
		directiony = 0;
	}

}

float ppoint2[3 * 3 * 4] =
{
  -1.0, -1.0, -1.0, 1.0,
  0.0, -1.0, -1.0, 1.0,
  1.0, -1.0, -1.0, 1.0,

  -1.0, 0.0, -1.0, 1.0,
   0.0, 0.0, -1.0, 1.0,
  1.0, 0.0, -1.0, 1.0,

  -1.0, 1.0, -1.0, 1.0,
  0.0, 1.0, -1.0, 1.0,
  1.0, 1.0, -1.0, 1.0
};

float pcpoint2[3 * 3 * 4] =
{
  1.0, 1.0, 1.0, 1.0,
  1.0, 1.0, 1.0, 1.0,
  1.0, 1.0, 1.0, 1.0,

  1.0, 1.0, 1.0, 1.0,
  1.0, 1.0, 1.0, 1.0,
  1.0, 1.0, 1.0, 1.0,

  1.0, 1.0, 1.0, 1.0,
  1.0, 1.0, 1.0, 1.0,
  1.0, 1.0, 1.0, 1.0
};

float ptpoint2[3 * 3 * 2] =
{
  0.0, 0.0, 0.5, 0.0, 1.0 , 0.0,
  0.0, 0.5, 0.5, 0.5, 1.0 , 0.5,
  0.0, 1.0, 0.5, 1.0, 1.0 , 1.0
};

moPreEffectMirrorG::moPreEffectMirrorG() {
	SetName("mirrorg");
	DPoint2 = NULL;
}

moPreEffectMirrorG::~moPreEffectMirrorG() {
	Finish();
}

void
moPreEffectMirrorG::DrawPoints2(void) {
/*
  GLint i, j;
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(4);
  glBegin(GL_POINTS);
  for(i = 0; i < wpoint2; i++) {
    for(j = 0; j < hpoint2; j++) {
      glVertex4dv(&Point2[i * 4 * wpoint2 + j * 4]);
    }
  }
  glEnd();*/
}


MOboolean moPreEffectMirrorG::Init()
{
    if (!PreInit()) return false;

	int i;

    moDefineParamIndex( MIRRORG_ALPHA, moText("alpha") );
	moDefineParamIndex( MIRRORG_COLOR, moText("color") );
	moDefineParamIndex( MIRRORG_SYNCRO, moText("syncro") );
	moDefineParamIndex( MIRRORG_PHASE, moText("phase") );


	moDefineParamIndex( MIRRORG_BLENDING, moText("blending") );
	moDefineParamIndex( MIRRORG_ABERRATION, moText("aberration") );
	moDefineParamIndex( MIRRORG_POLYGONMODE, moText("polygonmode") );
	moDefineParamIndex( MIRRORG_GENERALX, moText("generalx") );
	moDefineParamIndex( MIRRORG_GENERALY, moText("generaly") );
	moDefineParamIndex( MIRRORG_DOTS, moText("dots") );
	moDefineParamIndex( MIRRORG_LINES, moText("lines") );

    // Hacer la inicializacion de este efecto en particular.
    // Seteos de OpenGL.
    // Seteos de Texturas.
	hpoint2 = VMINOR_ORDER;
	wpoint2 = hpoint2;

	//cantidad de vertices del plano
	npoint2 = hpoint2*wpoint2;

    //array de coordenadas de cada vertice
	DPoint2 = new MOfloat [npoint2*4];
	for(i=0;i<(npoint2*4);i++) {
		DPoint2[i] = 0.0;
	}

	//array de colores por cada vertice
	CPoint2 = new MOfloat [npoint2*4];
	for(i=0;i<(npoint2*4);i++) {
		CPoint2[i] = 1.0;
	}

	//array de coordenadas de texturas por cada vertice
	TPoint2 = new MOfloat [npoint2*2];
	for(i=0;i<(npoint2*2);i++) {
		TPoint2[i] = 0.0;
	}

	return true;
}

moConfigDefinition *
moPreEffectMirrorG::GetDefinition( moConfigDefinition *p_configdefinition ) {
	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, MIRRORG_BLENDING, moValue( "0", MO_VALUE_NUM ).Ref() );
	p_configdefinition->Add( moText("aberration"), MO_PARAM_FUNCTION, MIRRORG_ABERRATION, moValue( "1.02", MO_VALUE_FUNCTION).Ref());
	p_configdefinition->Add( moText("polygonmode"), MO_PARAM_POLYGONMODE, MIRRORG_POLYGONMODE, moValue( "0", MO_VALUE_NUM ).Ref() );
	p_configdefinition->Add( moText("generalx"), MO_PARAM_FUNCTION, MIRRORG_GENERALX, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("generaly"), MO_PARAM_FUNCTION, MIRRORG_GENERALY, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("dots"), MO_PARAM_COMPOSE, MIRRORG_DOTS, moValue( "0", "TXT") );
	p_configdefinition->Add( moText("lines"), MO_PARAM_COMPOSE, MIRRORG_LINES, moValue( "0", "TXT") );
	return p_configdefinition;
}

void
moPreEffectMirrorG::Calculate(void) {
    int i,j,l;

	//carga los valores de los puntos del config
	moValue& mValuePoints( m_Config[moR(MIRRORG_DOTS)].GetValue() );

	NpointsG = mValuePoints.GetSubValueCount() / (MO_MIRRORG_POINTG_FIXED+1);

	for(i=0;i<NpointsG;i++) {
        pointsG[i].amplitude = mValuePoints.GetSubValue( MO_MIRRORG_POINTG_AMPLITUDE + i*6).Fun()->Eval(state.tempo.ang);
        pointsG[i].atenuationfunction = mValuePoints.GetSubValue( MO_MIRRORG_POINTG_ATENUATION_FUNCTION+i*6).Text();
        pointsG[i].amplitudeatenuation = mValuePoints.GetSubValue( MO_MIRRORG_POINTG_ATENUATION_AMPLITUDE+i*6).Fun()->Eval(state.tempo.ang);
        pointsG[i].x = mValuePoints.GetSubValue( MO_MIRRORG_POINTG_X+i*6).Fun()->Eval(state.tempo.ang)*0.78125f + 0.390625;
        pointsG[i].y = mValuePoints.GetSubValue( MO_MIRRORG_POINTG_Y+i*6).Fun()->Eval(state.tempo.ang)*0.5859375f + 0.29296875;
        pointsG[i].fixed = mValuePoints.GetSubValue( MO_MIRRORG_POINTG_FIXED+i*6).Text();
	}

	//carga los valores de las lineas
	moValue& mValueLines( m_Config[moR(MIRRORG_LINES)].GetValue() );
	NlinesG = mValueLines.GetSubValueCount() / (MO_MIRRORG_LINEG_FIXED+1);

	for(i=0;i<NlinesG;i++) {
        linesG[i].amplitude = mValueLines.GetSubValue( MO_MIRRORG_LINEG_AMPLITUDE + i*8  ).Fun()->Eval(state.tempo.ang);
        linesG[i].atenuationfunction = mValueLines.GetSubValue( MO_MIRRORG_LINEG_ATENUATION_FUNCTION + i*8 ).Text();
        linesG[i].amplitudeatenuation = mValueLines.GetSubValue( MO_MIRRORG_LINEG_ATENUATION_AMPLITUDE + i*8 ).Fun()->Eval(state.tempo.ang);
        linesG[i].directionx = mValueLines.GetSubValue( MO_MIRRORG_LINEG_DIRECTIONX + i*8 ).Fun()->Eval(state.tempo.ang);
        linesG[i].directiony = mValueLines.GetSubValue( MO_MIRRORG_LINEG_DIRECTIONY + i*8 ).Fun()->Eval(state.tempo.ang);
        linesG[i].Normalizar();
        linesG[i].positionx = mValueLines.GetSubValue( MO_MIRRORG_LINEG_POSITIONX + i*8 ).Fun()->Eval(state.tempo.ang)*0.78125f + 0.390625;
        linesG[i].positiony = mValueLines.GetSubValue( MO_MIRRORG_LINEG_POSITIONY + i*8 ).Fun()->Eval(state.tempo.ang)*0.5859375 + 0.29296875;
        linesG[i].fixed = mValueLines.GetSubValue( MO_MIRRORG_LINEG_FIXED + i*8 ).Text();
	}


	//carga valroes generales
	generalx = m_Config[moR(MIRRORG_GENERALX)][MO_SELECTED][0].Fun()->Eval(state.tempo.ang);
	generaly = m_Config[moR(MIRRORG_GENERALY)][MO_SELECTED][0].Fun()->Eval(state.tempo.ang);

	for(j=0; j<hpoint2; j++) {
			for(i=0; i<wpoint2; i++) {

				//desplazamiento con funcion de movimiento global
				TPoint2[j * wpoint2 * 2  + i * 2]+= generalx;
				TPoint2[j * wpoint2 * 2  + i * 2 + 1]+= generaly;

				CPoint2[j * wpoint2 * 4  + i * 4]		= 1.0f;
				CPoint2[j * wpoint2 * 4 + 1  + i * 4]	= 1.0f;//*(0.5+sin((float)(i*i)+(float)(j*j))/2.0);
				CPoint2[j * wpoint2 * 4 + 2 + i * 4]	= 1.0f;//*cos((float)(i*i)+(float)(j*j));
				CPoint2[j * wpoint2 * 4 + 3 + i * 4]	= 1.0f;

				//calcula el desplazamiento de los puntos segun los valores dados
				for(l=0;l<NpointsG;l++) {
					pointsG[l].SetDXDY(TPoint2[j * wpoint2 * 2  + i * 2],TPoint2[j * wpoint2 * 2  + i * 2 + 1]);
					TPoint2[j * wpoint2 * 2  + i * 2]+= pointsG[l].dx;
					TPoint2[j * wpoint2 * 2  + i * 2 + 1]+= pointsG[l].dy;
				}

				//calcula el desplazamiento de las lineas segun los valores dados
				for(l=0;l<NlinesG;l++) {
					linesG[l].SetDXDY(TPoint2[j * wpoint2 * 2  + i * 2],TPoint2[j * wpoint2 * 2  + i * 2 + 1]);
					TPoint2[j * wpoint2 * 2  + i * 2]+= linesG[l].dx;
					TPoint2[j * wpoint2 * 2  + i * 2 + 1]+= linesG[l].dy;
				}
			}
	}
}

void moPreEffectMirrorG::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    float w,h;
    float prop;

    w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    h = m_pResourceManager->GetRenderMan()->ScreenHeight();
    if ( w == 0 || h == 0 ) { w  = 1; h = 1; prop = 1.0; }
    else {
      prop = (float) h / (float) w;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    glAlphaFunc(GL_GREATER,0.0f);
    glEnable(GL_ALPHA_TEST);


    glMatrixMode(GL_PROJECTION);    // Select The Projection Matrix
    glLoadIdentity();               // Reset The Projection Matrix
    //glOrtho( -0.5, 0.5, -0.5*prop, 0.5*prop, -1, 1);          // Set Up An Ortho Screen
    gluOrtho2D( -0.5, 0.5, -0.5*prop, 0.5*prop );

	SetColor( m_Config[moR(MIRRORG_COLOR)][MO_SELECTED], m_Config[moR(MIRRORG_ALPHA)][MO_SELECTED], state );

	int i,j,bl;
	float ab;

	ab = m_Config[moR(MIRRORG_ABERRATION)][MO_SELECTED][0].Fun()->Eval(state.tempo.ang);
	bl = m_Config.GetParam(moR(MIRRORG_BLENDING)).GetValue().GetSubValue(0).Int();

    // Aca van los comandos OpenGL del efecto.

	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );


  SetBlending( (moBlendingModes) bl );


	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	GLfloat deltai,deltaj,tdeltai,tdeltaj;
	GLfloat minora,minorb,majora,majorb;

	minora = -0.5;//(w/1024.0f);//0.8 800 pix
	minorb = 0.5;//*(w/1024.0f);
	majora = -0.5*prop;//0.6 600pix
	majorb = 0.5*prop;
	deltai =(minorb-minora) /(float)(wpoint2-1);
	deltaj =(majorb-majora) /(float)(hpoint2-1);
	//tdeltai = (w/1024.0f)/(float)(wpoint2-1);
	//tdeltaj = (h/1024.0f)/(float)(hpoint2-1);
	tdeltai = 1.0 / (float)(wpoint2-1);
	tdeltaj = 1.0 / (float)(hpoint2-1);

	//Valores iniciales por defecto
	//DPoint //coordenadas del punto
	//CPoint //Color del punto
	//TPoint //coordenadas de la textura en el plane.
	for(j=0; j<hpoint2; j++) {
			for(i=0; i<wpoint2; i++) {

				DPoint2[j * wpoint2 * 4  + i * 4]		= ( minora+((float)i)*(deltai) ) * ab;	//X
				DPoint2[j * wpoint2 * 4 + 1  + i * 4]	= ( majora+((float)j)*(deltaj) ) * ab;	//Y
				DPoint2[j * wpoint2 * 4 + 2 + i * 4]	= -0.5185f*ab;						//Z
				DPoint2[j * wpoint2 * 4 + 3 + i * 4]	= 1.0f;							//Normal

				CPoint2[j * wpoint2 * 4  + i * 4]		= 1.0f;
				CPoint2[j * wpoint2 * 4 + 1  + i * 4]	= 1.0f;//*(0.5+sin((float)(i*i)+(float)(j*j))/2.0);
				CPoint2[j * wpoint2 * 4 + 2 + i * 4]	= 1.0f;//*cos((float)(i*i)+(float)(j*j));
				CPoint2[j * wpoint2 * 4 + 3 + i * 4]	= 1.0f;

				TPoint2[j * wpoint2 * 2  + i * 2]		= 0.0f+((float)i)*(tdeltai);
				TPoint2[j * wpoint2 * 2  + i * 2 + 1]	= 0.0f+((float)j)*(tdeltaj);
			}
	}

  glShadeModel(GL_SMOOTH);

  Calculate();

  // Estos factores de correccion se agregaron para tener el cuenta el cambio entre el viejo y el nuevo modo
  // de texturas.


  //float fs = 1024.0f / w;
  //float ft = 1024.0f / h;
  float fs = 1.0;
  float ft = 1.0;


  //solo dibujo los quads ya calculados
  int pmode = m_Config.Int( moR(MIRRORG_POLYGONMODE) );

  for(int mode=0; mode<2; mode++) {

    if (mode==0) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth( 1.0 );
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetRenderMan()->RenderTexGLId(MO_EFFECTS_TEX));
    } else if (mode==1) {
        ///si hay puntos o rayas a agregar
        if (pmode==MO_POLYGONMODE_LINE) {
          ///line version
          glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
          glLineWidth( 0.5 );
          glDisable(GL_TEXTURE_2D);
        } else if (pmode==MO_POLYGONMODE_POINT) {
          ///points version
          glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
          glPointSize( 2.0 );
          glDisable(GL_TEXTURE_2D);
        } else {
            break;
        }
    }

    for(j=0; j<(hpoint2-1); j++) {
        glBegin(GL_QUAD_STRIP);
        for(i=0; i<wpoint2; i++) {
          glTexCoord2f(TPoint2[j * wpoint2 * 2  + i * 2] * fs,TPoint2[j * wpoint2 * 2  + i * 2+1] * ft);
          glVertex2fv(&DPoint2[j * wpoint2 * 4  + i * 4]);
          //glVertex3fv(&DPoint2[j * wpoint2 * 4  + i * 4]);
          glTexCoord2f(TPoint2[(j+1) * wpoint2 * 2  + i * 2] * fs,TPoint2[(j+1) * wpoint2 * 2  + i * 2+1] * ft);
          glVertex2fv(&DPoint2[(j+1) * wpoint2 * 4  + i * 4]);
          //glVertex3fv(&DPoint2[(j+1) * wpoint2 * 4  + i * 4]);
        }
        glEnd();
    }
  }

/*
    glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetRenderMan()->RenderTexGLId(MO_EFFECTS_TEX));


    glBegin(GL_QUADS);
      glTexCoord2f( 0.0, 1.0 );
      glVertex2f( minora, majorb);

      glTexCoord2f( 1.0, 1.0 );
      glVertex2f(  minorb, majorb);

      glTexCoord2f( 1.0, 0.0 );
      glVertex2f(  minorb,  majora);

      glTexCoord2f( 0.0, 0.0 );
      glVertex2f( minora,  majora);
    glEnd();
    */

	//glClear(GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//volvemos al modo por default

}

MOboolean moPreEffectMirrorG::Finish()
{
	if(DPoint2!=NULL) {
		delete [] DPoint2;
		DPoint2 = NULL;
	}
	if(CPoint2!=NULL) {
		delete [] CPoint2;
		CPoint2 = NULL;
	}
	if(TPoint2!=NULL) {
		delete [] TPoint2;
		TPoint2 = NULL;
	}
    return PreFinish();
}

