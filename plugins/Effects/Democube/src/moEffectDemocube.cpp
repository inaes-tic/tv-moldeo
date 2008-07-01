/*******************************************************************************

                             moEffectDemocube.cpp

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

  Copyright (C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andres Colubri

*******************************************************************************/

#include "moEffectDemocube.h"

//========================
//  Factory
//========================

moEffectDemocubeFactory *m_EffectIconoFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if (m_EffectIconoFactory==NULL)
		m_EffectIconoFactory = new moEffectDemocubeFactory();
	return (moEffectFactory*) m_EffectIconoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectIconoFactory;
	m_EffectIconoFactory = NULL;
}

moEffect*  moEffectDemocubeFactory::Create() {
	return new moEffectDemocube();
}

void moEffectDemocubeFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectDemocube::moEffectDemocube()
{
	SetName("democube");
}

moEffectDemocube::~moEffectDemocube()
{
	Finish();
}

MOboolean moEffectDemocube::Init()
{
    if (!PreInit()) return false;

	moDefineParamIndex( DEMOCUBE_ALPHA, moText("alpha") );
	moDefineParamIndex( DEMOCUBE_COLOR, moText("color") );
	moDefineParamIndex( DEMOCUBE_SYNC, moText("syncro") );
	moDefineParamIndex( DEMOCUBE_PHASE, moText("phase") );
	moDefineParamIndex( DEMOCUBE_BLENDING, moText("blending") );
	moDefineParamIndex( DEMOCUBE_DEFORM, moText("deform") );
	moDefineParamIndex( DEMOCUBE_LENGTH, moText("length") );
	moDefineParamIndex( DEMOCUBE_ZDISP, moText("zdisp") );
	moDefineParamIndex( DEMOCUBE_TEXTURELEFT, moText("textureleft") );
	moDefineParamIndex( DEMOCUBE_TEXTUREFRONT, moText("texturefront") );
	moDefineParamIndex( DEMOCUBE_TEXTURERIGHT, moText("textureright") );
	moDefineParamIndex( DEMOCUBE_TEXTUREBACK, moText("textureback") );
	moDefineParamIndex( DEMOCUBE_TEXTURETOP, moText("texturetop") );
	moDefineParamIndex( DEMOCUBE_TEXTUREBOTTOM, moText("texturebottom") );
	moDefineParamIndex( DEMOCUBE_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( DEMOCUBE_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( DEMOCUBE_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( DEMOCUBE_ROTATEX, moText("rotatex") );
	moDefineParamIndex( DEMOCUBE_ROTATEY, moText("rotatey") );
	moDefineParamIndex( DEMOCUBE_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( DEMOCUBE_SCALEX, moText("scalex") );
	moDefineParamIndex( DEMOCUBE_SCALEY, moText("scaley") );
	moDefineParamIndex( DEMOCUBE_SCALEZ, moText("scalez") );
	moDefineParamIndex( DEMOCUBE_INLET, moText("inlet") );
	moDefineParamIndex( DEMOCUBE_OUTLET, moText("outlet") );

	m_cube_grid_dl = m_cube_length / CUBE_NCELLS;
	m_half_cube_length = 0.5 * m_cube_length;

    // ArcBall rotation initialization.
    ArcBall.setBounds(m_pResourceManager->GetRenderMan()->RenderWidth(), m_pResourceManager->GetRenderMan()->RenderHeight());
    isClicked  = false;
    isRPressed = false;
    isDragging = false;

    Transform.s.XX = 1.0f;
    Transform.s.XY = 0.0f;
    Transform.s.XZ = 0.0f;
    Transform.s.XW = 0.0f;
    Transform.s.YX = 0.0f;
    Transform.s.YY = 1.0f;
    Transform.s.YZ = 0.0f;
    Transform.s.YW = 0.0f;
    Transform.s.ZX = 0.0f;
    Transform.s.ZY = 0.0f;
    Transform.s.ZZ = 1.0f;
    Transform.s.ZW = 0.0f;
    Transform.s.TX = 0.0f;
    Transform.s.TY = 0.0f;
    Transform.s.TZ = 0.0f;
    Transform.s.TW = 1.0f;
    LastRot.s.XX = 1.0f;
    LastRot.s.XY = 0.0f;
    LastRot.s.XZ = 0.0f;
    LastRot.s.YX = 0.0f;
    LastRot.s.YY = 1.0f;
    LastRot.s.YZ = 0.0f;
    LastRot.s.ZX = 0.0f;
    LastRot.s.ZY = 0.0f;
    LastRot.s.ZZ = 1.0f;
    ThisRot.s.XX = 1.0f;
    ThisRot.s.XY = 0.0f;
    ThisRot.s.XZ = 0.0f;
    ThisRot.s.YX = 0.0f;
    ThisRot.s.YY = 1.0f;
    ThisRot.s.YZ = 0.0f;
    ThisRot.s.ZX = 0.0f;
    ThisRot.s.ZY = 0.0f;
    ThisRot.s.ZZ = 1.0f;

    distance_z = 0;
    show_cursor = 0;

	rot_center[X] = 0.5 * m_cube_length;
	rot_center[Y] = 0.5 * m_cube_length;
	rot_center[Z] = -0.5 * m_cube_length;

    return true;
}

void moEffectDemocube::Draw( moTempo* tempogral, moEffectState* parentstate)
{
	PreDraw(tempogral, parentstate);

	m_temp_angle = fmod(float(state.tempo.ang), float(2.0 * MO_PI));

	SetColor();
	SetBlending();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    SetView();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	DrawCube();
}

MOboolean moEffectDemocube::Finish()
{
    PreFinish();
	filters.Finish();
	return true;
}

void moEffectDemocube::Interaction( moIODeviceManager *IODeviceManager )
{
	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moEffect::Interaction( IODeviceManager );

	if (devicecode!=NULL)
	for(int i=0; i<ncodes; i++) {

		temp = devicecode[i].First;

		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);
			if (state)
			switch(i) {
				case MO_MOUSE_MOTION_X:
					MousePt.s.X = valor;
					break;
				case MO_MOUSE_MOTION_Y:
					MousePt.s.Y = valor;
					break;
				case MO_MOUSE_DOWN_LEFT:
					isClicked = true;
					break;
				case MO_MOUSE_MOTION_Y_RIGHT:
					distance_z -= 5.0 * valor;
					break;
				case MO_KEY_PRESS_C:
					show_cursor = !show_cursor;
					SDL_ShowCursor(show_cursor);
					break;
			}
		temp = temp->next;
		}
	}
    UpdateRotation();
}

void moEffectDemocube::SetColor()
{
	MOfloat color[4];

    for (int i = MO_RED; i <= MO_ALPHA; i++)
        color[i] = m_Config[moR(DEMOCUBE_COLOR)][MO_SELECTED][i].Fun()->Eval(state.tempo.ang);

    glColor4f(color[0] * state.tintr,
              color[1] * state.tintg,
              color[2] * state.tintb,
              color[3] * state.alpha);
}

void moEffectDemocube::SetBlending()
{
	MOint blending;

	blending = m_Config[moR(DEMOCUBE_BLENDING)].GetIndexValue();

    glEnable(GL_BLEND);
	switch (blending)
	{
		//ALPHA DEPENDENT
		case 0:
			//TRANSPARENCY [Rs * As] + [Rd *(1 -As)] = As*(Rs-Rd) + Rd
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			break;
		case 1:
			//ADDITIVE WITH TRANSPARENCY: Rs*As + Rd*Ad
			glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
			break;

		//NON ALPHA
		case 2:
			//MIXING [Rs *( 1 - Rd )] + [ Rd * 1] = Rs + Rd - Rs*Rd
			//additive without saturation
			glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ONE );
			break;
		case 3:
			//MULTIPLY: [Rs * Rd] + [Rd * 0] = Rs * Rd
			glBlendFunc( GL_DST_COLOR, GL_ZERO );
			break;
		case 4:
			//EXCLUSION: [Rs *(1 - Rd)] + [Rd *(1 - Rs)] = Rs + Rd - 2*Rs*Rd
			glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);//
			break;
		case 5:
			//ADDITIVE Rs+Rd
			glBlendFunc( GL_ONE, GL_ONE );
			break;
		case 6:
			//OVERLAY: 2*Rs*Rd
			glBlendFunc( GL_DST_COLOR, GL_SRC_COLOR );
			break;
		case 7:
			//SUBSTRACTIVE [Rs *( 1 - Rd )] + [ Rd * 0] = Rs - Rs*Rd
			//substractive
			glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ZERO );
			break;
		case 8:
			// [Rs * min(As,1-Ad) ] + [ Rd * Ad]
			//
			glBlendFunc( GL_SRC_ALPHA_SATURATE,  GL_DST_ALPHA);
			break;
			//Multiply mode:(a*b)
			//Average mode:(a+b)/2
			//Screen mode:  f(a,b) = 1 -(1-a) *(1-b)
			//Difference mode:  f(a,b) = |a - b|
			//Negation mode:  f(a,b) = 1 - |1 - a - b|
			//Exclusion mode f(a,b) = a + b - 2ab or f(a,b) = average(difference(a,b),negation(a,b))
			//Overlay mode f(a,b) =   	2ab(for a < ½) 1 - 2 *(1 - a) *(1 - b)(else)
			//Color dodge mode:  f(a,b) = a /(1 - b)
			//Color burn mode:  f(a,b) = 1 -(1 - a) / b
			//Inverse color dodge mode:  f(a,b) = b /(1 - a)
			//Inverse color burn mode:  f(a,b) = 1 -(1 - b) / a
		default:
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			break;
	}
}

