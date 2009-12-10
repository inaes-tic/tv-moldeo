/*******************************************************************************

                                moPostEffectPostProcess2d.h

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

#ifndef __MO_EFFECT_PP2D_H__
#define __MO_EFFECT_PP2D_H__

#include "moPostPlugin.h"

#define MO_PP2D_TRANSLATE_X 0
#define MO_PP2D_TRANSLATE_Y 1
#define MO_PP2D_SCALE_X 2
#define MO_PP2D_SCALE_Y 3

enum moPostProcess2dParamIndex {
	PP2D_ALPHA,
	PP2D_COLOR,
	PP2D_SYNC,
	PP2D_PHASE,
	PP2D_TEXTURE,
	PP2D_BLENDING,
	PP2D_WIDTH,
	PP2D_HEIGHT,
	PP2D_TRANSLATEX,
	PP2D_TRANSLATEY,
	PP2D_TRANSLATEZ,
	PP2D_ROTATEX,
	PP2D_ROTATEY,
	PP2D_ROTATEZ,
	PP2D_SCALEX,
	PP2D_SCALEY,
	PP2D_SCALEZ,
	PP2D_INLET,
	PP2D_OUTLET
};

class moPostEffectPostProcess2d: public moPostEffect {

public:
    moPostEffectPostProcess2d();
    virtual ~moPostEffectPostProcess2d();

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

class moPostEffectPostProcess2dFactory : public moPostEffectFactory {

public:
    moPostEffectPostProcess2dFactory() {}
    virtual ~moPostEffectPostProcess2dFactory() {}
    moPostEffect* Create();
    void Destroy(moPostEffect* fx);
};

extern "C"
{
MO_PLG_API moPostEffectFactory* CreatePostEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
