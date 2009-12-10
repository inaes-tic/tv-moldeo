/*******************************************************************************

                                moEffectIcon.h

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

#ifndef __MO_EFFECT_ICON_H__
#define __MO_EFFECT_ICON_H__

#include "moPlugin.h"

#define MO_ICON_TRANSLATE_X 0
#define MO_ICON_TRANSLATE_Y 1
#define MO_ICON_SCALE_X 2
#define MO_ICON_SCALE_Y 3

enum moIconParamIndex {
	ICON_ALPHA,
	ICON_COLOR,
	ICON_SYNC,
	ICON_PHASE,
	ICON_TEXTURE,
	ICON_BLENDING,
	ICON_WIDTH,
	ICON_HEIGHT,
	ICON_TRANSLATEX,
	ICON_TRANSLATEY,
	ICON_TRANSLATEZ,
	ICON_ROTATEX,
	ICON_ROTATEY,
	ICON_ROTATEZ,
	ICON_SCALEX,
	ICON_SCALEY,
	ICON_SCALEZ,
	ICON_INLET,
	ICON_OUTLET
};

class moEffectIcon: public moEffect {

public:
    moEffectIcon();
    virtual ~moEffectIcon();

    MOboolean Init();
    void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
    MOboolean Finish();
	void Interaction( moIODeviceManager * );
	moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );
    void Update( moEventList *Events );

private:

	MOint Tx, Ty, Tz;
	MOfloat Sx, Sy, Sz;
	MOint Rx, Ry, Rz;

};

class moEffectIconFactory : public moEffectFactory {

public:
    moEffectIconFactory() {}
    virtual ~moEffectIconFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