void moEffectDemocube::DrawCube()
{
	moTexture* ptex;
	float mint, maxt, d;
	moVector3f lbcorner, vx, vy, vz;
	// Face 1.
	lbcorner[X] = 0;
	lbcorner[Y] = 0;
	lbcorner[Z] = 0;
	vx[X] = 1.0;
	vx[Y] = 0.0;
	vx[Z] = 0.0;
	vy[X] = 0.0;
	vy[Y] = 1.0;
	vy[Z] = 0.0;
	vz[X] = 0.0;
	vz[Y] = 0.0;
	vz[Z] = 1.0;
	ptex = filters[0]->GetDestTex()->GetTexture(0);
	d = 0.5 * (1.0 - float(ptex->GetHeight()) / float(ptex->GetWidth()));
	mint = 0.0 - d;
	maxt = 1.0 + d;
	DrawCubeFace(lbcorner, vx, vy, vz, filters[0]->GetDestTex()->GetGLId(0), mint, maxt);

	// Face 2.
	lbcorner[X] = m_cube_length;
	lbcorner[Y] = 0.0;
	lbcorner[Z] = 0.0;
	vx[X] = 0.0;
	vx[Y] = 0.0;
	vx[Z] = -1.0;
	vy[X] = 0.0;
	vy[Y] = 1.0;
	vy[Z] = 0.0;
	vz[X] = 1.0;
	vz[Y] = 0.0;
	vz[Z] = 0.0;
	ptex = filters[0]->GetSrcTex()->GetTexture(0);
	d = 0.5 * (1.0 - float(ptex->GetHeight()) / float(ptex->GetWidth()));
	mint = 0.0 - d;
	maxt = 1.0 + d;
	DrawCubeFace(lbcorner, vx, vy, vz, filters[0]->GetSrcTex()->GetGLId(0), mint, maxt);

	// Face 3.
	lbcorner[X] = m_cube_length;
	lbcorner[Y] = 0.0;
	lbcorner[Z] = -m_cube_length;
	vx[X] = -1.0;
	vx[Y] = 0.0;
	vx[Z] = 0.0;
	vy[X] = 0.0;
	vy[Y] = 1.0;
	vy[Z] = 0.0;
	vz[X] = 0.0;
	vz[Y] = 0.0;
	vz[Z] = -1.0;
	ptex = filters[1]->GetDestTex()->GetTexture(0);
	d = 0.5 * (1.0 - float(ptex->GetHeight()) / float(ptex->GetWidth()));
	mint = 0.0 - d;
	maxt = 1.0 + d;
	DrawCubeFace(lbcorner, vx, vy, vz, filters[1]->GetDestTex()->GetGLId(0), mint, maxt);

	// Face 4.
	lbcorner[X] = 0.0;
	lbcorner[Y] = 0.0;
	lbcorner[Z] = -m_cube_length;
	vx[X] = 0.0;
	vx[Y] = 0.0;
	vx[Z] = 1.0;
	vy[X] = 0.0;
	vy[Y] = 1.0;
	vy[Z] = 0.0;
	vz[X] = -1.0;
	vz[Y] = 0.0;
	vz[Z] = 0.0;
	ptex = filters[1]->GetSrcTex()->GetTexture(0);
	d = 0.5 * (1.0 - float(ptex->GetHeight()) / float(ptex->GetWidth()));
	mint = 0.0 - d;
	maxt = 1.0 + d;
	DrawCubeFace(lbcorner, vx, vy, vz, filters[1]->GetSrcTex()->GetGLId(0), mint, maxt);

	// Face 5.
	lbcorner[X] = 0.0;
	lbcorner[Y] = m_cube_length;
	lbcorner[Z] = 0.0;
	vx[X] = 1.0;
	vx[Y] = 0.0;
	vx[Z] = 0.0;
	vy[X] = 0.0;
	vy[Y] = 0.0;
	vy[Z] = -1.0;
	vz[X] = 0.0;
	vz[Y] = 1.0;
	vz[Z] = 0.0;
	ptex = filters[2]->GetDestTex()->GetTexture(0);
	d = 0.5 * (1.0 - float(ptex->GetHeight()) / float(ptex->GetWidth()));
	mint = 0.0 - d;
	maxt = 1.0 + d;
	DrawCubeFace(lbcorner, vx, vy, vz, filters[2]->GetDestTex()->GetGLId(0), mint, maxt);

	// Face 6.
	lbcorner[X] = 0.0;
	lbcorner[Y] = 0.0;
	lbcorner[Z] = -m_cube_length;
	vx[X] = 1.0;
	vx[Y] = 0.0;
	vx[Z] = 0.0;
	vy[X] = 0.0;
	vy[Y] = 0.0;
	vy[Z] = 1.0;
	vz[X] = 0.0;
	vz[Y] = -1.0;
	vz[Z] = 0.0;
	ptex = filters[3]->GetDestTex()->GetTexture(0);
	d = 0.5 * (1.0 - float(ptex->GetHeight()) / float(ptex->GetWidth()));
	mint = 0.0 - d;
	maxt = 1.0 + d;
	DrawCubeFace(lbcorner, vx, vy, vz, filters[3]->GetDestTex()->GetGLId(0), mint, maxt);
}

