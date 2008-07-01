/*******************************************************************************

                                moEffectProtein.h

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *   (at your option) any later version.                                    *
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

#ifndef __MO_EFFECT_PROTEIN_H__
#define __MO_EFFECT_PROTEIN_H__

#include "moPlugin.h"
#include "moCurve.h"
#include "moArcBall.h"

#define MO_MOUSE_MOTION_X 0
#define MO_MOUSE_MOTION_Y 1
#define MO_MOUSE_DOWN_LEFT 2
#define MO_MOUSE_MOTION_Y_RIGHT 3
#define MO_KEY_PRESS_S 4
#define MO_KEY_PRESS_C 5
#define MO_KEY_PRESS_H 6
#define MO_KEY_PRESS_R 7
#define MO_KEY_PRESS_A 8
#define MO_KEY_PRESS_B 9
#define MO_KEY_PRESS_1 10
#define MO_KEY_PRESS_2 11
#define MO_KEY_PRESS_3 12
#define MO_KEY_PRESS_4 13

class moEffectProtein: public moEffect
{
public:
    moEffectProtein();
    virtual ~moEffectProtein();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    void Interaction(moIODeviceManager *IODeviceManager);
    MOboolean Finish();
private:
    void SetupGeometry();
    void SetupBlend();
    void SetupLights();
    void SetupMaterials();
    void SetView();
    void UpdateRotation();
    void RotateView();
    void ShakeBabyShake();
    void DrawGeometry();
    void RestoreConf();

    GLUquadric* QTube;
    GLUquadric* QSphere;
    int LSphere, LTube;

    MOint fn_protein, pl_in_plugin;
    MOint pl_ss_plugin, pl_conect_file;
    int RibbonDetail, NRDivisions, RibbonStyle;
    bool UseVDWradius;
    double RibbonWidth[3];
    double HelixWidth;
    double AtomRadius, BondRadius;
    bool NonProtEnabled;
    bool RibbonEnabled;
    bool AtomsEnabled;
    bool BondsEnabled;
    bool UseSecStrColors;

    vector_3d RibbonColor, AtomColor, BondColor, DefColor;
    vector_3d AtomColors[9];
    double VDWradius[9];
    double SecStrColors[3][3];

    moBspline spline_side1, spline_center, spline_side2;
    vector_3d FlipTestV;
    vector_3d Center;
    double GyrRad;
    double GyrRadCoeff;
    double MouseScaling;
    double SizeScaling;

    void DrawSpline(int n);
    void DrawFlatRibbonPatch();
    void DrawSolidRibbonSection(double HeightFactor);
    void SetRibbonColor(int id, int aa);
    void DrawRibbon(int id, int aa);
    void DrawLineRibbon();
    void DrawFlatRibbon();
    void DrawStrandRibbon();
    void DrawTubeRibbon();
    void ShiftControlPoints();
    void AddControlPoints(int id, int aa);
    void ConstructControlPoints(int id, int aa);

    void DrawSphere(const vector_3d &p0, double r, const vector_3d &color);
    void DrawTube(const vector_3d &v, const vector_3d &p0, double l, double r, const vector_3d &color);
    int AtomIndex(const char* name);
    void SetAtomColor(int idx);
    void DrawAtoms(int id, int aa);
    void DrawAtoms(int hg);
    void DrawBonds(int id, int aa);
    void DrawBonds(int hg);

    MOint transx, transy, transz;
    MOint rotatx, rotaty, rotatz;
    MOint scalex, scaley, scalez;

    MOint eyex,eyey,eyez;
    MOint viewx,viewy,viewz;

    MOint lightx,lighty,lightz;

    MOint alpha;
    MOint ambient, diffuse, specular;
    MOdouble AlphaV;

    // Mouse input.
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

    bool show_cursor, scaling;
};

class moEffectProteinFactory : public moEffectFactory
{
public:
    moEffectProteinFactory() {}
    virtual ~moEffectProteinFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
