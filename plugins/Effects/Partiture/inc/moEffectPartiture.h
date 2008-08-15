/*******************************************************************************

                                moEffectPartiture.h

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

#ifndef __MO_EFFECT_PARTITURE_H__
#define __MO_EFFECT_PARTITURE_H__

#include "moPlugin.h"
#include "mo3dModelManager.h"
#include "moCTrack.h"

#define MO_PARTITURE_TRANSLATE_X		0
#define MO_PARTITURE_TRANSLATE_Y		1
#define MO_PARTITURE_TRANSLATE_Z		2
#define MO_PARTITURE_SCALE_X			3
#define MO_PARTITURE_SCALE_Y			4
#define MO_PARTITURE_SCALE_Z			5

enum moPartitureParamIndex {
	PARTITURE_ALPHA = 0,
	PARTITURE_COLOR = 1,
	PARTITURE_SYNC = 2,
	PARTITURE_PHASE = 3,
	PARTITURE_TEXTURE = 4,
	PARTITURE_SPECULAR = 5,
	PARTITURE_DIFFUSE = 6,
	PARTITURE_AMBIENT = 7,
	PARTITURE_FONT = 8,
	PARTITURE_SCROLLTIME = 9,
	PARTITURE_PENTAGRAMABACK = 10,
	PARTITURE_PENTAGRAMAFRONT = 11,
	PARTITURE_PENTAGRAMATEXTURE = 12,
	PARTITURE_TRANSLATEX = 13,
	PARTITURE_TRANSLATEY = 14,
	PARTITURE_TRANSLATEZ = 15,
	PARTITURE_ROTATEX = 16,
	PARTITURE_ROTATEY,
	PARTITURE_ROTATEZ,
	PARTITURE_SCALEX,
	PARTITURE_SCALEY,
	PARTITURE_SCALEZ,
	PARTITURE_EYEX,
	PARTITURE_EYEY,
	PARTITURE_EYEZ,
	PARTITURE_VIEWX,
	PARTITURE_VIEWY,
	PARTITURE_VIEWZ,
	PARTITURE_LIGHTX,
	PARTITURE_LIGHTY,
	PARTITURE_LIGHTZ,
	PARTITURE_INLET,
	PARTITURE_OUTLET
};

#define PARTITURE_MAXTRACKS 4

class moEffectPartiture: public moEffect
{
public:
    moEffectPartiture();
    virtual ~moEffectPartiture();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    virtual void Update( moEventList* p_EventList );
	void Interaction( moIODeviceManager *IODeviceManager );
	moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );
    MOboolean Finish();

private:

    void DrawPentagram( float x, float y, float w, float h );
    void HeaderToY( moCNote *p_CNote, float h, float yPent, float& y, bool &sostenido);


    moFont* m_pFont;

    MOint g_ViewMode;

    MOint maestrofont;

	MOfloat	Tx,Ty,Tz,Sx,Sy,Sz;

    mo3dModelManagerRef Models3d;

    //moCNotes  tracks[PARTITURE_MAXTRACKS];

    moCTrack  CTracks[PARTITURE_MAXTRACKS];

};

class moPartitureFactory : public moEffectFactory
{
    public:
        moPartitureFactory() {}
        virtual ~moPartitureFactory() {}
        moEffect* Create();
        void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
