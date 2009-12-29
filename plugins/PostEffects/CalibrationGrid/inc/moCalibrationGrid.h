/*******************************************************************************

                                moCalibrationGrid.h

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
  Gustavo Orrillo

*******************************************************************************/

#ifndef __MO_EFFECT_CALIBRATIONGRID_H__
#define __MO_EFFECT_CALIBRATIONGRID_H__

#include "moPostPlugin.h"

#define MO_CALIBRATIONGRID_TRANSLATE_X 0
#define MO_CALIBRATIONGRID_TRANSLATE_Y 1
#define MO_CALIBRATIONGRID_SCALE_X 2
#define MO_CALIBRATIONGRID_SCALE_Y 3

enum moCalibrationGridParamIndex {
	CALIBRATIONGRID_ALPHA,
	CALIBRATIONGRID_COLOR,
	CALIBRATIONGRID_SYNC,
	CALIBRATIONGRID_PHASE,
	CALIBRATIONGRID_TEXTURE,
	CALIBRATIONGRID_TRANSLATEX,
	CALIBRATIONGRID_TRANSLATEY,
	CALIBRATIONGRID_TRANSLATEZ,
	CALIBRATIONGRID_ROTATEX,
	CALIBRATIONGRID_ROTATEY,
	CALIBRATIONGRID_ROTATEZ,
	CALIBRATIONGRID_SCALEX,
	CALIBRATIONGRID_SCALEY,
	CALIBRATIONGRID_SCALEZ,
	CALIBRATIONGRID_SHOWGRID
};

class moCalibrationGrid: public moPostEffect {

public:
    moCalibrationGrid();
    virtual ~moCalibrationGrid();

    MOboolean Init();
    void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
    MOboolean Finish();
	moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

private:

	MOint Tx, Ty, Tz;
	MOfloat Sx, Sy, Sz;
	MOint Rx, Ry, Rz;

};

class moCalibrationGridFactory : public moPostEffectFactory {

public:
    moCalibrationGridFactory() {};
    virtual ~moCalibrationGridFactory() {};
    moPostEffect* Create();
    void Destroy(moPostEffect* fx);
};

extern "C"
{
MO_PLG_API moPostEffectFactory* CreatePostEffectFactory();
MO_PLG_API void DestroyPostEffectFactory();
}

#endif
