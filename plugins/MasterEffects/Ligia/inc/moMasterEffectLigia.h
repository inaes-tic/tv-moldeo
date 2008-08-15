/*******************************************************************************

                            moMasterEffectLigia.h

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

#ifndef __MO_MASTER_EFFECT_LIGIA_H__
#define __MO_MASTER_EFFECT_LIGIA_H__

#define MO_ACTION_SHUTDOWN_LIGIA_CONSOLE	0
#define MO_ACTION_PAUSE_LIGIA_CONSOLE		1
#define MO_ACTION_LIGIA_AUTOMATIC_MODE		2
#define MO_ACTION_REINIT_LIGIA			3
#define MO_ACTION_SAVE_SCREEN			4
#define MO_ACTION_SET_FPS			5
#define MO_ACTION_SHUTDOWN_EFFECTS		6
#define MO_ACTION_SHUTDOWN_PREPOST_EFFECTS	7
#define MO_ACTION_SHUTDOWN_MASTER_EFFECTS	8

class moMasterEffectLigia : public moMasterEffect
{
public:
    bool done;

    //config
    MOint sounds;
    MOint live;

    //textures
    MOint preview, stream;

    //interno
    MOswitch *selected_array;

    moTextureIndex Images;

    moMasterEffectLigia();
    virtual ~moMasterEffectLigia();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void Interaction(moIODeviceManager *);
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

};

class moMasterEffectLigiaFactory : public moMasterEffectFactory
{
public:
    moMasterEffectLigiaFactory() {}
    virtual ~moMasterEffectLigiaFactory() {}
    moMasterEffect* Create();
    void Destroy(moMasterEffect* fx);
};

extern "C"
{
MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory();
MO_PLG_API void DestroyMasterEffectFactory();
}

#endif
