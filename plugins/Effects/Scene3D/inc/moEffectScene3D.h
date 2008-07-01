/*******************************************************************************

                              moEffectScene3D.h

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

#ifndef __MO_EFFECT_SCENE3D_H__
#define __MO_EFFECT_SCENE3D_H__

#include "moPlugin.h"
#include "3ds/3ds.h"

class moEffectScene3D: public moEffect
{
public:
    moEffectScene3D();
    virtual ~moEffectScene3D();

    MOboolean Init();
    void Draw( moTempo*,moEffectState* parentstate=NULL);
    MOboolean Finish();
private:
    MOint object;
    MOint g_ViewMode;

    MOint color, texture, size;
    MOint transx, transy, transz;
    MOint rotatx, rotaty, rotatz;
    MOint scalex, scaley, scalez;

    MOint eyex, eyey, eyez;
    MOint viewx, viewy, viewz;

    MOint lightx, lighty, lightz;

    MOint ambient, diffuse, specular;

    mo3dModelManagerRef Models3d;
};

class moEffectScene3DFactory : public moEffectFactory
{
public:
    moEffectScene3DFactory() {}
    virtual ~moEffectScene3DFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
