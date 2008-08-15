/*******************************************************************************

                               moEffectDraw.cpp

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

#include "moEffectDraw.h"

//========================
//  Factory
//========================

moEffectDrawFactory *m_EffectDrawoFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if (m_EffectDrawoFactory==NULL)
		m_EffectDrawoFactory = new moEffectDrawFactory();
	return (moEffectFactory*) m_EffectDrawoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectDrawoFactory;
	m_EffectDrawoFactory = NULL;
}

moEffect*  moEffectDrawFactory::Create() {
	return new moEffectDraw();
}

void moEffectDrawFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectDraw::moEffectDraw() {

	SetName("draw");

}

moEffectDraw::~moEffectDraw() {
	Finish();
}

MOboolean moEffectDraw::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( DRAW_ALPHA, moText("alpha") );
	moDefineParamIndex( DRAW_COLOR, moText("color") );
	moDefineParamIndex( DRAW_SYNC, moText("syncro") );
	moDefineParamIndex( DRAW_PHASE, moText("phase") );
	moDefineParamIndex( DRAW_TEXTURE, moText("texture") );
	moDefineParamIndex( DRAW_SCRIPT, moText("script") );
	moDefineParamIndex( DRAW_BLENDING, moText("blending") );
	moDefineParamIndex( DRAW_WIDTH, moText("width") );
	moDefineParamIndex( DRAW_HEIGHT, moText("height") );
	moDefineParamIndex( DRAW_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( DRAW_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( DRAW_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( DRAW_ROTATEX, moText("rotatex") );
	moDefineParamIndex( DRAW_ROTATEY, moText("rotatey") );
	moDefineParamIndex( DRAW_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( DRAW_SCALEX, moText("scalex") );
	moDefineParamIndex( DRAW_SCALEY, moText("scaley") );
	moDefineParamIndex( DRAW_SCALEZ, moText("scalez") );
	moDefineParamIndex( DRAW_INLET, moText("inlet") );
	moDefineParamIndex( DRAW_OUTLET, moText("outlet") );

    Tx = Ty = Tz = Rx = Ry = Rz = 0.0;
	Sx = Sy = Sz = 1.0;

	m_MovieScriptFN = m_Config[moParamReference(DRAW_SCRIPT)][MO_SELECTED][0].Text();
	if ((moText)m_MovieScriptFN!=moText("")) {

	    moScript::InitScript();
        RegisterFunctions();

        m_MovieScriptFN = m_pResourceManager->GetDataMan()->GetDataPath()+ moText("/") + (moText)m_MovieScriptFN;

        if ( CompileFile(m_MovieScriptFN) ) {

            SelectScriptFunction( "Init" );
//            AddFunctionParam( m_FramesPerSecond );
//            AddFunctionParam( (int)m_FramesLength );
 //           AddFunctionParam( m_pResourceManager->GetRenderMan()->RenderWidth() );
 //           AddFunctionParam( m_pResourceManager->GetRenderMan()->RenderHeight() );
            RunSelectedFunction();

        } else {

        }
	}


	return true;
}

void moEffectDraw::Draw( moTempo* tempogral, moEffectState* parentstate )
{
    MOint indeximage;
	MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    PreDraw( tempogral, parentstate);


    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
	glLoadIdentity();									// Reset The View

    // Cambiar la proyeccion para una vista ortogonal //
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,w,0,h,-1,1);                              // Set Up An Ortho Screen


    // Funcion de blending y de alpha channel //
    glEnable(GL_BLEND);

	glDisable(GL_ALPHA);

    // Draw //
	glTranslatef(  ( m_Config[moR(DRAW_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang)+Tx )*w,
                   ( m_Config[moR(DRAW_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang)+Ty )*h,
					m_Config[moR(DRAW_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang)+Tz);

	glRotatef(  m_Config[moR(DRAW_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(DRAW_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(DRAW_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
	glScalef(   m_Config[moR(DRAW_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang)*Sx,
                m_Config[moR(DRAW_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang)*Sy,
                m_Config[moR(DRAW_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang)*Sz);

    SetColor( m_Config[moR(DRAW_COLOR)][MO_SELECTED], m_Config[moR(DRAW_ALPHA)][MO_SELECTED], state );

    SetBlending( (moBlendingModes) m_Config[moR(DRAW_BLENDING)][MO_SELECTED][0].Int() );

    moTexture* pImage = (moTexture*) m_Config[moR(DRAW_TEXTURE)].GetData()->Pointer();

    glBindTexture( GL_TEXTURE_2D, m_Config[moR(DRAW_TEXTURE)].GetData()->GetGLId(&state.tempo) );

    PosTextX0 = 0.0;
	PosTextX1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordS() :  1.0 );
    PosTextY0 = 0.0;
    PosTextY1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordT() :  1.0 );

	ancho = (int)m_Config[ moR(DRAW_WIDTH) ].GetData()->Fun()->Eval(state.tempo.ang)* (float)(w/800.0);
	alto = (int)m_Config[ moR(DRAW_HEIGHT) ].GetData()->Fun()->Eval(state.tempo.ang)* (float)(h/600.0);

	glBegin(GL_QUADS);
		glTexCoord2f( PosTextX0, PosTextY1);
		glVertex2i( -ancho, -alto);

		glTexCoord2f( PosTextX1, PosTextY1);
		glVertex2i(  ancho, -alto);

		glTexCoord2f( PosTextX1, PosTextY0);
		glVertex2i(  ancho,  alto);

		glTexCoord2f( PosTextX0, PosTextY0);
		glVertex2i( -ancho,  alto);
	glEnd();
}

MOboolean moEffectDraw::Finish()
{
    return PreFinish();
}

void moEffectDraw::Interaction( moIODeviceManager *IODeviceManager ) {

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
				case MO_DRAW_TRANSLATE_X:
					Tx = valor;
					MODebug->Push(IntToStr(valor));
					break;
				case MO_DRAW_TRANSLATE_Y:
					Ty = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
					MODebug->Push(IntToStr(valor));
					break;
				case MO_DRAW_SCALE_X:
					Sx+=((float) valor / (float) 256.0);
					MODebug->Push(IntToStr(valor));
					break;
				case MO_DRAW_SCALE_Y:
					Sy+=((float) valor / (float) 256.0);
					MODebug->Push(IntToStr(valor));
					break;
			}
		temp = temp->next;
		}
	}

}

moConfigDefinition *
moEffectDraw::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, DRAW_TEXTURE, moValue( "iconos/estrellas/star.tga", "TXT") );
    p_configdefinition->Add( moText("script"), MO_PARAM_SCRIPT, DRAW_SCRIPT );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, DRAW_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, DRAW_WIDTH, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, DRAW_HEIGHT, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, DRAW_TRANSLATEX, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, DRAW_TRANSLATEY, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, DRAW_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, DRAW_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, DRAW_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, DRAW_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, DRAW_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, DRAW_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, DRAW_SCALEZ );
	return p_configdefinition;
}


int moEffectDraw::ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber)
{
    switch (iFunctionNumber - m_iMethodBase)
    {
        case 0:
            return GetFeature(vm);
        case 1:
            return GetTicks(vm);
        case 2:
            return PushDebugString(vm);
        case 3:
            return triangle(vm);
        case 4:
            return line(vm);
        case 5:
            return arc(vm);
        case 6:
            return point(vm);
        case 7:
            return quad(vm);
        case 8:
            return ellipse(vm);
        case 9:
            return rect(vm);
        case 10:
            return strokeWeight(vm);
        case 11:
            return background(vm);
        case 12:
            return colorMode(vm);
        case 13:
            return stroke(vm);
        case 14:
            return nofill(vm);
        case 15:
            return nostroke(vm);
        case 16:
            return fill(vm);
        case 17:
            return pushMatrix(vm);
        case 18:
            return popMatrix(vm);
        case 19:
            return resetMatrix(vm);
        case 20:
            return scale(vm);
        case 21:
            return translate(vm);
        case 22:
            return rotate(vm);

	}
    return 0;
}

void moEffectDraw::HandleReturns(moLuaVirtualMachine& vm, const char *strFunc)
{
    if (strcmp (strFunc, "Run") == 0)
    {
        lua_State *state = (lua_State *) vm;
        MOint script_result = (MOint) lua_tonumber (state, 1);
		if (script_result != 0)
			MODebug2->Error(moText("Script returned error code: ") + (moText)IntToStr(script_result));
    }
}

void moEffectDraw::RegisterFunctions()
{
    m_iMethodBase = RegisterFunction("GetFeature");


    RegisterFunction("triangle");
	RegisterFunction("line");
	RegisterFunction("arc");
	RegisterFunction("point");
	RegisterFunction("quad");
	RegisterFunction("ellipse");
	RegisterFunction("rect");
	RegisterFunction("strokeWeight");
	RegisterFunction("background");
	RegisterFunction("colorMode");
	RegisterFunction("stroke");
	RegisterFunction("nofill");
	RegisterFunction("nostroke");
	RegisterFunction("fill");

	RegisterFunction("pushMatrix");
	RegisterFunction("popMatrix");
	RegisterFunction("resetMatrix");

	RegisterFunction("scale");
	RegisterFunction("translate");
	RegisterFunction("rotate");

	RegisterFunction("GetTicks");

	RegisterFunction("PushDebugString");
}

int moEffectDraw::GetFeature(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    float x, y, v;

    if (m_pTrackerData) {
		x = m_pTrackerData->GetFeature(i)->x;
		y = m_pTrackerData->GetFeature(i)->y;
		v = m_pTrackerData->GetFeature(i)->val;
	}
    /*
	if (m_pTrackerData) {
		x = m_pTrackerData->m_FeatureList->feature[i]->x;
		y = m_pTrackerData->m_FeatureList->feature[i]->y;
		v = m_pTrackerData->m_FeatureList->feature[i]->val;
	} else if (m_pTrackerGpuData) {
		x = m_pTrackerGpuData->m_FeatureList->_list[i]->x;
		y = m_pTrackerGpuData->m_FeatureList->_list[i]->y;
		v = m_pTrackerGpuData->m_FeatureList->_list[i]->valid;
	}*/

	lua_pushnumber(state, (lua_Number)x);
	lua_pushnumber(state, (lua_Number)y);
	lua_pushnumber(state, (lua_Number)v);

    return 3;
}

