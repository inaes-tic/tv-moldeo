/*******************************************************************************

                                moEffectPlane.h

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

#ifndef __MO_EFFECT_PLANE_H__
#define __MO_EFFECT_PLANE_H__

#include "moPlugin.h"

enum moIconParamIndex {
	PLANE_ALPHA,
	PLANE_COLOR,
	PLANE_SYNC,
	PLANE_PHASE,
	PLANE_TEXTURE,
	PLANE_BLENDING,
	PLANE_WIDTH,
	PLANE_HEIGHT,
	PLANE_TRANSLATEX,
	PLANE_TRANSLATEY,
	PLANE_TRANSLATEZ,
	PLANE_ROTATEX,
	PLANE_ROTATEY,
	PLANE_ROTATEZ,
	PLANE_SCALEX,
	PLANE_SCALEY,
	PLANE_SCALEZ,
	PLANE_INLET,
	PLANE_OUTLET
};

class moEffectPlane: public moEffect
{
public:
    moEffectPlane();
    virtual ~moEffectPlane();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

private:
    MOint height_i, width_i;
    MOint vert_i, horiz_i;

};

class moEffectPlaneFactory : public moEffectFactory {
public:
    moEffectPlaneFactory() {}
    virtual ~moEffectPlaneFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
