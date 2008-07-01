/*******************************************************************************

                                moEffectGrid.h

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

#ifndef __MO_EFFECT_GRID_H__
#define __MO_EFFECT_GRID_H__

#include "moPlugin.h"
#include "supereg/tmotor.h"

#define MO_GRID_UP 		0
#define MO_GRID_DOWN		1
#define MO_GRID_TURN_LEFT	2
#define MO_GRID_TURN_RIGHT	3
#define MO_GRID_TILT_LEFT	4
#define MO_GRID_TILT_RIGHT	5
#define MO_GRID_FORWARD	6
#define MO_GRID_BACK		7
#define MO_GRID_PITCH		8
#define MO_GRID_TRIM		9

class moEffectGrid: public moEffect
{
public:
    TEngine_Utility *Grid;

    moEffectGrid();
    virtual ~moEffectGrid();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void Interaction(moIODeviceManager*);
private:
	moTextureManager*	MOTextures;
    MOint color, textura, texturab;
    MOtexture   *textures;
    MOuint      ntextures;

    PFNGLMULTITEXCOORD2FARBPROC  glMultiTexCoord2fARB;
    PFNGLACTIVETEXTUREARBPROC    glActiveTextureARB;
};

class moEffectGridFactory : public moEffectFactory 
{
public:
    moEffectGridFactory() {}
    virtual ~moEffectGridFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