///=========================================================================
/// moP5 luascript functions
///=========================================================================

    int moEffectDraw::triangle(moLuaVirtualMachine& vm) {

        return 0;
    }

	int moEffectDraw::line(moLuaVirtualMachine& vm) {

        MOfloat x0 = 0, y0 = 0, z0 = 0, x1 = 0, y1 = 0, z1 = 0;

        lua_State *state = (lua_State *) vm;

        int count_callinfo = lua_gettop(state);

        switch(count_callinfo) {
            case 4:
                x0 = (MOfloat) lua_tonumber (state, 1);
                y0 = (MOfloat) lua_tonumber (state, 2);
                x1 = (MOfloat) lua_tonumber (state, 3);
                y1 = (MOfloat) lua_tonumber (state, 4);
                MOP5.line( x0, y0, x1, y1 );
                break;
            case 6:
                x0 = (MOfloat) lua_tonumber (state, 1);
                y0 = (MOfloat) lua_tonumber (state, 2);
                z0 = (MOfloat) lua_tonumber (state, 3);
                x1 = (MOfloat) lua_tonumber (state, 4);
                y1 = (MOfloat) lua_tonumber (state, 5);
                z1 = (MOfloat) lua_tonumber (state, 6);
                MOP5.line( x0, y0, z0, x1, y1 , z1);
                break;
            default:
                MODebug2->Error( moText("moEffectDraw::line Error, not enough parameters for function :") + IntToStr(count_callinfo) + moText(" instead of 4 or 6") );
                break;
        }

        return 0;
    }

    int moEffectDraw::arc(moLuaVirtualMachine& vm) {

        return 0;
    }

	int moEffectDraw::point(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::quad(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::ellipse(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::rect(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::strokeWeight(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::background(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::colorMode(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::stroke(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::nofill(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::nostroke(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::fill(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::pushMatrix(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::popMatrix(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::resetMatrix(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::scale(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::translate(moLuaVirtualMachine& vm) { return 0; }

	int moEffectDraw::rotate(moLuaVirtualMachine& vm) { return 0; }

///=========================================================================
/// Moldeo functions
///=========================================================================


int moEffectDraw::GetTicks(moLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	lua_pushnumber(state, (lua_Number)m_Ticks);
    return 1;
}


int moEffectDraw::PushDebugString(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
	if (lua_isboolean(state,1)) {
		bool vb = lua_toboolean(state,1);
		vb ? m_tScriptDebug.Push(moText("true")) : m_tScriptDebug.Push(moText("false"));
	} else {
		char *text = (char *) lua_tostring (state, 1);
		m_tScriptDebug.Push(moText(text));
	}

    return 0;
}
