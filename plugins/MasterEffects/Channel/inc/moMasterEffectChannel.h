/*******************************************************************************

                            moMasterEffectChannel.h

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

#ifndef __MO_MASTER_EFFECT_CHANNEL_H__
#define __MO_MASTER_EFFECT_CHANNEL_H__

#include "moMasterPlugin.h"

//esto debe tener el mismo orden que en el config
#define MO_ACTION_SEL_EFFECT_0			0
#define MO_ACTION_SEL_EFFECT_1			1
#define MO_ACTION_SEL_EFFECT_2			2
#define MO_ACTION_SEL_EFFECT_3			3
#define MO_ACTION_SEL_EFFECT_4			4
#define MO_ACTION_SEL_EFFECT_5			5
#define MO_ACTION_SEL_EFFECT_6			6
#define MO_ACTION_SEL_EFFECT_7			7
#define MO_ACTION_SEL_EFFECT_8			8
#define MO_ACTION_SEL_EFFECT_9			9
#define MO_ACTION_SEL_EFFECT_10		10
#define MO_ACTION_SEL_EFFECT_11		11
#define MO_ACTION_SEL_EFFECT_12		12
#define MO_ACTION_SEL_EFFECT_13		13
#define MO_ACTION_SEL_EFFECT_14		14
#define MO_ACTION_SEL_EFFECT_15		15
#define MO_ACTION_SEL_EFFECT_16		16
#define MO_ACTION_SEL_EFFECT_17		17
#define MO_ACTION_SEL_EFFECT_18		18
#define MO_ACTION_SEL_EFFECT_19		19
#define MO_ACTION_SEL_EFFECT_20		20
#define MO_ACTION_SEL_EFFECT_21		21
#define MO_ACTION_SEL_EFFECT_22		22
#define MO_ACTION_SEL_EFFECT_23		23
#define MO_ACTION_SEL_EFFECT_24		24
#define MO_ACTION_SEL_EFFECT_25		25
#define MO_ACTION_SEL_EFFECT_26		26
#define MO_ACTION_SEL_EFFECT_27		27
#define MO_ACTION_SEL_EFFECT_28		28
#define MO_ACTION_SEL_EFFECT_29		29
#define MO_ACTION_SEL_EFFECT_30		30
#define MO_ACTION_SEL_EFFECT_31		31
#define MO_ACTION_SEL_EFFECT_32		32
#define MO_ACTION_SEL_EFFECT_33		33
#define MO_ACTION_SEL_EFFECT_34		34
#define MO_ACTION_SEL_EFFECT_35		35
#define MO_ACTION_SEL_EFFECT_36		36
#define MO_ACTION_SEL_EFFECT_37		37
#define MO_ACTION_SEL_EFFECT_38		38
#define MO_ACTION_SEL_EFFECT_39		39
#define MO_ACTION_SEL_PRE_EFFECT_0		40
#define MO_ACTION_SEL_PRE_EFFECT_1		41
#define MO_ACTION_SEL_PRE_EFFECT_2		42
#define MO_ACTION_SEL_PRE_EFFECT_3		43
#define MO_ACTION_SEL_POST_EFFECT_0		44
#define MO_ACTION_SEL_POST_EFFECT_1		45
#define MO_ACTION_SEL_POST_EFFECT_2		46
#define MO_ACTION_SEL_POST_EFFECT_3		47
#define MO_ACTION_SYNCHRO_EFFECT		48
#define MO_ACTION_BEAT_PULSE_EFFECT		49
#define MO_ACTION_TEMPO_DELTA_SUB_EFFECT 	50
#define MO_ACTION_TEMPO_DELTA_BMO_EFFECT 	51
#define MO_ACTION_TEMPO_PITCH_EFFECT		52
#define MO_ACTION_TEMPO_FACTOR_SUB_EFFECT 	53
#define MO_ACTION_TEMPO_FACTOR_BMO_EFFECT 	54
#define MO_ACTION_ALPHA_SUB_EFFECT		55
#define MO_ACTION_ALPHA_BMO_EFFECT		56
#define MO_ACTION_WHEEL_ALPHA			57
#define MO_ACTION_AMPLITUDE_SUB_EFFECT		58
#define MO_ACTION_AMPLITUDE_BMO_EFFECT	59
#define MO_ACTION_WHEEL_AMPLITUDE		60
#define MO_ACTION_MAGNITUDE_SUB_EFFECT		61
#define MO_ACTION_MAGNITUDE_BMO_EFFECT	62
#define MO_ACTION_WHEEL_MAGNITUDE		63
#define MO_ACTION_TINT_SUB_EFFECT		64
#define MO_ACTION_TINT_BMO_EFFECT		65
#define MO_ACTION_WHEEL_TINT			66
#define MO_ACTION_WHEEL_CHROMANCY		67
#define MO_ACTION_WHEEL_SATURATION		68
#define MO_ACTION_PARAM_NEXT_EFFECT		69
#define MO_ACTION_PARAM_PREV_EFFECT		70
#define MO_ACTION_VALOR_NEXT_EFFECT		71
#define MO_ACTION_VALOR_PREV_EFFECT		72
#define MO_ACTION_PARAMSET_NEXT_EFFECT		73
#define MO_ACTION_PARAMSET_PREV_EFFECT		74
#define MO_ACTION_UNSELECT_EFFECTS		75
#define MO_ACTION_SEL_UNIQUE			76


#define MO_EFFECT_PREEFFECT		0
#define MO_EFFECT_EFFECT		4
#define MO_EFFECT_POSTEFFECT	44
#define MO_EFFECT_MASTEREFFECT	56

class moMasterEffectChannel : public moMasterEffect
{
public:
    char *testmemalloc;
    void TestMemAlloc();

    MOswitch *m_SelectedArray;
    MOint m_Selected;//-1 si nada, o : el indice correspondiente al efecto seleccionado

	MOboolean	m_bFirstSelected;

    moMasterEffectChannel();
    moMasterEffectChannel(char*);
    virtual ~moMasterEffectChannel();
    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void Interaction(moIODeviceManager *);
};

class moMasterEffectChannelFactory : public moMasterEffectFactory
{
public:
    moMasterEffectChannelFactory() {}
    virtual ~moMasterEffectChannelFactory() {}
    moMasterEffect* Create();
    void Destroy(moMasterEffect* fx);
};

extern "C"
{
MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory();
MO_PLG_API void DestroyMasterEffectFactory();
}

#endif
