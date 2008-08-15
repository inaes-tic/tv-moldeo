/*******************************************************************************

                      moMasterEffectPresetPanel.h

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

#include "moMasterPlugin.h"

#ifndef __MO_MASTER_EFFECT_PRESETPANEL_H__
#define __MO_MASTER_EFFECT_PRESETPANEL_H__

#define MO_ACTION_PRESET_0_SAVE 0
#define MO_ACTION_PRESET_1_SAVE 1
#define MO_ACTION_PRESET_2_SAVE 2
#define MO_ACTION_PRESET_3_SAVE 3
#define MO_ACTION_PRESET_4_SAVE 4
#define MO_ACTION_PRESET_5_SAVE 5
#define MO_ACTION_PRESET_6_SAVE 6
#define MO_ACTION_PRESET_7_SAVE 7
#define MO_ACTION_PRESET_8_SAVE 8
#define MO_ACTION_PRESET_0_LOAD 9
#define MO_ACTION_PRESET_1_LOAD 10
#define MO_ACTION_PRESET_2_LOAD 11
#define MO_ACTION_PRESET_3_LOAD 12
#define MO_ACTION_PRESET_4_LOAD 13
#define MO_ACTION_PRESET_5_LOAD 14
#define MO_ACTION_PRESET_6_LOAD 15
#define MO_ACTION_PRESET_7_LOAD 16
#define MO_ACTION_PRESET_8_LOAD 17
#define MO_ACTIONS_PRESET 18

class moMasterEffectPresetPanel : public moMasterEffect
{
public:
    moConsoleState states[MO_MAX_PRESETS];
    moPresets **presets;

    moMasterEffectPresetPanel();
    virtual ~moMasterEffectPresetPanel();
    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void SavePreset(MOint);
    void LoadPreset(MOint);
    void Interaction(moIODeviceManager *);
};

class moMasterEffectPresetPanelFactory : public moMasterEffectFactory
{
public:
    moMasterEffectPresetPanelFactory() {}
    virtual ~moMasterEffectPresetPanelFactory() {}
    moMasterEffect* Create();
    void Destroy(moMasterEffect* fx);
};

extern "C"
{
MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory();
MO_PLG_API void DestroyMasterEffectFactory();
}

#endif