void moEffectDemocube::DrawCubeFace(const moVector3f &lbcorner, const moVector3f &vx, const moVector3f &vy, const moVector3f &vz, MOint glid, MOfloat mint, MOfloat maxt)
{
	if (m_deformation)
	{
		moVector3f face_point0p, face_point1p, face_point0, face_point1;
		moVector3f normal0, normal1;
		MOfloat x, y, z;
		MOint i, j;
		MOfloat s, t;

		// Generating the face grid.
		for (j = 0; j < CUBE_NCELLS; j++)
		{
			for (i = 0; i <= CUBE_NCELLS; i++)
			{
				// Point contained in the face plane, corresponding to (i, j).
				x = m_cube_grid_dl * i;
				y = m_cube_grid_dl * j;
				face_point0p = x * vx + y * vy;
				z = GenZDisplacement(x, y);
				face_point0 = face_point0p + z * vz;
				m_face_grid[i][j] = face_point0;

				// Point contained in the face plane, corresponding to (i, j + 1).
				y = m_cube_grid_dl * (j + 1);
				face_point1p = x * vx + y * vy;
				z = GenZDisplacement(x, y);
				face_point1 = face_point1p + z * vz;
				m_face_grid[i][j+1] = face_point1;
			}
		}

		// Rendering the face, applying textures and normals.
		glBindTexture(GL_TEXTURE_2D, glid);
		for (j = 0; j < CUBE_NCELLS; j++)
		{
			glBegin(GL_QUAD_STRIP);
				for (i = 0; i <= CUBE_NCELLS; i++)
				{
					s = float(i) / float(CUBE_NCELLS);
					t = mint + (float(j) / float(CUBE_NCELLS)) * (maxt - mint);
					GetPointAndNormal(i, j, vz, face_point0, normal0);
					glTexCoord2f(s, t);
					glNormal3f(normal0[X], normal0[Y], normal0[Z]);
					glVertex3f(lbcorner[X] + face_point0[X] - rot_center[X], lbcorner[Y] + face_point0[Y] - rot_center[Y], lbcorner[Z] + face_point0[Z] - rot_center[Z]);

					t = mint + (float(j + 1) / float(CUBE_NCELLS)) * (maxt - mint);
					GetPointAndNormal(i, j + 1, vz, face_point1, normal1);
					glTexCoord2f(s, t);
					glNormal3f(normal1[X], normal1[Y], normal1[Z]);
					glVertex3f(lbcorner[X] + face_point1[X] - rot_center[X], lbcorner[Y] + face_point1[Y] - rot_center[Y], lbcorner[Z] + face_point1[Z] - rot_center[Z]);
				}
			glEnd();
		}
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, glid);

		glNormal3f(vz[X], vz[Y], vz[Z]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, mint);
			glVertex3f(lbcorner[X] - rot_center[X], lbcorner[Y] - rot_center[Y], lbcorner[Z] - rot_center[Z]);

			glTexCoord2f(1.0, mint);
			glVertex3f(lbcorner[X] + m_cube_length * vx[X] - rot_center[X], lbcorner[Y] + m_cube_length * vx[Y] - rot_center[Y], lbcorner[Z] + m_cube_length * vx[Z] - rot_center[Z]);

			glTexCoord2f(1.0, maxt);
			glVertex3f(lbcorner[X] + m_cube_length * (vx[X] + vy[X]) - rot_center[X], lbcorner[Y] + m_cube_length * (vx[Y] + vy[Y]) - rot_center[Y], lbcorner[Z] + m_cube_length * (vx[Z] + vy[Z]) - rot_center[Z]);

			glTexCoord2f(0.0, maxt);
			glVertex3f(lbcorner[X] + m_cube_length * vy[X] - rot_center[X], lbcorner[Y] + m_cube_length * vy[Y] - rot_center[Y], lbcorner[Z] + m_cube_length * vy[Z] - rot_center[Z]);
		glEnd();
	}
}

