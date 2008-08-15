/*******************************************************************************

                                moEffectIcon3D.cpp

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
  Andr� Colubri

*******************************************************************************/

#include "moEffectIcon3D.h"

//========================
//  Factory
//========================

moEffectIcon3DFactory *m_EffectIcon3DFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectIcon3DFactory==NULL)
		m_EffectIcon3DFactory = new moEffectIcon3DFactory();
	return(moEffectFactory*) m_EffectIcon3DFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectIcon3DFactory;
	m_EffectIcon3DFactory = NULL;
}

moEffect*  moEffectIcon3DFactory::Create() {
	return new moEffectIcon3D();
}

void moEffectIcon3DFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectIcon3D::moEffectIcon3D() {
	SetName("icon3d");
}

moEffectIcon3D::~moEffectIcon3D() {
	Finish();
}

MOboolean moEffectIcon3D::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( ICON3D_ALPHA, moText("alpha") );
	moDefineParamIndex( ICON3D_COLOR, moText("color") );
	moDefineParamIndex( ICON3D_SYNC, moText("syncro") );
	moDefineParamIndex( ICON3D_PHASE, moText("phase") );
	moDefineParamIndex( ICON3D_TEXTURE, moText("texture") );
	moDefineParamIndex( ICON3D_POLYGONMODE, moText("polygonmode") );
	moDefineParamIndex( ICON3D_BLENDING, moText("blending") );
	moDefineParamIndex( ICON3D_OBJECT, moText("object") );
	moDefineParamIndex( ICON3D_SPECULAR, moText("specular") );
	moDefineParamIndex( ICON3D_DIFFUSE, moText("diffuse") );
	moDefineParamIndex( ICON3D_AMBIENT, moText("ambient") );
	moDefineParamIndex( ICON3D_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( ICON3D_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( ICON3D_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( ICON3D_ROTATEX, moText("rotatex") );
	moDefineParamIndex( ICON3D_ROTATEY, moText("rotatey") );
	moDefineParamIndex( ICON3D_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( ICON3D_SCALEX, moText("scalex") );
	moDefineParamIndex( ICON3D_SCALEY, moText("scaley") );
	moDefineParamIndex( ICON3D_SCALEZ, moText("scalez") );
	moDefineParamIndex( ICON3D_EYEX, moText("eyex") );
	moDefineParamIndex( ICON3D_EYEY, moText("eyey") );
	moDefineParamIndex( ICON3D_EYEZ, moText("eyez") );
	moDefineParamIndex( ICON3D_VIEWX, moText("viewx") );
	moDefineParamIndex( ICON3D_VIEWY, moText("viewy") );
	moDefineParamIndex( ICON3D_VIEWZ, moText("viewz") );
	moDefineParamIndex( ICON3D_LIGHTX, moText("lightx") );
	moDefineParamIndex( ICON3D_LIGHTY, moText("lighty") );
	moDefineParamIndex( ICON3D_LIGHTZ, moText("lightz") );
	moDefineParamIndex( ICON3D_INLET, moText("inlet") );
	moDefineParamIndex( ICON3D_OUTLET, moText("outlet") );

	Tx = 1; Ty = 1; Tz = 1;
	Sx = 1; Sy = 1; Sz = 1;

	g_ViewMode = GL_TRIANGLES;

	//inicializacion de los objetos y materiales
	Models3d.MODebug = MODebug;
	Models3d.Init( GetConfig(), m_Config.GetParamIndex(moText("object")), m_pResourceManager->GetModelMan() );

	return true;
}

void moEffectIcon3D::Draw( moTempo* tempogral,moEffectState* parentstate)
{

    PreDraw( tempogral, parentstate);

    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View

/*
    // Cambiar la proyeccion para una vista ortogonal //
	//glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	//glEnable(GL_ALPHA_TEST);
	//glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glEnable(GL_SMOOTH);
	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	float specularColor[4] = {1.0,1.0,1.0,1.0};
	float LightPos[3] = {0,30,0};
	float LightDif[4] = {1.0f,1.0f,1.0f,1.0f};
	float LightAmb[4] = {1.0f,1.0f,1.0f,1.0f};
	float g_Shine = 50.0;

	// This is where we actually set the specular color and intensity.

	// To change the material properties of our objects, we use glMaterialfv().
	// GL_FRONT_AND_BACK says that we want this applied to both sides of the polygons.
	// GL_SPECULAR say that we want to set the specular color.
	// specularColor is our color array, so we pass that in lastly.
	specularColor[0] = m_Config[moR(ICON3D_SPECULAR)][MO_SELECTED][MO_RED].Fun()->Eval(state.tempo.ang);
	specularColor[1] = m_Config[moR(ICON3D_SPECULAR)][MO_SELECTED][MO_GREEN].Fun()->Eval(state.tempo.ang);
	specularColor[2] = m_Config[moR(ICON3D_SPECULAR)][MO_SELECTED][MO_BLUE].Fun()->Eval(state.tempo.ang);
	specularColor[3] = m_Config[moR(ICON3D_SPECULAR)][MO_SELECTED][MO_ALPHA].Fun()->Eval(state.tempo.ang);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);


	// The same thing applies as above, but this time we pass in
	// GL_SHININESS to say we want to change the intensity of the specular.
	// We then pass in an address to our shine intensity, g_Shine.
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80.0);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	//glLightiv(GL_LIGHT0,GL_POSITION,LightPos);

// Give OpenGL our position,	then view,		then up vector
	gluPerspective(45.0f,(float)m_pResourceManager->GetRenderMan()->ScreenWidth() / (float)m_pResourceManager->GetRenderMan()->ScreenHeight(), 4.0f ,4000.0f);
	//m_pResourceManager->GetGLMan()->SetPerspectiveView(m_pResourceManager->GetRenderMan()->ScreenWidth(),m_pResourceManager->GetRenderMan()->ScreenHeight());

	gluLookAt(		m_Config[moR(ICON3D_EYEX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(ICON3D_EYEY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(ICON3D_EYEZ)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(ICON3D_VIEWX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(ICON3D_VIEWY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(ICON3D_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang),
					0, 1, 0);

	LightPos[0] = m_Config[moR(ICON3D_LIGHTX)].GetData()->Fun()->Eval(state.tempo.ang);
	LightPos[1] = m_Config[moR(ICON3D_LIGHTY)].GetData()->Fun()->Eval(state.tempo.ang);
	LightPos[2] = m_Config[moR(ICON3D_LIGHTZ)].GetData()->Fun()->Eval(state.tempo.ang);
	glLightfv(GL_LIGHT0,GL_POSITION,LightPos);


	LightDif[0] = m_Config[moR(ICON3D_DIFFUSE)][MO_SELECTED][MO_RED].Fun()->Eval(state.tempo.ang);
	LightDif[1] = m_Config[moR(ICON3D_DIFFUSE)][MO_SELECTED][MO_GREEN].Fun()->Eval(state.tempo.ang);
	LightDif[2] = m_Config[moR(ICON3D_DIFFUSE)][MO_SELECTED][MO_BLUE].Fun()->Eval(state.tempo.ang);
	LightDif[3] = m_Config[moR(ICON3D_DIFFUSE)][MO_SELECTED][MO_ALPHA].Fun()->Eval(state.tempo.ang);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,LightDif);

	LightAmb[0] = m_Config[moR(ICON3D_AMBIENT)][MO_SELECTED][MO_RED].Fun()->Eval(state.tempo.ang);
	LightAmb[1] = m_Config[moR(ICON3D_AMBIENT)][MO_SELECTED][MO_GREEN].Fun()->Eval(state.tempo.ang);
	LightAmb[2] = m_Config[moR(ICON3D_AMBIENT)][MO_SELECTED][MO_BLUE].Fun()->Eval(state.tempo.ang);
	LightAmb[3] = m_Config[moR(ICON3D_AMBIENT)][MO_SELECTED][MO_ALPHA].Fun()->Eval(state.tempo.ang);
	glLightfv(GL_LIGHT0,GL_AMBIENT,LightAmb);


*/
    // Draw //
	//BEGIN
	//glBindTexture(GL_TEXTURE_2D, Images.Get(Image,&state.tempo));
                  // Select The Modelview Matrix

	SetPolygonMode( (moPolygonModes)m_Config[moR(ICON3D_POLYGONMODE)].GetValue().GetSubValue(0).Int());
	//SetBlending( );

	glTranslatef(   m_Config[moR(ICON3D_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(ICON3D_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(ICON3D_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));

	glRotatef(  m_Config[moR(ICON3D_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(ICON3D_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(ICON3D_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );

	glScalef(   m_Config[moR(ICON3D_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(ICON3D_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(ICON3D_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang));



	//glEnable(GL_NORMALIZE);
/*
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// Set The Texture Generation Mode For S To Sphere Mapping( NEW )
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);						// Enable Texture Coord Generation For S( NEW )
	glEnable(GL_TEXTURE_GEN_T);
	*/

	Models3d.Draw( m_Config[ moParamReference(ICON3D_OBJECT) ].GetIndexValue(), &state, GL_TRIANGLES);

/*
	//END
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_CULL_FACE);
	glDisable(GL_COLOR_MATERIAL);						// Allow color
	glDisable(GL_LIGHTING);								// Turn on lighting
	glDisable(GL_LIGHT0);								// Turn on a light with defaults set
	glDisable(GL_DEPTH_TEST);

    // Dejamos todo como lo encontramos //
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
*/
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

MOboolean moEffectIcon3D::Finish()
{
	Models3d.Finish();

    return PreFinish();
}

void moEffectIcon3D::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp,*aux;
	MOint did,cid,state,e,valor,estaux,elaux;
	moText txt;

	if (devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);
			if (state)
			switch(i) {
				case MO_ICON3D_TRANSLATE_X:
					Tx = valor;
					txt = FloatToStr(Tx);
					MODebug->Push(moText("Tx: ") + (moText)txt);
					break;
				case MO_ICON3D_TRANSLATE_Y:
					Ty = valor;
					txt = FloatToStr(Ty);
					MODebug->Push(moText("Ty: ") + (moText)txt);
					break;
				case MO_ICON3D_TRANSLATE_Z:
					Tz = valor;
					txt = FloatToStr(Tz);
					MODebug->Push(moText("Tz: ") + (moText)txt);
					break;
				case MO_ICON3D_SCALE_X:
					Sx+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sx);
					MODebug->Push(moText("Sx: ") + (moText)txt);
					break;
				case MO_ICON3D_SCALE_Y:
					Sy+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sy);
					MODebug->Push(moText("Sy: ") + (moText)txt);
				case MO_ICON3D_SCALE_Z:
					Sz+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sz);
					MODebug->Push(moText("Sz: ") + (moText)txt);
					break;
			}
		temp = temp->next;
		}
	}

}


moConfigDefinition *
moEffectIcon3D::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("specular"), MO_PARAM_COLOR, ICON3D_SPECULAR );
	p_configdefinition->Add( moText("diffuse"), MO_PARAM_COLOR, ICON3D_DIFFUSE );
	p_configdefinition->Add( moText("ambient"), MO_PARAM_COLOR, ICON3D_AMBIENT );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, ICON3D_TEXTURE, moValue( "iconos/estrellas/star.tga", "TXT") );
	p_configdefinition->Add( moText("polygonmode"), MO_PARAM_POLYGONMODE, ICON3D_POLYGONMODE, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, ICON3D_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("object"), MO_PARAM_FUNCTION, ICON3D_OBJECT, moValue( "objetos/iconos3d/moldeologo.3ds", "TXT") );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_FUNCTION, ICON3D_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_FUNCTION, ICON3D_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_FUNCTION, ICON3D_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_FUNCTION, ICON3D_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_FUNCTION, ICON3D_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_FUNCTION, ICON3D_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_FUNCTION, ICON3D_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_FUNCTION, ICON3D_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_FUNCTION, ICON3D_SCALEZ );
	p_configdefinition->Add( moText("eyex"), MO_PARAM_FUNCTION, ICON3D_EYEX );
	p_configdefinition->Add( moText("eyey"), MO_PARAM_FUNCTION, ICON3D_EYEY );
	p_configdefinition->Add( moText("eyez"), MO_PARAM_FUNCTION, ICON3D_EYEZ );
	p_configdefinition->Add( moText("viewx"), MO_PARAM_FUNCTION, ICON3D_VIEWX );
	p_configdefinition->Add( moText("viewy"), MO_PARAM_FUNCTION, ICON3D_VIEWY );
	p_configdefinition->Add( moText("viewz"), MO_PARAM_FUNCTION, ICON3D_VIEWZ );
	p_configdefinition->Add( moText("lightx"), MO_PARAM_FUNCTION, ICON3D_LIGHTX );
	p_configdefinition->Add( moText("lighty"), MO_PARAM_FUNCTION, ICON3D_LIGHTY );
	p_configdefinition->Add( moText("lightz"), MO_PARAM_FUNCTION, ICON3D_LIGHTZ );
	return p_configdefinition;
}
