/*******************************************************************************

                                moEffectCubeMap.h

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

#ifndef __MO_EFFECT_CUBEMAP_H__
#define __MO_EFFECT_CUBEMAP_H__

#include "moPlugin.h"

enum moCubemapParamIndex {
	CUBEMAP_ALPHA,
	CUBEMAP_COLOR,
	CUBEMAP_SYNC,
	CUBEMAP_PHASE,
	CUBEMAP_TEXTURELEFT,
	CUBEMAP_TEXTUREFRONT,
	CUBEMAP_TEXTURERIGHT,
	CUBEMAP_TEXTUREBACK,
	CUBEMAP_TEXTURETOP,
	CUBEMAP_TEXTUREBOTTOM,
	CUBEMAP_TRANSLATEX,
	CUBEMAP_TRANSLATEY,
	CUBEMAP_TRANSLATEZ,
	CUBEMAP_ROTATEX,
	CUBEMAP_ROTATEY,
	CUBEMAP_ROTATEZ,
	CUBEMAP_SCALEX,
	CUBEMAP_SCALEY,
	CUBEMAP_SCALEZ,
	CUBEMAP_INLET,
	CUBEMAP_OUTLET
};

class moEffectCubeMap : public moEffect
{
public:

    moEffectCubeMap();
    virtual ~moEffectCubeMap();

    MOboolean Init();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

};

class moEffectCubeMapFactory : public moEffectFactory
{
public:
    moEffectCubeMapFactory() {}
    virtual ~moEffectCubeMapFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