// A displacement along the z direction is calculated upon the distance between the point with
// (x, y) coordinates and the rederence point (l/2, l/2), where l is the side length of
// the cube. This displacement is dependent on the tempo.
MOfloat moEffectDemocube::GenZDisplacement(MOfloat x, MOfloat y)
{
	MOfloat c2 = 4.0 / (m_cube_length * m_cube_length);
	MOfloat c4 = c2 * c2;

	MOfloat dx = x - m_half_cube_length;
	MOfloat dy = y - m_half_cube_length;
	MOfloat dx2 = dx * dx;
	MOfloat dy2 = dy * dy;

	MOfloat h = m_max_displacement * cos(m_temp_angle);

	return h * (c4 * (dx2 * dy2) - c2 * (dx2 + dy2) + 1);
}

void moEffectDemocube::GetPointAndNormal(MOint i, MOint j, const moVector3f &vz, moVector3f &point, moVector3f &normal)
{
	point = m_face_grid[i][j];
	if ((i == 0) || (j == 0)) normal = vz;
	else
	{
		moVector3f pointx, pointy, dirx, diry;

//		pointx.Normalize();

		pointx = m_face_grid[i - 1][j];
		pointy = m_face_grid[i][j - 1];

		dirx = pointx - point;
		diry = pointy - point;

		normal = dirx;
		normal.Cross(diry);
		normal.Normalize();
	}
}

