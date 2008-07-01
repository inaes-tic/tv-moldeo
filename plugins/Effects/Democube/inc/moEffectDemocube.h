/*******************************************************************************

                              moEffectDemocube.h

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
  Andres Colubri

*******************************************************************************/

#ifndef __MO_EFFECT_DEMOCUBE_H__
#define __MO_EFFECT_DEMOCUBE_H__

#include "moPlugin.h"
#include "moMathCurve.h"
#include "moMathVector3.h"
#include "moArcBall.h"
#include "moTextureFilterIndex.h"

#define MO_MOUSE_MOTION_X 0
#define MO_MOUSE_MOTION_Y 1
#define MO_MOUSE_DOWN_LEFT 2
#define MO_MOUSE_MOTION_Y_RIGHT 3
#define MO_KEY_PRESS_C 4

#define CUBE_NCELLS 10

enum {
X = 0,
Y = 1,
Z = 2
};

enum moDemocubeParamIndex {
	DEMOCUBE_ALPHA,
	DEMOCUBE_COLOR,
	DEMOCUBE_SYNC,
	DEMOCUBE_PHASE,
	DEMOCUBE_BLENDING,
	DEMOCUBE_DEFORM,
	DEMOCUBE_LENGTH,
	DEMOCUBE_ZDISP,
	DEMOCUBE_TEXTURELEFT,
	DEMOCUBE_TEXTUREFRONT,
	DEMOCUBE_TEXTURERIGHT,
	DEMOCUBE_TEXTUREBACK,
	DEMOCUBE_TEXTURETOP,
	DEMOCUBE_TEXTUREBOTTOM,
	DEMOCUBE_TRANSLATEX,
	DEMOCUBE_TRANSLATEY,
	DEMOCUBE_TRANSLATEZ,
	DEMOCUBE_ROTATEX,
	DEMOCUBE_ROTATEY,
	DEMOCUBE_ROTATEZ,
	DEMOCUBE_SCALEX,
	DEMOCUBE_SCALEY,
	DEMOCUBE_SCALEZ,
	DEMOCUBE_INLET,
	DEMOCUBE_OUTLET
};

class moEffectDemocube: public moEffect
{
public:
    moEffectDemocube();
    virtual ~moEffectDemocube();

    MOboolean Init();
    void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void Interaction( moIODeviceManager * );

    virtual moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

private:
	moTextureFilterIndex filters;

	// Parameter indices.
    MOint color_idx;
	MOint blending_idx;

	// Parameters.
	MOfloat m_temp_angle;
	MOfloat m_cube_length;
	MOfloat m_max_displacement;
	MOboolean m_deformation;
	MOfloat m_half_cube_length, m_cube_grid_dl;
	moVector3f m_face_grid[CUBE_NCELLS + 1][CUBE_NCELLS + 1];
	moVector3f rot_center;

    // Mouse input.
    bool show_cursor;
    MOdouble distance_z;

    // ArcBall rotation using the mouse.
    ArcBallT    ArcBall; // ArcBall Instance
    Point2fT    MousePt;                 // Current Mouse Point
    bool        isClicked;               // Clicking The Mouse?
    bool        isRPressed;              // Pressing The Reset Button?
    bool        isDragging;              // Dragging The Mouse?
    Matrix4fT   Transform;               // Final Transform
    Matrix3fT   LastRot;                 // Last Rotation
    Matrix3fT   ThisRot;                 // This Rotation

	void SetColor();
	void SetBlending();
	void SetView();

	void DrawCube();
	void DrawCubeFace(const moVector3f &lbcorner, const moVector3f &vx, const moVector3f &vy, const moVector3f &vz, MOint glid, MOfloat mint, MOfloat maxt);
	MOfloat GenZDisplacement(MOfloat x, MOfloat y);
	void GetPointAndNormal(MOint i, MOint j, const moVector3f &vz, moVector3f &point, moVector3f &normal);

	void UpdateRotation();
};

class moEffectDemocubeFactory : public moEffectFactory {

public:
    moEffectDemocubeFactory() {}
    virtual ~moEffectDemocubeFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
