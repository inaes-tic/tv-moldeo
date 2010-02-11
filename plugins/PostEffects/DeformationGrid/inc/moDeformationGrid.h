/*******************************************************************************

                                moDeformationGrid.h

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
  Gustavo Orrillo

*******************************************************************************/

#ifndef __MO_EFFECT_DEFORMATIONGRID_H__
#define __MO_EFFECT_DEFORMATIONGRID_H__

#include "moPostPlugin.h"

#define MO_DEFORMATIONGRID_TRANSLATE_X 0
#define MO_DEFORMATIONGRID_TRANSLATE_Y 1
#define MO_DEFORMATIONGRID_SCALE_X 2
#define MO_DEFORMATIONGRID_SCALE_Y 3

/**
*   Alpha
*
*   Texture: la textura
*   TEX_COORD_X1: coordenadas de la textura pos x1
*   TEX_COORD_Y1: coordenadas de la textura pos y1
*   TEX_COORD_X2: coordenadas de la textura pos x2
*   TEX_COORD_Y2: coordenadas de la textura pos y2
*/

enum moDeformationGridParamIndex {
	DEFORMATIONGRID_ALPHA,
	DEFORMATIONGRID_COLOR,
	DEFORMATIONGRID_SYNC,
	DEFORMATIONGRID_PHASE,
	DEFORMATIONGRID_TEXTURE,

    DEFORMATIONGRID_WIDTH,
    DEFORMATIONGRID_HEIGHT,
	DEFORMATIONGRID_POINTS, //if n vertex

    DEFORMATIONGRID_TEXCOORD_X1,
    DEFORMATIONGRID_TEXCOORD_Y1,
    DEFORMATIONGRID_TEXCOORD_X2,
    DEFORMATIONGRID_TEXCOORD_Y2,

	DEFORMATIONGRID_TRANSLATEX,
	DEFORMATIONGRID_TRANSLATEY,
	DEFORMATIONGRID_TRANSLATEZ,
	DEFORMATIONGRID_ROTATEX,
	DEFORMATIONGRID_ROTATEY,
	DEFORMATIONGRID_ROTATEZ,
	DEFORMATIONGRID_SCALEX,
	DEFORMATIONGRID_SCALEY,
	DEFORMATIONGRID_SCALEZ,

	DEFORMATIONGRID_SHOWGRID,
	DEFORMATIONGRID_CLEAR,
	DEFORMATIONGRID_PRECISION,
	DEFORMATIONGRID_MODE

};

enum moDeformationGridMode {
        PUNTO,
        LINEA,
        COLUMNA
};


enum moCodesDeformationGrid {
    DEFORMATIONGRID_MODO_1,
    DEFORMATIONGRID_MODO_2,
    DEFORMATIONGRID_MODO_3,
    DEFORMATIONGRID_SEL_IZQ,
    DEFORMATIONGRID_SEL_DER,
    DEFORMATIONGRID_SEL_ARR,
    DEFORMATIONGRID_SEL_ABA,
    DEFORMATIONGRID_CURSOR_IZQ,
    DEFORMATIONGRID_CURSOR_DER,
    DEFORMATIONGRID_CURSOR_ARR,
    DEFORMATIONGRID_CURSOR_ABA,
    DEFORMATIONGRID_RESET

};




class moDeformationGrid: public moPostEffect {

public:
    moDeformationGrid();
    virtual ~moDeformationGrid();

    MOboolean Init();
    void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void Interaction( moIODeviceManager *IODeviceManager );
	moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    void ResetPuntos();
    void UpdateParameters();


    private:
        moVector2f*  m_Points;
        moVector2f* m_TPoints;
        int m_Width;
        int m_Height;

        double m_Precision; ///la precision con la que desplaza el cursor
        int showgrid;
        int clear;

        int m_Selector_I;
        int m_Selector_J;
        MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;

        moDeformationGridMode m_Modo;

        void EscalarPuntos( bool horizontal, float escala );


};

class moDeformationGridFactory : public moPostEffectFactory {

public:
    moDeformationGridFactory() {};
    virtual ~moDeformationGridFactory() {};
    moPostEffect* Create();
    void Destroy(moPostEffect* fx);


};

extern "C"
{
MO_PLG_API moPostEffectFactory* CreatePostEffectFactory();
MO_PLG_API void DestroyPostEffectFactory();
}

#endif
