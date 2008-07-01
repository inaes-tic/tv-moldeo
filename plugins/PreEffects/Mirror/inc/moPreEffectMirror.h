/*******************************************************************************

                            moPreEffectMirror.h

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

#ifndef __MO_PRE_EFFECT_MIRROR_H__
#define __MO_PRE_EFFECT_MIRROR_H__

#include "moPrePlugin.h"

enum moMirrorIndex {
    MIRROR_ALPHA,
	MIRROR_SYNCRO,
	MIRROR_PHASE,
	MIRROR_COLOR,
	MIRROR_POLYGONMODE,
	MIRROR_RADIUS,
	MIRROR_ABERRATION,
	MIRROR_CONFIGURATION,
	MIRROR_BLENDING
};

class moPreEffectMirror : public moPreEffect
{
public:
	MOint MO_MESHA_WIDTH;
    MOint MO_MESHA_HEIGHT;
    MOint MO_MESHA_TEX;

	ajCoord **MESHAG;

	MOint MIRRORRENDERWIDTH;
	MOint MIRRORRENDERHEIGHT;

    moPreEffectMirror();
    virtual ~moPreEffectMirror();

    MOboolean Init();
    void ReInit();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );
};

class moPreEffectMirrorFactory : public moPreEffectFactory
{
public:
    moPreEffectMirrorFactory() {}
    virtual ~moPreEffectMirrorFactory() {}
    moPreEffect* Create();
    void Destroy(moPreEffect* fx);
};

extern "C"
{
MO_PLG_API moPreEffectFactory* CreatePreEffectFactory();
MO_PLG_API void DestroyPreEffectFactory();
}

#endif
