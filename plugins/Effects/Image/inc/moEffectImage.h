/*******************************************************************************

                                moEffectImage.h

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



#ifndef __MO_EFFECT_IMAGE_H__
#define __MO_EFFECT_IMAGE_H__

#include "moPlugin.h"

#define MOParam(__PARAMINDEX)	m_Config[moR(__PARAMINDEX)]
#define MOValue(__PARAMINDEX,__VALUEINDEX) m_Config[moR(__PARAMINDEX)][__VALUEINDEX]
#define MOSubValue(__PARAMINDEX,__VALUEINDEX,__SUBVALUEINDEX) m_Config[moR(__PARAMINDEX)][__VALUEINDEX][__SUBVALUEINDEX]

enum moImageParamIndex {
	IMAGE_ALPHA,
	IMAGE_COLOR,
	IMAGE_SYNC,
	IMAGE_PHASE,
	IMAGE_TEXTURE,
	/*IMAGE_FILTER,*/
	IMAGE_BLENDING,
	IMAGE_POSTEXX,
	IMAGE_POSTEXY,
	IMAGE_ANCTEXX,
	IMAGE_ALTTEXY,
	IMAGE_POSCUADX,
	IMAGE_POSCUADY,
	IMAGE_ANCCUADX,
	IMAGE_ALTCUADY,
	IMAGE_INLET,
	IMAGE_OUTLET
};

#define MO_FILTER_PAR1		0

class moEffectImage : public moEffect
{
public:
	moTextFilterParam FilterParams;
	MOboolean using_filter;

    moEffectImage();
    virtual ~moEffectImage();

    MOboolean Init();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
	void Interaction( moIODeviceManager *IODeviceManager );
	moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );
    MOboolean Finish();

	//void Update( moEventList *p_EventList);

/*
    //TRACKER
	MOboolean				m_bTrackerInit;
	moTrackerKLTSystemData*	m_pTrackerData;
	moTrackerGpuKLTSystemData*	m_pTrackerGpuData;
	MOint NF, TrackerX, TrackerY, Variance;
	void DrawTrackerFeatures();

	//Script
	void RegisterFunctions();
	int GetFeature(moLuaVirtualMachine& vm);
	int DrawLine(moLuaVirtualMachine& vm);
	int PushDebugString(moLuaVirtualMachine& vm);
	int fcount;
	int m_iMethodBase;
*/
};

class moEffectImageFactory : public moEffectFactory
{
public:
    moEffectImageFactory() {}
    virtual ~moEffectImageFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
