/*******************************************************************************

                     moPostEffectVideoWallMorphing.h

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

#ifndef __MO_POST_EFFECT_VIDEOWALL_MORPHING_H__
#define __MO_POST_EFFECT_VIDEOWALL_MORPHING_H__

#include "moPostPlugin.h"

enum moVideoWallMorphingParamIndex {
	VIDEOWALLMORPHING_ALPHA,
	VIDEOWALLMORPHING_COLOR,
	VIDEOWALLMORPHING_CONFIGURATION,
	VIDEOWALLMORPHING_BLENDING,
	VIDEOWALLMORPHING_RADIUS,
	VIDEOWALLMORPHING_ABERRATION,
	VIDEOWALLMORPHING_TRANSLATEX,
	VIDEOWALLMORPHING_TRANSLATEY,
	VIDEOWALLMORPHING_TRANSLATEZ,
	VIDEOWALLMORPHING_SCALEX,
	VIDEOWALLMORPHING_SCALEY,
	VIDEOWALLMORPHING_SCALEZ,
	VIDEOWALLMORPHING_ROTATEX,
	VIDEOWALLMORPHING_ROTATEY,
	VIDEOWALLMORPHING_ROTATEZ
};

#ifndef MO_RED
#define MO_RED      0
#define MO_GREEN    1
#define MO_BLUE     2
#define MO_ALPHA    3
#endif

typedef struct moVideowallPoint
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
    MOfloat INCPX,INCPY,INCPZ;
    MOfloat INCDX,INCDY,INCDZ;
    MOfloat INCZX,INCZY,INCZZ;
    MOfloat DISTANCE,DISTANCEC;
    MOfloat NX,NY,NZ;
} TmoVideowallPoint;

class moPostEffectVideoWallMorphing : public moPostEffect
{
public:
    MOint cf, cfprevious;
    MOswitch Animate;
    MOuint Steps, NSteps;
    MOfloat dist;
	TmoVideowallPoint **MESHAMORPH;
    MOint MO_MESHA_WIDTH4;
    MOint MO_MESHA_HEIGHT4;
    MOint MO_MESHA_TEX2;

	moRenderManager*	MORenderMan;

    moPostEffectVideoWallMorphing();
    virtual ~moPostEffectVideoWallMorphing();

    MOboolean Init();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void LoadConfiguration();
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );
};

class moPostEffectVideoWallMorphingFactory : public moPostEffectFactory
{
public:
    moPostEffectVideoWallMorphingFactory() {}
    virtual ~moPostEffectVideoWallMorphingFactory() {}
    moPostEffect* Create();
    void Destroy(moPostEffect* fx);
};

extern "C"
{
MO_PLG_API moPostEffectFactory* CreatePostEffectFactory();
MO_PLG_API void DestroyPostEffectFactory();
}

#endif
