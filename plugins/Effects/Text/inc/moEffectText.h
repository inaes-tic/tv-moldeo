/*******************************************************************************

                              moEffectText.h

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

#ifndef __MO_EFFECT_TEXT_H__
#define __MO_EFFECT_TEXT_H__

#include "moPlugin.h"

#define MO_TEXT_TRANSLATE_X 0
#define MO_TEXT_TRANSLATE_Y 1
#define MO_TEXT_SCALE_X 2
#define MO_TEXT_SCALE_Y 3

enum moTextParamIndex {
	TEXT_ALPHA,
	TEXT_COLOR,
	TEXT_SYNC,
	TEXT_PHASE,
	TEXT_TEXTURE,
	TEXT_TEXT,
	TEXT_BLENDING,
	TEXT_WIDTH,
	TEXT_HEIGHT,
	TEXT_TRANSLATEX,
	TEXT_TRANSLATEY,
    TEXT_SCALEX,
	TEXT_SCALEY,
	TEXT_ROTATEZ,
    TEXT_INLET,
	TEXT_OUTLET
};


class moEffectText : public moEffect
{
    public:


        moEffectText();
        virtual ~moEffectText();

        MOboolean Init();
        void Draw(moTempo*, moEffectState* parentstate = NULL);
        MOboolean Finish();

        void Interaction( moIODeviceManager * );
        moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    private:

        GLuint m_GLBaseList; // Base Display List For The Font
        GLuint FontId;

        MOint Tx, Ty;
        MOfloat Sx, Sy;
        MOint Rz;

        moTexture*  m_pFont;

        //esto lo podriamos poner en el FontManager....
        GLvoid BuildFont(GLvoid);
        GLvoid glPrint(GLint x, GLint y, char *string, int set, float scx, float scy, float rt);
        GLvoid KillFont(GLvoid);
};

class moEffectTextFactory : public moEffectFactory
{
public:
    moEffectTextFactory() {}
    virtual ~moEffectTextFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