void moEffectDemocube::SetView()
{
	MOint w = m_pResourceManager->GetRenderMan()->RenderWidth();
	MOint h = m_pResourceManager->GetRenderMan()->RenderHeight();

    gluPerspective(45.0f, (GLfloat)w/(GLfloat)h, 4.0f , 4000.0f);
    gluLookAt(0.0, 0.0, 2.0 * m_cube_length + distance_z,
              0.0, 0.0, 0.0,
              0, 1, 0);

    glMultMatrixf(Transform.M);
}

void moEffectDemocube::UpdateRotation()
{
    if (isRPressed)													// If Reset button pressed, Reset All Rotations
    {
		Matrix3fSetIdentity(&LastRot);								// Reset Rotation
		Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
        Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);		// Reset Rotation
    }

    if (!isDragging)												// Not Dragging
    {
        if (isClicked)												// First Click
        {
			isDragging = true;										// Prepare For Dragging
			LastRot = ThisRot;										// Set Last Static Rotation To Last Dynamic One
			ArcBall.click(&MousePt);								// Update Start Vector And Prepare For Dragging
		}
    }
    else
    {
        if (isClicked)												// Still Clicked, So Still Dragging
        {
            Quat4fT     ThisQuat;

            ArcBall.drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion
            Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
            Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
            Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
        }
        else														// No Longer Dragging
			isDragging = false;
    }
}



