/*******************************************************************************

                            moPreEffectErase.h

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

#ifndef __MO_PRE_EFFECT_ERASE_H__
#define __MO_PRE_EFFECT_ERASE_H__

#include "moPrePlugin.h"

enum moEraseIndex {
    ERASE_ALPHA,
    ERASE_COLOR
};

class moPreEffectErase : public moPreEffect
{
public:
    MOint color;

    moPreEffectErase();
    virtual ~moPreEffectErase();
	moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    MOboolean Init();
    void Draw(moTempo*, moEffectState* = NULL);
    MOboolean Finish();
};

class moPreEffectEraseFactory : public moPreEffectFactory
{
public:
    moPreEffectEraseFactory() {}
    virtual ~moPreEffectEraseFactory() {}
    moPreEffect* Create();
    void Destroy(moPreEffect* fx);
};

extern "C"
{
MO_PLG_API moPreEffectFactory* CreatePreEffectFactory();
MO_PLG_API void DestroyPreEffectFactory();
}

#endif
