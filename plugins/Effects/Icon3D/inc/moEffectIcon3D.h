/*******************************************************************************

                                moEffectIcon3D.h

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

#ifndef __MO_EFFECT_ICON3D_H__
#define __MO_EFFECT_ICON3D_H__

#include "moPlugin.h"
#include "mo3dModelManager.h"

#define MO_ICON3D_TRANSLATE_X		0
#define MO_ICON3D_TRANSLATE_Y		1
#define MO_ICON3D_TRANSLATE_Z		2
#define MO_ICON3D_SCALE_X			3
#define MO_ICON3D_SCALE_Y			4
#define MO_ICON3D_SCALE_Z			5

enum moIcon3dParamIndex {
	ICON3D_ALPHA,
	ICON3D_COLOR,
	ICON3D_SYNC,
	ICON3D_PHASE,
	ICON3D_OBJECT,
	ICON3D_TEXTURE,
	ICON3D_AMBIENT,
	ICON3D_SPECULAR,
	ICON3D_DIFFUSE,
	ICON3D_POLYGONMODE,
	ICON3D_BLENDING,
	ICON3D_TRANSLATEX,
	ICON3D_TRANSLATEY,
	ICON3D_TRANSLATEZ,
	ICON3D_ROTATEX,
	ICON3D_ROTATEY,
	ICON3D_ROTATEZ,
	ICON3D_SCALEX,
	ICON3D_SCALEY,
	ICON3D_SCALEZ,
	ICON3D_EYEX,
	ICON3D_EYEY,
	ICON3D_EYEZ,
	ICON3D_VIEWX,
	ICON3D_VIEWY,
	ICON3D_VIEWZ,
	ICON3D_LIGHTX,
	ICON3D_LIGHTY,
	ICON3D_LIGHTZ,
	ICON3D_INLET,
	ICON3D_OUTLET
};

class moEffectIcon3D: public moEffect
{
public:
    moEffectIcon3D();
    virtual ~moEffectIcon3D();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
	void Interaction( moIODeviceManager *IODeviceManager );
	moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );
    MOboolean Finish();
private:

    MOint g_ViewMode;

	MOfloat	Tx,Ty,Tz,Sx,Sy,Sz;

    mo3dModelManagerRef Models3d;
};

class moEffectIcon3DFactory : public moEffectFactory
{
public:
    moEffectIcon3DFactory() {}
    virtual ~moEffectIcon3DFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
