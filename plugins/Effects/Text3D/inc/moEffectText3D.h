/*******************************************************************************

                              moEffectText3D.h

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

#ifndef __MO_EFFECT_TEXT3D_H__
#define __MO_EFFECT_TEXT3D_H__

#include "moPlugin.h"

#define MO_TEXT3D_TRANSLATE_X 0
#define MO_TEXT3D_TRANSLATE_Y 1
#define MO_TEXT3D_SCALE_X 2
#define MO_TEXT3D_SCALE_Y 3

enum moTextParamIndex {
	TEXT3D_ALPHA,
	TEXT3D_COLOR,
	TEXT3D_SYNC,
	TEXT3D_PHASE,
	TEXT3D_FONT,
	TEXT3D_TEXT,
	TEXT3D_BLENDING,
	TEXT3D_WIDTH,
	TEXT3D_HEIGHT,
	TEXT3D_TRANSLATEX,
	TEXT3D_TRANSLATEY,
	TEXT3D_TRANSLATEZ,
    TEXT3D_SCALEX,
	TEXT3D_SCALEY,
	TEXT3D_SCALEZ,
	TEXT3D_ROTATEX,
	TEXT3D_ROTATEY,
	TEXT3D_ROTATEZ,
	TEXT3D_EYEX,
	TEXT3D_EYEY,
	TEXT3D_EYEZ,
	TEXT3D_VIEWX,
	TEXT3D_VIEWY,
	TEXT3D_VIEWZ,
	TEXT3D_LIGHTX,
	TEXT3D_LIGHTY,
	TEXT3D_LIGHTZ,
    TEXT3D_INLET,
	TEXT3D_OUTLET
};


class moEffectText3D : public moEffect
{
    public:


        moEffectText3D();
        virtual ~moEffectText3D();

        MOboolean Init();
        void Draw(moTempo*, moEffectState* parentstate = NULL);
        MOboolean Finish();

        void Interaction( moIODeviceManager * );
        moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    private:

        void setUpLighting();
};

class moEffectText3DFactory : public moEffectFactory
{
public:
    moEffectText3DFactory() {}
    virtual ~moEffectText3DFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
