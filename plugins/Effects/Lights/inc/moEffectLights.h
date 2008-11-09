/*******************************************************************************

                                moEffectLights.h

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

#ifndef __MO_EFFECT_LIGHTS__
#define __MO_EFFECT_LIGHTS__

#include "moPlugin.h"

#ifndef MO_RED
#define MO_RED      0
#define MO_GREEN    1
#define MO_BLUE     2
#define MO_ALPHA    3
#endif


enum moLightsParamIndex {
	LIGHTS_ALPHA,
	LIGHTS_COLOR,
	LIGHTS_SYNC,
	LIGHTS_PHASE,
	LIGHTS_BLENDING,
	LIGHTS_NUMBER,
	LIGHTS_TEXTURE,
	LIGHTS_FACTORX,
	LIGHTS_FACTORY,
	LIGHTS_FACTORZ,
	LIGHTS_DIVISOR,
	LIGHTS_TRANSLATEX,
	LIGHTS_TRANSLATEY,
    LIGHTS_SCALEX,
	LIGHTS_SCALEY,
	LIGHTS_ROTATEZ,
    LIGHTS_INLET,
	LIGHTS_OUTLET
};

class moEffectLights : public moEffect
{

    public:

        moEffectLights();
        virtual ~moEffectLights();

        MOboolean Init();
        void Draw(moTempo*, moEffectState* parentstate = NULL);
        MOboolean Finish();

        moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

    protected:

        moCoords Lights;

        MOfloat radius;

        MOint cf, cfprevious, NSteps, NS;

};

class moEffectLightsFactory : public moEffectFactory
{
public:
    moEffectLightsFactory() {}
    virtual ~moEffectLightsFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
