/*******************************************************************************

                            moPreEffectMirrorG.h

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

/* Este efecto reemplazara a los dos efectos MirrorG y Mirror, en un futuro*/

#ifndef __MO_PRE_EFFECT_MIRRORG_H__
#define __MO_PRE_EFFECT_MIRRORG_H__

#include "moPrePlugin.h"


#define MO_MIRRORG_POINTG_AMPLITUDE 0
#define MO_MIRRORG_POINTG_ATENUATION_FUNCTION 1
#define MO_MIRRORG_POINTG_ATENUATION_AMPLITUDE 2
#define MO_MIRRORG_POINTG_X 3
#define MO_MIRRORG_POINTG_Y 4
#define MO_MIRRORG_POINTG_FIXED 5

#define MO_MIRRORG_LINEG_AMPLITUDE 0
#define MO_MIRRORG_LINEG_ATENUATION_FUNCTION 1
#define MO_MIRRORG_LINEG_ATENUATION_AMPLITUDE 2
#define MO_MIRRORG_LINEG_DIRECTIONX 3
#define MO_MIRRORG_LINEG_DIRECTIONY 4
#define MO_MIRRORG_LINEG_POSITIONX 5
#define MO_MIRRORG_LINEG_POSITIONY 6
#define MO_MIRRORG_LINEG_FIXED 7

#define VORDER 10
#define CORDER 10
#define TORDER 3

#define VMMOOR_ORDER 30
#define VMINOR_ORDER 30

#define CMMOOR_ORDER 3
#define CMINOR_ORDER 3

#define TMMOOR_ORDER 3
#define TMINOR_ORDER 3
#define VDIM 4
#define CDIM 4
#define TDIM 2

#define ONE_D 1
#define TWO_D 2

#define EVAL 3

#define MESH 4

#include "port/strconst.h"

enum moMirrorgIndex {
    MIRRORG_ALPHA,
	MIRRORG_SYNCRO,
	MIRRORG_PHASE,
	MIRRORG_COLOR,
	MIRRORG_ABERRATION,
	MIRRORG_BLENDING,
	MIRRORG_GENERALX,
	MIRRORG_GENERALY,
	MIRRORG_DOTS,
	MIRRORG_LINES
};

class moPointG
{
public:
    MOfloat x,y;
    MOfloat amplitude;
    MOfloat amplitudeatenuation;
    moText atenuationfunction; //L: lineal o E: exponencial I: inverso a la distancia O: logaritmo u otra
    moText fixed;

    MOint atenuationfunctioni;
    //devuelve
    MOfloat dx,dy;

    MOfloat SetDXDY(MOfloat _x, MOfloat _y);
};

class moLineG
{
public:
    MOfloat directionx, directiony;
    MOfloat positionx, positiony;
    MOfloat amplitude;
    MOfloat amplitudeatenuation;
    moText atenuationfunction; //L: lineal o E: exponencial I: inverso a la distancia O: logaritmo u otra
    moText fixed;
    MOint atenuationfunctioni;

    MOfloat dx,dy;
    MOfloat SetDXDY(MOfloat _x, MOfloat _y);
    void Normalizar();
};

class moCircleG
{
public:
    MOfloat x,y;
};

class moSpiralG
{
public:
    MOfloat x,y;
};

class moWaveG
{
public:
    MOfloat x,y;
};

class moPreEffectMirrorG : public moPreEffect
{
public:
    MOint color;
    MOint transx, transy, transz;
    MOint rotatx, rotaty, rotatz;
    MOint scalex, scaley, scalez;

    MOint FpointsG, FlinesG, FwavesG, FspiralsG, FcirclesG;
    MOint MpointsTx, MpointsTy; //movimiento de los puntos, vector que se suma
    MOint MlinesTx, MlinesTy, MlineasVx, MlineasVy; //T traslacion V direccion
    MOint McirclesTx, McirclesTy, Mradius; //centro y radius
    MOint MwavesVx, MwavesVy; //direccion de la linea
    MOint MspiralsTx, MspiralsTy; //centro

    MOint GeneralMx,GeneralMy;

    //para calculo interno
    MOdouble generalx, generaly;
    moPointG pointsG[20];
    moLineG linesG[20];
	MOint NpointsG, NlinesG;

	//unused:
	moWaveG wavesG[20];
    moCircleG circlesG[20];
    moSpiralG spiralsG[20];
    MOint NcirclesG, NspiralsG, NwavesG;

    MOint aberration;
    MOint configuration, blending;
//GLdouble Point2[3*3*4], cPoint2[3*3*4], tPoint2[3*3*2];
//GLdouble* Point2ptr =(GLdouble*)&Point2[0];
//GLfloat DPoint2[VMMOOR_ORDER][VMINOR_ORDER][4];
    GLfloat *DPoint2;//array de coordenadas de cada vertice
    GLfloat *CPoint2;//array de colores de cada vertice
    GLfloat *TPoint2;//array de coordenadas de texturas de cada vertice

    MOint npoint2, hpoint2, wpoint2;

    moPreEffectMirrorG();
    virtual ~moPreEffectMirrorG();

    void Calculate();
    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    void DrawPoints2();
};

class moPreEffectMirrorGFactory : public moPreEffectFactory
{
public:
    moPreEffectMirrorGFactory() {}
    virtual ~moPreEffectMirrorGFactory() {}
    moPreEffect* Create();
    void Destroy(moPreEffect* fx);
};

extern "C"
{
MO_PLG_API moPreEffectFactory* CreatePreEffectFactory();
MO_PLG_API void DestroyPreEffectFactory();
}

#endif