moConfigDefinition *
moEffectDemocube::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("blending"), MO_PARAM_TEXT, DEMOCUBE_BLENDING);
	p_configdefinition->Add( moText("deform"), MO_PARAM_FUNCTION, DEMOCUBE_DEFORM);
	p_configdefinition->Add( moText("length"), MO_PARAM_FUNCTION, DEMOCUBE_LENGTH);
	p_configdefinition->Add( moText("zdisp"), MO_PARAM_FUNCTION, DEMOCUBE_ZDISP);
	p_configdefinition->Add( moText("texturefront"), MO_PARAM_TEXTURE, DEMOCUBE_TEXTUREFRONT );
	p_configdefinition->Add( moText("textureright"), MO_PARAM_TEXTURE, DEMOCUBE_TEXTURERIGHT );
	p_configdefinition->Add( moText("textureback"), MO_PARAM_TEXTURE, DEMOCUBE_TEXTUREBACK );
	p_configdefinition->Add( moText("textureleft"), MO_PARAM_TEXTURE, DEMOCUBE_TEXTURELEFT );
	p_configdefinition->Add( moText("texturetop"), MO_PARAM_TEXTURE, DEMOCUBE_TEXTURETOP );
	p_configdefinition->Add( moText("texturebottom"), MO_PARAM_TEXTURE, DEMOCUBE_TEXTUREBOTTOM );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, DEMOCUBE_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, DEMOCUBE_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, DEMOCUBE_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, DEMOCUBE_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, DEMOCUBE_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, DEMOCUBE_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, DEMOCUBE_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, DEMOCUBE_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, DEMOCUBE_SCALEZ );
	return p_configdefinition;
}
