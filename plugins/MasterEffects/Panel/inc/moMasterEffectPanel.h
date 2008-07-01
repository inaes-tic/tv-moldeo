/*******************************************************************************

                        moMasterEffectPanel.h

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

#ifndef __MO_MASTER_EFFECT_PANEL_H__
#define __MO_MASTER_EFFECT_PANEL_H__

#include "moMasterPlugin.h"

#define MO_ACTION_SWITCH_EFFECT_0 0
#define MO_ACTION_SWITCH_EFFECT_1 1
#define MO_ACTION_SWITCH_EFFECT_2 2
#define MO_ACTION_SWITCH_EFFECT_3 3
#define MO_ACTION_SWITCH_EFFECT_4 4
#define MO_ACTION_SWITCH_EFFECT_5 5
#define MO_ACTION_SWITCH_EFFECT_6 6
#define MO_ACTION_SWITCH_EFFECT_7 7
#define MO_ACTION_SWITCH_EFFECT_8 8
#define MO_ACTION_SWITCH_EFFECT_9 9
#define MO_ACTION_SWITCH_EFFECT_10 10
#define MO_ACTION_SWITCH_EFFECT_11 11
#define MO_ACTION_SWITCH_EFFECT_12 12
#define MO_ACTION_SWITCH_EFFECT_13 13
#define MO_ACTION_SWITCH_EFFECT_14 14
#define MO_ACTION_SWITCH_EFFECT_15 15
#define MO_ACTION_SWITCH_EFFECT_16 16
#define MO_ACTION_SWITCH_EFFECT_17 17
#define MO_ACTION_SWITCH_EFFECT_18 18
#define MO_ACTION_SWITCH_EFFECT_19 19
#define MO_ACTION_SWITCH_EFFECT_20 20
#define MO_ACTION_SWITCH_EFFECT_21 21
#define MO_ACTION_SWITCH_EFFECT_22 22
#define MO_ACTION_SWITCH_EFFECT_23 23
#define MO_ACTION_SWITCH_EFFECT_24 24
#define MO_ACTION_SWITCH_EFFECT_25 25
#define MO_ACTION_SWITCH_EFFECT_26 26
#define MO_ACTION_SWITCH_EFFECT_27 27
#define MO_ACTION_SWITCH_EFFECT_28 28
#define MO_ACTION_SWITCH_EFFECT_29 29
#define MO_ACTION_SWITCH_EFFECT_30 30
#define MO_ACTION_SWITCH_EFFECT_31 31
#define MO_ACTION_SWITCH_EFFECT_32 32
#define MO_ACTION_SWITCH_EFFECT_33 33
#define MO_ACTION_SWITCH_EFFECT_34 34
#define MO_ACTION_SWITCH_EFFECT_35 35
#define MO_ACTION_SWITCH_EFFECT_36 36
#define MO_ACTION_SWITCH_EFFECT_37 37
#define MO_ACTION_SWITCH_EFFECT_38 38
#define MO_ACTION_SWITCH_EFFECT_39 39
#define MO_ACTION_SWITCH_PRE_EFFECT_0 40
#define MO_ACTION_SWITCH_PRE_EFFECT_1 41
#define MO_ACTION_SWITCH_PRE_EFFECT_2 42
#define MO_ACTION_SWITCH_PRE_EFFECT_3 43
#define MO_ACTION_SWITCH_PRE_EFFECT_4 44
#define MO_ACTION_SWITCH_PRE_EFFECT_5 45
#define MO_ACTION_SWITCH_PRE_EFFECT_6 46
#define MO_ACTION_SWITCH_PRE_EFFECT_7 47
#define MO_ACTION_SWITCH_PRE_EFFECT_8 48
#define MO_ACTION_SWITCH_PRE_EFFECT_9 49
#define MO_ACTION_SWITCH_PRE_EFFECT_10 50
#define MO_ACTION_SWITCH_PRE_EFFECT_11 51
#define MO_ACTION_SWITCH_POST_EFFECT_0 52
#define MO_ACTION_SWITCH_POST_EFFECT_1 53
#define MO_ACTION_SWITCH_POST_EFFECT_2 54
#define MO_ACTION_SWITCH_POST_EFFECT_3 55
#define MO_ACTION_SWITCH_POST_EFFECT_4 56
#define MO_ACTION_SWITCH_POST_EFFECT_5 57
#define MO_ACTION_SWITCH_POST_EFFECT_6 58
#define MO_ACTION_SWITCH_POST_EFFECT_7 59
#define MO_ACTION_SWITCH_POST_EFFECT_8 60
#define MO_ACTION_SWITCH_POST_EFFECT_9 61
#define MO_ACTION_SWITCH_POST_EFFECT_10 62
#define MO_ACTION_SWITCH_POST_EFFECT_11 63
#define MO_ACTIONS_PANEL 64

class moMasterEffectPanel : public moMasterEffect
{
public:
    MOswitch *onlist;

    moMasterEffectPanel();
    virtual ~moMasterEffectPanel();
    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void Interaction(moIODeviceManager *);
};

class moMasterEffectPanelFactory : public moMasterEffectFactory
{
public:
    moMasterEffectPanelFactory() {}
    virtual ~moMasterEffectPanelFactory() {}
    moMasterEffect* Create();
    void Destroy(moMasterEffect* fx);
};

extern "C"
{
MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory();
MO_PLG_API void DestroyMasterEffectFactory();
}

#endif
