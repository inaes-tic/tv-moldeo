/*******************************************************************************

                                moEffectSampleTest3d.h

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

#ifndef __MO_EFFECT_SAMPLETEST3D_H__
#define __MO_EFFECT_SAMPLETEST3D_H__

#include "moPlugin.h"

enum moIconParamIndex {
	SAMPLETEST3D_ALPHA,
	SAMPLETEST3D_COLOR,
	SAMPLETEST3D_SYNC,
	SAMPLETEST3D_PHASE,
	SAMPLETEST3D_TEXTURE,
	SAMPLETEST3D_WIDTH,
	SAMPLETEST3D_HEIGHT,
	SAMPLETEST3D_TRANSLATEX,
	SAMPLETEST3D_TRANSLATEY,
	SAMPLETEST3D_TRANSLATEZ,
	SAMPLETEST3D_ROTATEX,
	SAMPLETEST3D_ROTATEY,
	SAMPLETEST3D_ROTATEZ,
	SAMPLETEST3D_SCALEX,
	SAMPLETEST3D_SCALEY,
	SAMPLETEST3D_SCALEZ,
	SAMPLETEST3D_INLET,
	SAMPLETEST3D_OUTLET
};

class moEffectSampleTest3d: public moEffect
{
public:
    moEffectSampleTest3d();
    virtual ~moEffectSampleTest3d();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

private:
    MOint height_i, width_i;
    MOint vert_i, horiz_i;

};

class moEffectSampleTest3dFactory : public moEffectFactory {
public:
    moEffectSampleTest3dFactory() {}
    virtual ~moEffectSampleTest3dFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
