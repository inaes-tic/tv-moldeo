/*******************************************************************************

                             moEffectLightPercussion.h

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

#ifndef __MO_EFFECT_LIGHTPERCUSSION__
#define __MO_EFFECT_LIGHTPERCUSSION__

#include "moPlugin.h"

#ifndef MO_RED
#define MO_RED      0
#define MO_GREEN    1
#define MO_BLUE     2
#define MO_ALPHA   3
#endif

class moLightPercussionPoint : public moAbstract
{
public:
    MOint  I,J;
    MOfloat P0X,P0Y,P0Z;
    MOuint NSteps;
    MOuint Steps;
    MOswitch Animate;
    MOswitch Pressed;
    MOfloat Energy;//la actual

    MOfloat E_stable;
    MOfloat E_excited;
    MOfloat E_explosion;
    MOfloat E_depressed;
    MOint Decay_time;//decae 1 cada tantos milisegundos
    MOint Time;

    moLightPercussionPoint *neighbors[3][3];

    moLightPercussionPoint();
    ~moLightPercussionPoint();

    void EnergyPulse();//cuando se pulsa, adquiere gran cantidad de energia
    void Increase(MOfloat);//incrementa su energia de un valor arbitrario
    void Decay();//el punto decae todo el tiempo, y transfiere a sus congeneres
    void Irradiate(float);
};

class moEffectLightPercussion : public moEffect
{
public:
    //config
    MOint color;
    MOint images;

    MOint divisorf, factorx, factory, factorz;

    MOint transx, transy, transz;
    MOint rotatx, rotaty, rotatz;
    MOint scalex, scaley, scalez;

    MOint configuration;

    //images
    moTextureIndex Images;

    MOint Mwidth, Mheight;
    moLightPercussionPoint **MESHALIGHTS;

    MOint cf, cfprevious, NSteps, NS;

    moEffectLightPercussion();
    virtual ~moEffectLightPercussion();

    MOboolean Init();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void Interaction(moIODeviceManager *);
};

class MO_PLG_API moEffectLightPercussionFactory : public moEffectFactory 
{
public:
    moEffectLightPercussionFactory() {}
    ~moEffectLightPercussionFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
