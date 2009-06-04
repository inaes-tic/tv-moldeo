/*******************************************************************************

                                moEffectSampleTest.h

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

#ifndef __MO_EFFECT_SAMPLETEST_H__
#define __MO_EFFECT_SAMPLETEST_H__

#include "moPlugin.h"

#define MO_SAMPLETEST_TRANSLATE_X 0
#define MO_SAMPLETEST_TRANSLATE_Y 1
#define MO_SAMPLETEST_SCALE_X 2
#define MO_SAMPLETEST_SCALE_Y 3

enum moSampleTestParamIndex {
	SAMPLETEST_ALPHA,
	SAMPLETEST_COLOR,
	SAMPLETEST_SYNC,
	SAMPLETEST_PHASE,
	SAMPLETEST_TEXTURE,
	SAMPLETEST_BLENDING,
	SAMPLETEST_WIDTH,
	SAMPLETEST_HEIGHT,
	SAMPLETEST_TRANSLATEX,
	SAMPLETEST_TRANSLATEY,
	SAMPLETEST_TRANSLATEZ,
	SAMPLETEST_ROTATEX,
	SAMPLETEST_ROTATEY,
	SAMPLETEST_ROTATEZ,
	SAMPLETEST_SCALEX,
	SAMPLETEST_SCALEY,
	SAMPLETEST_SCALEZ,
	SAMPLETEST_INLET,
	SAMPLETEST_OUTLET
};

class moEffectSampleTest: public moEffect {

public:
    moEffectSampleTest();
    virtual ~moEffectSampleTest();

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

class moEffectSampleTestFactory : public moEffectFactory {

public:
    moEffectSampleTestFactory() {}
    virtual ~moEffectSampleTestFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
