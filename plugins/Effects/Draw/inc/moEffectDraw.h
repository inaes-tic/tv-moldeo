/*******************************************************************************

                                moEffectDraw.h

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

#ifndef __MO_EFFECT_DRAW_H__
#define __MO_EFFECT_DRAW_H__

#include "moPlugin.h"
#include "moLuaBase.h"
#include "moP5.h"

#define MO_DRAW_TRANSLATE_X 0
#define MO_DRAW_TRANSLATE_Y 1
#define MO_DRAW_SCALE_X 2
#define MO_DRAW_SCALE_Y 3

enum moDrawParamIndex {
	DRAW_ALPHA,
	DRAW_COLOR,
	DRAW_SYNC,
	DRAW_PHASE,
	DRAW_TEXTURE,
	DRAW_SCRIPT,
	DRAW_BLENDING,
	DRAW_WIDTH,
	DRAW_HEIGHT,
	DRAW_TRANSLATEX,
	DRAW_TRANSLATEY,
	DRAW_TRANSLATEZ,
	DRAW_ROTATEX,
	DRAW_ROTATEY,
	DRAW_ROTATEZ,
	DRAW_SCALEX,
	DRAW_SCALEY,
	DRAW_SCALEZ,
	DRAW_INLET,
	DRAW_OUTLET
};

class moEffectDraw: public moEffect {

public:
    moEffectDraw();
    virtual ~moEffectDraw();

    MOboolean Init();
    void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
    MOboolean Finish();
	void Interaction( moIODeviceManager * );
	moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    // Public script functions.
	int ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber);
	void HandleReturns(moLuaVirtualMachine& vm, const char *strFunc);


private:

    moP5    MOP5;
   	long                    m_Ticks;
	long					m_TicksAux;


	MOint Tx, Ty, Tz;
	MOfloat Sx, Sy, Sz;
	MOint Rx, Ry, Rz;

	moTextHeap	m_tScriptDebug;
    // CUSTOM 1
	moText m_MovieScriptFN;
    //TRACKER
	MOboolean				m_bTrackerInit;
	/*
	moTrackerKLTSystemData*	m_pTrackerData;
	moTrackerGpuKLTSystemData*	m_pTrackerGpuData;
    */
	moTrackerSystemData*	m_pTrackerData;

	// For indexing scripted methods.
	int m_iMethodBase;

    // Script functions.
	void RegisterFunctions();
	int GetFeature(moLuaVirtualMachine& vm);
    int triangle(moLuaVirtualMachine& vm);
    int line(moLuaVirtualMachine& vm);
	int arc(moLuaVirtualMachine& vm);
	int point(moLuaVirtualMachine& vm);
	int quad(moLuaVirtualMachine& vm);
	int ellipse(moLuaVirtualMachine& vm);
	int rect(moLuaVirtualMachine& vm);
	int strokeWeight(moLuaVirtualMachine& vm);
	int background(moLuaVirtualMachine& vm);
	int colorMode(moLuaVirtualMachine& vm);
	int stroke(moLuaVirtualMachine& vm);
	int nofill(moLuaVirtualMachine& vm);
	int nostroke(moLuaVirtualMachine& vm);
	int fill(moLuaVirtualMachine& vm);
	int pushMatrix(moLuaVirtualMachine& vm);
	int popMatrix(moLuaVirtualMachine& vm);
	int resetMatrix(moLuaVirtualMachine& vm);
	int scale(moLuaVirtualMachine& vm);
	int translate(moLuaVirtualMachine& vm);
	int rotate(moLuaVirtualMachine& vm);

	int GetTicks(moLuaVirtualMachine& vm);
	int PushDebugString(moLuaVirtualMachine& vm);

};

class moEffectDrawFactory : public moEffectFactory {

public:
    moEffectDrawFactory() {}
    virtual ~moEffectDrawFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
