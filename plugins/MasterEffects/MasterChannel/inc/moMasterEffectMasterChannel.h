/*******************************************************************************

                      moMasterEffectMasterChannel.h

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

#ifndef MO_MASTER_EFFECT_MASTERCHANNEL_H__
#define MO_MASTER_EFFECT_MASTERCHANNEL_H__

#include "moMasterPlugin.h"

#define MO_ACTION_SYNCHRONIZE_ALL		0
#define MO_ACTION_TEMPO_DELTA_SUB_MASTER	1
#define MO_ACTION_TEMPO_DELTA_BMO_MASTER	2
#define MO_ACTION_TEMPO_PITCH_MASTER		3
#define MO_ACTION_TEMPO_FACTOR_SUB_MASTER	4
#define MO_ACTION_TEMPO_FACTOR_BMO_MASTER	5
#define MO_ACTION_ALPHA_SUB_MASTER		6
#define MO_ACTION_ALPHA_BMO_MASTER		7
#define MO_ACTION_WHEEL_ALPHA_MASTER		8
#define MO_ACTION_AMPLITUDE_SUB_MASTER		9
#define MO_ACTION_AMPLITUDE_BMO_MASTER	10
#define MO_ACTION_WHEEL_AMPLITUDE_MASTER	11
#define MO_ACTION_MAGNITUDE_SUB_MASTER		12
#define MO_ACTION_MAGNITUDE_BMO_MASTER	13
#define MO_ACTION_WHEEL_MAGNITUDE_MASTER	14
#define MO_ACTION_TINT_SUB_MASTER		15
#define MO_ACTION_TINT_BMO_MASTER		16
#define MO_ACTION_WHEEL_TINT_MASTER		17
#define MO_ACTION_WHEEL_CHROMANCY_MASTER	18
#define MO_ACTION_WHEEL_SATURATION_MASTER	19
#define MO_ACTION_BEAT_PULSE_MASTER		20

class moMasterEffectMasterChannel : public moMasterEffect
{
public:
    moMasterEffectMasterChannel();
    virtual ~moMasterEffectMasterChannel();
    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void Interaction(moIODeviceManager *);

    void Synchronize();
};

class moMasterEffectMasterChannelFactory : public moMasterEffectFactory
{
public:
    moMasterEffectMasterChannelFactory() {}
    virtual ~moMasterEffectMasterChannelFactory() {}
    moMasterEffect* Create();
    void Destroy(moMasterEffect* fx);
};

extern "C"
{
MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory();
MO_PLG_API void DestroyMasterEffectFactory();
}
#endif
