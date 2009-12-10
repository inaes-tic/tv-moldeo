/*******************************************************************************

                        moPreEffectMirrorMorphing.h

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

#ifndef __MO_PRE_EFFECT_MIRRORMORPHING_H__
#define __MO_PRE_EFFECT_MIRRORMORPHING_H__

#include "moPrePlugin.h"

#ifndef MO_RED
#define MO_RED      0
#define MO_GREEN    1
#define MO_BLUE     2
#define MO_ALPHA    3
#endif

typedef struct moMirrorPoint
{
    MOint  I,J;
    MOfloat P0X,P0Y,P0Z;
    MOfloat P1X,P1Y,P1Z;
    MOfloat PAUXX,PAUXY,PAUXZ;
    MOfloat D0X,D0Y,D0Z;
    MOfloat D1X,D1Y,D1Z;	
    MOfloat Z0X,Z0Y,Z0Z;
    MOfloat Z1X,Z1Y,Z1Z;	
    MOuint NSteps;
    MOuint Steps;
    MOswitch Animate;
    MOfloat INCDX,INCDY,INCDZ;	
    MOfloat INCZX,INCZY,INCZZ;	
    MOfloat DISTANCE,DISTANCEC;
    MOfloat NX,NY,NZ;
} TmoMirrorPoint;

class moPreEffectMirrorMorphing : public moPreEffect
{
public:
    MOint color;
    MOint transx, transy, transz;
    MOint rotatx, rotaty, rotatz;
    MOint scalex, scaley, scalez;
    MOint radius, aberration;
    MOint configuration, blending;

    MOint cf, cfprevious;
    MOswitch Animate;
    MOuint Steps, NSteps;
    MOfloat dist;
	MOint MO_MESHA_WIDTH;
    MOint MO_MESHA_HEIGHT;
    MOint MO_MESHA_TEX;
    TmoMirrorPoint **MESHAMORPH;

    moPreEffectMirrorMorphing();
    virtual ~moPreEffectMirrorMorphing();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* = NULL);
    MOboolean Finish();
    void LoadConfiguration();
};

class moPreEffectMirrorMorphingFactory : public moPreEffectFactory
{
public:
    moPreEffectMirrorMorphingFactory() {}
    virtual ~moPreEffectMirrorMorphingFactory() {}
    moPreEffect* Create();
    void Destroy(moPreEffect* fx);
};

extern "C"
{
MO_PLG_API moPreEffectFactory* CreatePreEffectFactory();
MO_PLG_API void DestroyPreEffectFactory();
}

#endif
