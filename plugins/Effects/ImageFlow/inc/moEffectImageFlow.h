/*******************************************************************************

                             moEffectImageFlow.h

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                     *
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

  Copyright(C) 2007 Andrés Colubri

  Authors:
  Andrés Colubri
  Fabricio Costa

*******************************************************************************/

#include "moPlugin.h"
#include "moTextureIndex.h"
#include "moMathFunction.h"

#ifndef __MO_EFFECT_IMAGE_FLOW_H__
#define __MO_EFFECT_IMAGE_FLOW_H__

enum moImageFlowParamIndex {
    IMAGEFLOW_ALPHA,
	IMAGEFLOW_COLOR,
	IMAGEFLOW_BLENDING,
	IMAGEFLOW_IMAGES,
	IMAGEFLOW_VELOCITY,
	IMAGEFLOW_TEXTURE
};

class moEffectImageFlow : public moEffect
{
public:
    moEffectImageFlow();
    virtual ~moEffectImageFlow();

    MOboolean Init();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
    void Interaction( moIODeviceManager * );
    MOboolean Finish();
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

protected:
	float color[4];
	int blending;
	int flow_mode;
    moTextureIndex Images;

	int idx0, idx1;
	int tex0, tex1;
    float flow_coord;
	float flow_velocity0, flow_velocity, flow_velocity_bak;
	float minx, maxx, miny, maxy;

	void setBlendMode();
	void updateParameters();
};

class moEffectImageFlowFactory : public moEffectFactory
{
public:
    moEffectImageFlowFactory() {}
    virtual ~moEffectImageFlowFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
