/*******************************************************************************

                              moEffectProtein.cpp

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
  Andrés Colubri

*******************************************************************************/

#include "moEffectProtein.h"

#include "PL/PL.h"
#include "PL/MoleculeConst.h"

//========================
//  Factory
//========================

moEffectProteinFactory *m_EffectProteinFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){ 
	if (m_EffectProteinFactory==NULL)
		m_EffectProteinFactory = new moEffectProteinFactory();
	return (moEffectFactory*) m_EffectProteinFactory;
} 

MO_PLG_API void DestroyEffectFactory(){ 
	delete m_EffectProteinFactory;
	m_EffectProteinFactory = NULL;
} 

moEffect*  moEffectProteinFactory::Create() {
	return new moEffectProtein();
}

void moEffectProteinFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectProtein::moEffectProtein() 
{
    devicecode = NULL;
    ncodes = 0;
    m_Name = "protein";
}

moEffectProtein::~moEffectProtein() 
{
    Finish();
}

MOboolean moEffectProtein::Init()
{
    int n, i;

    if (!PreInit()) return false;
    // Hacer la inicializacion de este efecto en particular.

    // Opciones.
    alpha = m_Config.GetParamIndex("alpha");
    ambient = m_Config.GetParamIndex("ambient");
    diffuse = m_Config.GetParamIndex("diffuse");
    specular = m_Config.GetParamIndex("specular");	

    transx = m_Config.GetParamIndex("translatex");
    transy = m_Config.GetParamIndex("translatey");
    transz = m_Config.GetParamIndex("translatez");
    rotatx = m_Config.GetParamIndex("rotatex");
    rotaty = m_Config.GetParamIndex("rotatey");
    rotatz = m_Config.GetParamIndex("rotatez");
    scalex = m_Config.GetParamIndex("scalex");
    scaley = m_Config.GetParamIndex("scaley");
    scalez = m_Config.GetParamIndex("scalez");

    lightx = m_Config.GetParamIndex("lightx");
    lighty = m_Config.GetParamIndex("lighty");
    lightz = m_Config.GetParamIndex("lightz");

    eyex = m_Config.GetParamIndex("eyex");
    eyey = m_Config.GetParamIndex("eyey");
    eyez = m_Config.GetParamIndex("eyez");
    viewx = m_Config.GetParamIndex("viewx");
    viewy = m_Config.GetParamIndex("viewy");
    viewz = m_Config.GetParamIndex("viewz");

    // Cargando parametros de la protein.
    fn_protein = m_Config.GetParamIndex("structure");
    pl_in_plugin = m_Config.GetParamIndex("plugin_in");
    pl_ss_plugin = m_Config.GetParamIndex("plugin_ss");
    pl_conect_file = m_Config.GetParamIndex("conectivity");

    RibbonStyle = m_Config.GetParam(m_Config.GetParamIndex("ribbon_style")).GetValueInt();
    RibbonDetail = m_Config.GetParam(m_Config.GetParamIndex("ribbon_detail")).GetValueInt();
    NRDivisions = m_Config.GetParam(m_Config.GetParamIndex("ribbon_divisions")).GetValueInt();
    HelixWidth = m_Config.GetParam(m_Config.GetParamIndex("helix_width")).GetValueFlt();
    GyrRadCoeff = m_Config.GetParam(m_Config.GetParamIndex("rgyr_coeff")).GetValueFlt();
    MouseScaling = m_Config.GetParam(m_Config.GetParamIndex("mouse_scaling")).GetValueFlt();
    SizeScaling = m_Config.GetParam(m_Config.GetParamIndex("size_scaling")).GetValueFlt();

    RibbonEnabled = m_Config.GetParam(m_Config.GetParamIndex("ribbon_enabled")).GetValueInt();
    AtomsEnabled = m_Config.GetParam(m_Config.GetParamIndex("atoms_enabled")).GetValueInt();
    BondsEnabled = m_Config.GetParam(m_Config.GetParamIndex("bonds_enabled")).GetValueInt();
    UseSecStrColors = m_Config.GetParam(m_Config.GetParamIndex("use_ss_colors")).GetValueInt();

    NonProtEnabled = m_Config.GetParam(m_Config.GetParamIndex("non_prot_enabled")).GetValueInt();
    UseVDWradius = m_Config.GetParam(m_Config.GetParamIndex("use_vdw_radius")).GetValueInt();
    AtomRadius = m_Config.GetParam(m_Config.GetParamIndex("atom_radius")).GetValueFlt();
    BondRadius = m_Config.GetParam(m_Config.GetParamIndex("bond_radius")).GetValueFlt();



    n = m_Config.GetParamIndex("vdw_atom_radius");
    for (i = 0; i <= 8; i++) VDWradius[i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("ribbon_width");
    for (i = HELIX; i <= TURN; i++) RibbonWidth[i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("def_color");
    for (i = 0; i <= 2; i++) DefColor[i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("bond_color");
    for (i = 0; i <= 2; i++) BondColor[i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("helix_color");
    for (i = 0; i <= 2; i++) SecStrColors[HELIX][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("strand_color");
    for (i = 0; i <= 2; i++) SecStrColors[STRAND][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("turn_color");
    for (i = 0; i <= 2; i++) SecStrColors[TURN][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("N_color");
    for (i = 0; i <= 2; i++) AtomColors[0][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("C_color");
    for (i = 0; i <= 2; i++) AtomColors[1][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("H_color");
    for (i = 0; i <= 2; i++) AtomColors[2][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("O_color");
    for (i = 0; i <= 2; i++) AtomColors[3][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("S_color");
    for (i = 0; i <= 2; i++) AtomColors[4][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("P_color");
    for (i = 0; i <= 2; i++) AtomColors[5][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("AS_color");
    for (i = 0; i <= 2; i++) AtomColors[6][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("FE_color");
    for (i = 0; i <= 2; i++) AtomColors[7][i] = m_Config.GetParam(n).GetValueFlt(i);

    n = m_Config.GetParamIndex("X_color");
    for (i = 0; i <= 2; i++) AtomColors[8][i] = m_Config.GetParam(n).GetValueFlt(i);

    moText fileprot = m_Config.GetParam(fn_protein).GetValueStr();
    moText fileconect = m_Config.GetParam(pl_conect_file).GetValueStr();
    moText fileplg = m_Config.GetParam(pl_in_plugin).GetValueStr();
    moText ssplg = m_Config.GetParam(pl_ss_plugin).GetValueStr();

    fileprot = "data/" + fileprot;
    fileconect = "data/" + fileconect;

    // Setting-up protein library.
    Center[X] = Center[Y] = Center[Z] = 0.0;
    FlipTestV[X] = FlipTestV[Y] = FlipTestV[Z] = 0.0;
    plLoadBondsList(fileconect); 
    int ssplg_idx = plAddPlugin(ssplg);
    plgSetDefConfig(ssplg_idx); 
    plLoadStrFromFile(fileprot, fileplg, false, false, true, false);
    plMassCenterAll(Center);
    plgInitCalc(ssplg_idx);
    plgCalculate(ssplg_idx);
    plgFinishCalc(ssplg_idx);
    GyrRad = 0.0 < GyrRadCoeff ? plGyrRadiusAll() : 0.0;

    // Setting-up quadrics.
    QSphere = gluNewQuadric();
    gluQuadricNormals(QSphere, GLU_SMOOTH);
    gluQuadricTexture(QSphere, GL_TRUE);

    LSphere = glGenLists(1);
    glNewList(LSphere, GL_COMPILE);
        gluSphere(QSphere, 1, 10, 10);
    glEndList();

    QTube = gluNewQuadric();
    gluQuadricNormals(QTube, GLU_SMOOTH);
    gluQuadricTexture(QTube, GL_TRUE);

    LTube = glGenLists(1);
    glNewList(LTube, GL_COMPILE);
        gluCylinder(QTube, 1.0, 1.0, 1.0, 10, 1);
    glEndList();

    if (preconfig.GetPreConfNum() > 0) preconfig.PreConfFirst( GetConfig());

    // ArcBall rotation initialization.
    ArcBall.setBounds(MORenderMan->RenderWidth(), MORenderMan->RenderHeight());
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
    scaling = 0;
	return true;
}

void moEffectProtein::SetupGeometry()
{
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void moEffectProtein::SetupMaterials()
{
    // This is where we actually set the specular color and intensity.
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glDisable(GL_TEXTURE_2D);

    float specularColor[4] = {1.0,1.0,1.0,1.0};
    // To change the material properties of our objects, we use glMaterialfv().
    // GL_FRONT_AND_BACK says that we want this applied to both sides of the polygons.
    // GL_SPECULAR say that we want to set the specular color.
    // specularColor is our color array, so we pass that in lastly.
    specularColor[0] = m_Config.GetParam(specular).GetValueFlt(MO_RED);
    specularColor[1] = m_Config.GetParam(specular).GetValueFlt(MO_GREEN);
    specularColor[2] = m_Config.GetParam(specular).GetValueFlt(MO_BLUE);
    specularColor[3] = m_Config.GetParam(specular).GetValueFlt(MO_ALPHA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);
    // The same thing applies as above, but this time we pass in
    // GL_SHININESS to say we want to change the intensity of the specular.
    // We then pass in an address to our shine intensity, g_Shine.
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0);
}

void moEffectProtein::SetupBlend()
{
    glEnable(GL_BLEND);
    //TRANSPARENCY [Rs * As] + [Rd * (1 -As)] = As*(Rs-Rd) + Rd
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    AlphaV = mov.movx(m_Config.GetParam(alpha), state.tempo) * state.alpha;
}

void moEffectProtein::SetupLights()
{
    glEnable(GL_LIGHTING); // Turn on lighting
    glEnable(GL_LIGHT0); // Turn on a light with defaults set
    float LightPos[3] = {0,0,0};   
    LightPos[0] = mov.movx(m_Config.GetParam(lightx),state.tempo);
    LightPos[1] = mov.movy(m_Config.GetParam(lighty),state.tempo);
    LightPos[2] = mov.movz(m_Config.GetParam(lightz),state.tempo);
    glLightfv(GL_LIGHT0,GL_POSITION,LightPos);	

    float LightDif[4] = {0.75f,0.75f,0.75f,1.0f};
    LightDif[0] = m_Config.GetParam(diffuse).GetValueFlt(MO_RED);
    LightDif[1] = m_Config.GetParam(diffuse).GetValueFlt(MO_GREEN);
    LightDif[2] = m_Config.GetParam(diffuse).GetValueFlt(MO_BLUE);
    LightDif[3] = m_Config.GetParam(diffuse).GetValueFlt(MO_ALPHA);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,LightDif);

    float LightAmb[4] = {0.1f,0.1f,0.1f,1.0f};
    LightAmb[0] = m_Config.GetParam(ambient).GetValueFlt(MO_RED);
    LightAmb[1] = m_Config.GetParam(ambient).GetValueFlt(MO_GREEN);
    LightAmb[2] = m_Config.GetParam(ambient).GetValueFlt(MO_BLUE);
    LightAmb[3] = m_Config.GetParam(ambient).GetValueFlt(MO_ALPHA);
    glLightfv(GL_LIGHT0,GL_AMBIENT,LightAmb);
}

void moEffectProtein::SetView()
{
    gluPerspective(45.0f, (GLfloat)800/(GLfloat)600, 4.0f ,4000.0f);
    gluLookAt(mov.movx(m_Config.GetParam(eyex), state.tempo),
              mov.movy(m_Config.GetParam(eyey), state.tempo),
              mov.movz(m_Config.GetParam(eyez), state.tempo) + GyrRadCoeff * GyrRad + distance_z,
              mov.movx(m_Config.GetParam(viewx), state.tempo),
              mov.movy(m_Config.GetParam(viewy), state.tempo),
              mov.movz(m_Config.GetParam(viewz), state.tempo),
              0, 1, 0);
}

void moEffectProtein::UpdateRotation()
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

void moEffectProtein::RotateView()
{
    glMultMatrixf(Transform.M);
}

void moEffectProtein::ShakeBabyShake()
{
    glTranslatef(mov.movx(m_Config.GetParam(transx), state.tempo),
                 mov.movy(m_Config.GetParam(transy), state.tempo),
                 mov.movz(m_Config.GetParam(transz), state.tempo));
    glRotatef(mov.movx(m_Config.GetParam(rotatx), state.tempo), 1.0, 0.0, 0.0);
    glRotatef(mov.movx(m_Config.GetParam(rotaty), state.tempo), 0.0, 1.0, 0.0);
    glRotatef(mov.movx(m_Config.GetParam(rotatz), state.tempo), 0.0, 0.0, 1.0);
    glScalef(mov.movx(m_Config.GetParam(scalex), state.tempo),
             mov.movx(m_Config.GetParam(scaley), state.tempo),
             mov.movx(m_Config.GetParam(scalez), state.tempo));
}

void moEffectProtein::DrawGeometry()
{
    int id, aa; 
    bool draw_ribbon;
    for (id = plFirstID(); id <= plLastID(); id++)
    {
        draw_ribbon = RibbonEnabled && (3 <= plAACountID(id));
        for (aa = plFirstAA(id); aa <= plLastAA(id); aa++)
            if (!plAAMissing(id, aa))
            {
                if (draw_ribbon) DrawRibbon(id, aa);
                if (AtomsEnabled) DrawAtoms(id, aa);
                if (BondsEnabled) DrawBonds(id, aa);
            }
    }

    if (NonProtEnabled)
    {
        int hg;
        for (hg = plFirstHGR(); hg <= plLastHGR(); hg++)
        {
            DrawAtoms(hg);
            DrawBonds(hg);
        }
    }
}

void moEffectProtein::RestoreConf()
{
    // Restoring defaults.
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);
}

void moEffectProtein::Draw( moTempo* tempogral,moEffectState* parentstate) 
{
    PreDraw(tempogral, parentstate);

    SetupGeometry();
    SetupBlend();
    SetupLights();
    SetupMaterials();

    glMatrixMode(GL_PROJECTION); // Select the projection matrix.
    glLoadIdentity();            // Reset the projection matrix.

    SetView();
    RotateView();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ShakeBabyShake();
    DrawGeometry();

	/*
    glMatrixMode(GL_PROJECTION); // Select the projection matrix.
    glPopMatrix();               // Restore original projection Matrix.
    glMatrixMode(GL_MODELVIEW);  // Select The modelview matrix.
    glPopMatrix();               // Restore original modelview matrix.

    RestoreConf();
	*/
}

void moEffectProtein::Interaction(moIODeviceManager *IODeviceManager) 
{
	//aqui se parsean los codisp
	moDeviceCode *temp,*aux;
	MOint did,cid,estado,e,valor,estaux,elaux;
	double f, d;

        isClicked = isRPressed = false;
	if (devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			estado = IODeviceManager->array[did]->GetStatus(cid);
			valor = IODeviceManager->array[did]->GetValue(cid);
			if (estado)
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
					if (scaling)
					{
					    f = MouseScaling  * SizeScaling * (valor / abs(valor));
					    if (RibbonEnabled) 
					        for (int i = HELIX; i <= TURN; i++)
						{
							d = f * RibbonWidth[i]; 
							if (0 < RibbonWidth[i] + d) RibbonWidth[i] += d;
						}
					    if (AtomsEnabled || NonProtEnabled)
					    { 
						d = f * AtomRadius;
						if (0 < AtomRadius + d) AtomRadius += d;
					    }
					    if (BondsEnabled)
					    {
						d = f * BondRadius;
						if (0 < BondRadius + d) BondRadius += d;
					    }
					}
					else distance_z -= MouseScaling * valor;
					break;
				case MO_KEY_PRESS_S:
					scaling = !scaling;
					break;
				case MO_KEY_PRESS_C:
					show_cursor = !show_cursor;
					SDL_ShowCursor(show_cursor);
					break;
				case MO_KEY_PRESS_H:
					NonProtEnabled = !NonProtEnabled;
					break;
				case MO_KEY_PRESS_R:
					RibbonEnabled = !RibbonEnabled;
					break;
				case MO_KEY_PRESS_A:
					AtomsEnabled = !AtomsEnabled;
					break;
				case MO_KEY_PRESS_B:
					BondsEnabled = !BondsEnabled;
					break;
				case MO_KEY_PRESS_1:
					RibbonStyle = 1;
					break;
				case MO_KEY_PRESS_2:
					RibbonStyle = 2;
					break;
				case MO_KEY_PRESS_3:
					RibbonStyle = 3;
					break;
				case MO_KEY_PRESS_4:
					RibbonStyle = 4;
					break;
			}
		temp = temp->next;
		}
	}
    UpdateRotation();
}

MOboolean moEffectProtein::Finish() 
{
    gluDeleteQuadric(QSphere);
    glDeleteLists(LSphere, 1);

    gluDeleteQuadric(QTube);
    glDeleteLists(LTube, 1);

	return PreFinish();
}

void moEffectProtein::DrawSpline(int n)
{
    int ui;
    double u;
    vector_3d v0, v1;

    glColor4d(RibbonColor[0], RibbonColor[1], RibbonColor[2], AlphaV);

    if (n == 0) spline_side1.Feval(0, v1); else
    if (n == 1) spline_center.Feval(0, v1); 
    else spline_side2.Feval(0, v1);

    for (ui = 1; ui <= 10; ui ++)
        if (ui % RibbonDetail == 0)
        {
            u = 0.1 * ui; 
            CopyVect(v1, v0);

            if (n == 0) spline_side1.Feval(u, v1); else
            if (n == 1) spline_center.Feval(u, v1); 
            else spline_side2.Feval(u, v1);

            glBegin(GL_LINES);
                glVertex3d(v0[0] - Center[0], v0[1] - Center[1], v0[2] - Center[2]);
                glVertex3d(v1[0] - Center[0], v1[1] - Center[1], v1[2] - Center[2]);
            glEnd();
        }
}

void moEffectProtein::DrawFlatRibbonPatch()
{
    vector_3d CentPoint0, CentPoint1;
    vector_3d Sid1Point0, Sid1Point1;
    vector_3d Sid2Point0, Sid2Point1;
    vector_3d Transversal, Tangent;
    vector_3d Normal0, Normal1;
    int ui;
    double u;

    glColor4d(RibbonColor[0], RibbonColor[1], RibbonColor[2], AlphaV);

    // The initial geometry is generated.
    spline_side1.Feval(0, Sid1Point1);
    spline_center.Feval(0, CentPoint1);
    spline_side2.Feval(0, Sid2Point1);

    // The tangents at the three previous points are the same.
    spline_side2.Deval(0, Tangent);
    // Vector transversal to the ribbon.
    LinearComb(Sid1Point1, -1, Sid2Point1, Transversal);
    // The normal is calculated.
    CrossProduct(Transversal, Tangent, Normal1);
    Normalize(Normal1);

    for (ui = 1; ui <= 10; ui ++)
        if (ui % RibbonDetail == 0)
        {
            u = 0.1 * ui;

            // The geometry of the previous iteration is saved.
            CopyVect(Sid1Point1, Sid1Point0);
            CopyVect(CentPoint1, CentPoint0);
            CopyVect(Sid2Point1, Sid2Point0);
            CopyVect(Normal1, Normal0);

            // The new geometry is generated.
            spline_side1.Feval(u, Sid1Point1);
            spline_center.Feval(u, CentPoint1);
            spline_side2.Feval(u, Sid2Point1);

            // The tangents at the three previous points are the same.
            spline_side2.Deval(u, Tangent);
            // Vector transversal to the ribbon.
            LinearComb(Sid1Point1, -1, Sid2Point1, Transversal);
            // The normal is calculated.
            CrossProduct(Transversal, Tangent, Normal1);
            Normalize(Normal1);

            glBegin(GL_QUAD_STRIP);
                // The (Sid1Point0, Sid1Point1, MiddPoint0, MiddPoint1) face is drawn.
                glNormal3d(Normal0[0], Normal0[1], Normal0[2]);
                glVertex3d(Sid1Point0[0] - Center[0],
                           Sid1Point0[1] - Center[1],
                           Sid1Point0[2] - Center[2]);
                glNormal3d(Normal1[0], Normal1[1], Normal1[2]);
                glVertex3d(Sid1Point1[0] - Center[0],
                           Sid1Point1[1] - Center[1],
                           Sid1Point1[2] - Center[2]);
                glNormal3d(Normal0[0], Normal0[1], Normal0[2]);
                glVertex3d(CentPoint0[0] - Center[0],
                           CentPoint0[1] - Center[1],
                           CentPoint0[2] - Center[2]);
                glNormal3d(Normal1[0], Normal1[1], Normal1[2]);
                glVertex3d(CentPoint1[0] - Center[0],
                           CentPoint1[1] - Center[1],
                           CentPoint1[2] - Center[2]);
                // (MiddPoint0, MiddPoint1, Sid2Point0, Sid2Point1) plane is drawn.
                glNormal3d(Normal0[0], Normal0[1], Normal0[2]);
                glVertex3d(Sid2Point0[0] - Center[0],
                           Sid2Point0[1] - Center[1],
                           Sid2Point0[2] - Center[2]);
                glNormal3d(Normal1[0], Normal1[1], Normal1[2]);
                glVertex3d(Sid2Point1[0] - Center[0],
                           Sid2Point1[1] - Center[1],
                           Sid2Point1[2] - Center[2]);
            glEnd();
        }
}

// Draws an ellipsoidal section of ribbon corresponding to the current control
// points. If HeightFactor = 1 then the ellipsoid will be a circunference.
void moEffectProtein::DrawSolidRibbonSection(double HeightFactor)
{
    vector_3d CentPoint0, CentPoint1;
    vector_3d Sid1Point0, Sid1Point1;
    vector_3d Tangent0, Tangent1;
    vector_3d Transversal0, Transversal1;

    double Width0, Width1;
    double Height0, Height1;
    int ui;
    double u; 
    int n;

    vector_3d PointBk0, PointFt0, PointBk1, PointFt1;
    vector_3d NormalBk, NormalFt;
    vector_3d FaceNormalBk0, FaceNormalFt0, FaceNormalBk1, FaceNormalFt1;
    vector_3d t;
    double a, s, c;
    double x0, x1, y0, y1, s0, s1;

    glColor4d(RibbonColor[0], RibbonColor[1], RibbonColor[2], AlphaV);

    // The initial geometry is generated.
    spline_side1.Feval(0, Sid1Point1);
    spline_center.Feval(0, CentPoint1);

    // The tangent indicates the direction of the ribbon.
    spline_center.Deval(0, Tangent1);
    Normalize(Tangent1);

    // This vector is transversal to the ribbon (and perpendicular to Tangent1).
    LinearComb(Sid1Point1, -1, CentPoint1, Transversal1);
    Width1 = Normalize(Transversal1);
    Height1 = HeightFactor * Width1;

    // Slices loop.
    for (ui = 1; ui <= 10; ui ++)
        if (ui % RibbonDetail == 0)
        {
            u = 0.1 * ui;

            // The geometry of the previous iteration is saved.
            CopyVect(Sid1Point1, Sid1Point0);
            CopyVect(CentPoint1, CentPoint0);
            CopyVect(Tangent1, Tangent0);
            CopyVect(Transversal1, Transversal0);
            Width0 = Width1;
            Height0 = Height1;

            // The new geometry is generated.
            spline_side1.Feval(u, Sid1Point1);
            spline_center.Feval(u, CentPoint1);

            // The tangent indicates the direction of the ribbon.
            spline_center.Deval(u, Tangent1);
            Normalize(Tangent1);

            // This vector is transversal to the ribbon (and perpendicular to
            // Tangent1).
            LinearComb(Sid1Point1, -1, CentPoint1, Transversal1);
            Width1 = Normalize(Transversal1);
            Height1 = HeightFactor * Width1;

            // The "0" and "1" variables are ready, containig the two ellipsoids that
            // define the slide.
            // Now we draw the ribbon slice, joining the initial (denoted by 0) and the final (1)
            // ellipsoids.

            // We are going to move around the slice, adding faces as we advance. With
            // the postfix "Bk" (back) we denote the points that lie in the initial
            // ellipsid and with "Ft" (Front) the points on the final ellipsoid. Inside
            // this procedure "0" means the previous geometry in the faces loop.

            // These are the vectors that define the "canonical Y axis" for the back and
            // front ellipsoids: they are contained in the planes determined by
            // (Transversal0, NormalBk) and (Transversal1, NormalFt), respectively.
            CrossProduct(Transversal0, Tangent0, NormalBk); Normalize(NormalBk);
            CrossProduct(Transversal1, Tangent1, NormalFt); Normalize(NormalFt);

            LinearComb(CentPoint0, Width0, Transversal0, PointBk1);

            // t is the tangent to the back ellipsoid at the angle 0.
            ScalarProduct(Height0, NormalBk, t);
            CrossProduct(Tangent0, t, FaceNormalBk1);
            Normalize(FaceNormalBk1);

            LinearComb(CentPoint1, Width1, Transversal1, PointFt1);

            // t is the tangent to the front ellipsoid at the angle 0.
            ScalarProduct(Height1, NormalFt, t);
            CrossProduct(Tangent1, t, FaceNormalFt1);
            Normalize(FaceNormalFt1);

            // Faces loop.             
            for (n = 1; n <= NRDivisions; n++)
            {
                CopyVect(PointBk1, PointBk0);
                CopyVect(PointFt1, PointFt0);
                CopyVect(FaceNormalBk1, FaceNormalBk0);
                CopyVect(FaceNormalFt1, FaceNormalFt0);

                a = 2 * PI * (double(n) / double(NRDivisions));
                s = sin(a);
                c = cos(a);

                x0 = Width0 * c;
                y0 = Height0 * s;

                x1 = Width1 * c;
                y1 = Height1 * s;

                LinearComb(x0, Transversal0, y0, NormalBk, t);
                LinearComb(CentPoint0, +1, t, PointBk1);

                // t is the tangent to the back ellipsoid at the angle a.
                s0 = -Width0 * s;
                s1 = Height0 * c;
                LinearComb(s0, Transversal0, s1, NormalBk, t);
                CrossProduct(Tangent0, t, FaceNormalBk1);
                Normalize(FaceNormalBk1);

                LinearComb(x1, Transversal1, y1, NormalFt, t);
                LinearComb(CentPoint1, +1, t, PointFt1);

                // t is the tangent to the front ellipsoid at the angle a.
                s0 = -Width1 * s;
                s1 = Height1 * c;
                LinearComb(s0, Transversal1, s1, NormalFt, t);
                CrossProduct(Tangent1, t, FaceNormalFt1);
                Normalize(FaceNormalFt1);

                glBegin(GL_QUADS);
                    // The (PointFt0, PointBk0, PointBk1, PointFt1) face is drawn.
                    glNormal3d(FaceNormalFt0[0], FaceNormalFt0[1], FaceNormalFt0[2]);
                    glVertex3d(PointFt0[0] - Center[0],
                               PointFt0[1] - Center[1],
                               PointFt0[2] - Center[2]);
                    glNormal3d(FaceNormalBk0[0], FaceNormalBk0[1], FaceNormalBk0[2]);
                    glVertex3d(PointBk0[0] - Center[0],
                               PointBk0[1] - Center[1],
                               PointBk0[2] - Center[2]);
                    glNormal3d(FaceNormalBk1[0], FaceNormalBk1[1], FaceNormalBk1[2]);
                    glVertex3d(PointBk1[0] - Center[0],
                               PointBk1[1] - Center[1],
                               PointBk1[2] - Center[2]);
                    glNormal3d(FaceNormalFt1[0], FaceNormalFt1[1], FaceNormalFt1[2]);
                    glVertex3d(PointFt1[0] - Center[0],
                               PointFt1[1] - Center[1],
                               PointFt1[2] - Center[2]);
                glEnd();
            }
        }
}

void moEffectProtein::SetRibbonColor(int id, int aa)
{
    int ss, i; 
    if (UseSecStrColors)
    {
        ss = plGetSecStr(id, aa);
        for (i = 0; i < 3; i++) RibbonColor[i] = SecStrColors[ss][i];
    }
    else
        for (i = 0; i < 3; i++) RibbonColor[i] = DefColor[i]; 
}

void moEffectProtein::DrawRibbon(int id, int aa)
{
    SetRibbonColor(id, aa);
    ConstructControlPoints(id, aa);
    switch (RibbonStyle)
    {
        case 1: 
            DrawLineRibbon();
            break;
        case 2: 
            DrawFlatRibbon();
            break; 
        case 3: 
            DrawStrandRibbon();
            break;
        case 4: 
            DrawTubeRibbon();
            break;
    }
}

void moEffectProtein::DrawLineRibbon()
{
    DrawSpline(0);
    DrawSpline(1);
    DrawSpline(2);
}

void moEffectProtein::DrawFlatRibbon()
{
    DrawFlatRibbonPatch();
}

void moEffectProtein::DrawStrandRibbon()
{
    DrawSolidRibbonSection(0.3);
}

void moEffectProtein::DrawTubeRibbon()
{
    DrawSolidRibbonSection(1);
}

/******************************************************************************
The code in the following three functions was based in the theory presented in
the following article:
"Algorithm for ribbon models of proteins."
Authors: Mike Carson and Charles E. Bugg
         University of Alabama at Birmingham, Comprehensive Cancer Center
         252 BHS, THT 79, University Station, Birmingham, AL 35294, USA
Published in: J.Mol.Graphics 4, pp. 121-122 (1986)
******************************************************************************/

// Shifts the control points one place to the left.
void moEffectProtein::ShiftControlPoints()
{
     spline_side1.ShiftBSplineCPoints();
     spline_center.ShiftBSplineCPoints();
     spline_side2.ShiftBSplineCPoints();
}

// Adds a new control point to the arrays CPCenter, CPRight and CPLeft
void moEffectProtein::AddControlPoints(int id, int aa)
{
    vector_3d ca0, ca1, ox0, A, B, C, D, p0;
    double w;
    int f, ss, at_ca0, at_ox0, at_ca1;

    at_ca0 = plAT(id, aa, MC_CA);
    at_ox0 = plAT(id, aa, MC_O);
    at_ca1 = plAT(id, aa + 1, MC_CA);

    if ((at_ca0 == -1) || (at_ox0 == -1) || (at_ca1 == -1)) return;

    plAtomPos(id, aa, at_ca0, ca0);
    plAtomPos(id, aa, at_ox0, ox0);
    plAtomPos(id, aa + 1, at_ca1, ca1);

    LinearComb(ca1, -1, ca0, A);
    LinearComb(ox0, -1, ca0, B);

    // Vector normal to the peptide plane (pointing outside in the case of the
    // alpha helix).
    CrossProduct(A, B, C);

    // Vector contained in the peptide plane (perpendicular to its direction).
    CrossProduct(C, A, D);

    // Normalizing vectors.
    Normalize(C);
    Normalize(D);

    ss = plGetSecStr(id, aa);
    w = RibbonWidth[ss];

    // Flipping test (to avoid self crossing in the strands).
    if ((ss != HELIX) && (90.0 < VectAngle(FlipTestV, D)))
        // Flip detected. The plane vector is inverted.
        Scale(-1.0, D);

    // The central control point is constructed.
    LinearComb(0.5, ca0, 0.5, ca1, spline_center.BSplineCPoints[3]);

    if (ss == HELIX)
    {
        // When residue i is contained in a helix, the control point is moved away
        // from the helix axis, along the C direction. 
        f = plHandness(id, aa);
        CopyVect(spline_center.BSplineCPoints[3], p0);
        LinearComb(p0, f * HelixWidth, C, spline_center.BSplineCPoints[3]);
    }

    // The control points for the side ribbons are constructed.
    LinearComb(spline_center.BSplineCPoints[3], +w, D, spline_side1.BSplineCPoints[3]);
    LinearComb(spline_center.BSplineCPoints[3], -w, D, spline_side2.BSplineCPoints[3]);

    // Saving the plane vector (for the flipping test in the next call).
    CopyVect(D, FlipTestV);
}

void moEffectProtein::ConstructControlPoints(int id, int aa)
{
    int ss;
    if (plFirstAA(id) == aa)
    {
        // The control points 2 and 3 are created.
        FlipTestV[X] = FlipTestV[Y] = FlipTestV[Z] = 0.0;
        AddControlPoints(id, aa);

        CopyVect(spline_side1.BSplineCPoints[3], spline_side1.BSplineCPoints[2]);
        CopyVect(spline_center.BSplineCPoints[3], spline_center.BSplineCPoints[2]);
        CopyVect(spline_side2.BSplineCPoints[3], spline_side2.BSplineCPoints[2]);

        if (plAAMissing(id, aa + 1) || plAAMissing(id, aa + 2)) return;

        AddControlPoints(id, aa + 1);

        ss = plGetSecStr(id, aa + 1);

        // We still need the two first control points.
        // Moving backwards along the cp_center[2] - cp_center[3] direction.
        LinearComb(2.0, spline_center.BSplineCPoints[2], -1, spline_center.BSplineCPoints[3], spline_center.BSplineCPoints[1]);
        LinearComb(spline_center.BSplineCPoints[1], +RibbonWidth[ss], FlipTestV, spline_side1.BSplineCPoints[1]);
        LinearComb(spline_center.BSplineCPoints[1], -RibbonWidth[ss], FlipTestV, spline_side2.BSplineCPoints[1]);

        LinearComb(2.0, spline_center.BSplineCPoints[1], -1, spline_center.BSplineCPoints[2], spline_center.BSplineCPoints[0]);
        LinearComb(spline_center.BSplineCPoints[0], +RibbonWidth[ss], FlipTestV, spline_side1.BSplineCPoints[0]);
        LinearComb(spline_center.BSplineCPoints[0], -RibbonWidth[ss], FlipTestV, spline_side2.BSplineCPoints[0]);
    } 
    else
    {
        ShiftControlPoints();
        if ((plLastAA(id) == aa) || (plLastAA(id) == aa + 1))
        { 
            ss = plGetSecStr(id, aa);

            // Moving forward along the cp_center[1] - cp_center[2] direction.
            LinearComb(2.0, spline_center.BSplineCPoints[2], -1, spline_center.BSplineCPoints[1], spline_center.BSplineCPoints[3]);
            LinearComb(spline_center.BSplineCPoints[3], +RibbonWidth[ss], FlipTestV, spline_side1.BSplineCPoints[3]);
            LinearComb(spline_center.BSplineCPoints[3], -RibbonWidth[ss], FlipTestV, spline_side2.BSplineCPoints[3]);
        }  
        else 
        {
            if (plAAMissing(id, aa + 1) || plAAMissing(id, aa + 2)) return;
            AddControlPoints(id, aa + 1);
        }
    }
    spline_side1.UpdateMatrix3();
    spline_center.UpdateMatrix3();
    spline_side2.UpdateMatrix3();
}

void moEffectProtein::DrawSphere(const vector_3d &p0, double r, const vector_3d &color)
{
    glPushMatrix();

    glColor4d(color[0], color[1], color[2], AlphaV);

    glTranslated(p0[0] - Center[0], p0[1] - Center[1], p0[2] - Center[2]);

    glEnable(GL_NORMALIZE); // To normalize the normals after the scaling.
    glScaled(r, r, r);
    glCallList(LSphere);
    glDisable(GL_NORMALIZE);

    glPopMatrix();
}

void moEffectProtein::DrawTube(const vector_3d &v, const vector_3d &p0, double l, double r, const vector_3d &color)
{
    double aX, aY;

    glPushMatrix();

    glColor4d(color[0], color[1], color[2], AlphaV);

    glTranslated(p0[0] - Center[0], p0[1] - Center[1], p0[2] - Center[2]);

    GiveYZAngles(v, aY, aX);
    glRotated(aY, 0.0, 1.0, 0.0);
    glRotated(aX, 1.0, 0.0, 0.0);

    glEnable(GL_NORMALIZE); // To normalize the normals after the scaling.
    glScaled(r, r, l);
    glCallList(LTube);
    glDisable(GL_NORMALIZE);

    glPopMatrix();
}

int moEffectProtein::AtomIndex(const char* name)
{
    int l = strlen(name);
    if (0 < l)
    {
        if (m_Name == AS_STR) return 6;
        if (m_Name == FE_STR) return 7;

        char c = name[0];
        bool digit = (c == '0') || (c == '1') || (c == '2') || (c == '3') ||
                     (c == '4') || (c == '5') || (c == '6') || (c == '7') ||
                     (c == '8') || (c == '9');
        if (digit && (1 < l)) c = name[1];
        if (c == 'N') return 0;
        if (c == 'C') return 1;
        if (c == 'H') return 2;
        if (c == 'O') return 3;
        if (c == 'S') return 4;
        if (c == 'P') return 5;
        return 8;
    }
    else return 8;
}

void moEffectProtein::SetAtomColor(int idx)
{
    CopyVect(AtomColors[idx], AtomColor);
}

void moEffectProtein::DrawAtoms(int id, int aa)
{
    int at, idx;
    double r;
    vector_3d atom;
    for (at = plFirstAT(id, aa); at <= plLastAT(id, aa); at++)
        if (!plATMissing(id, aa, at))
        {
            plAtomPos(id, aa, at, atom);
            idx = AtomIndex(plAtomName(id, aa, at));
            SetAtomColor(idx);

            if (UseVDWradius) r = VDWradius[idx];
            else r = AtomRadius;
            DrawSphere(atom, r, AtomColor);
        }
}

void moEffectProtein::DrawAtoms(int hg)
{
    int at, idx;
    double r;
    vector_3d atom;
    for (at = plFirstHAT(hg); at <= plLastHAT(hg); at++)
    {
        plHAtomPos(hg, at, atom);
        idx = AtomIndex(plHAtomName(hg, at));
        SetAtomColor(idx);

        if (UseVDWradius) r = VDWradius[idx];
        else r = AtomRadius;
        DrawSphere(atom, r, AtomColor);
    }
}

void moEffectProtein::DrawBonds(int id, int aa)
{
    int bd;
    vector_3d bond, atom0;
    for (bd = plFirstBD(id, aa); bd <= plLastBD(id, aa); bd++)
    {
        plBondVect(id, aa, bd, bond);
        plBondAtomPos(id, aa, bd, ATOM_, atom0);
        DrawTube(bond, atom0, plBondLength(id, aa, bd), BondRadius, BondColor);
    }
}

void moEffectProtein::DrawBonds(int hg)
{
    int bd;
    vector_3d bond, atom0;
    for (bd = plFirstHBD(hg); bd <= plLastHBD(hg); bd++)
    {
        plHBondVect(hg, bd, bond);
        plHBondAtomPos(hg, bd, ATOM_, atom0);
        DrawTube(bond, atom0, plHBondLength(hg, bd), BondRadius, BondColor);
    }
}
