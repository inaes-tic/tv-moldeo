/*******************************************************************************

                              moEffectLiveDrawing2.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                     *
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

  Copyright(C) 2007 Andrés Colubri

  Authors:
  Andrés Colubri
  Fabricio Costa

*******************************************************************************/

#include "moEffectLiveDrawing2.h"

//========================
//  Factory
//========================

moEffectLiveDrawing2Factory *m_EffectImageFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectImageFactory==NULL)
		m_EffectImageFactory = new moEffectLiveDrawing2Factory();
	return(moEffectFactory*) m_EffectImageFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectImageFactory;
	m_EffectImageFactory = NULL;
}

moEffect*  moEffectLiveDrawing2Factory::Create() {
	return new moEffectLiveDrawing2();
}

void moEffectLiveDrawing2Factory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectLiveDrawing2::moEffectLiveDrawing2() {
	SetName("livedrawing2");
	gestures = lastGesture = NULL;
	nGestures = 0;
}

moEffectLiveDrawing2::~moEffectLiveDrawing2() {
	Finish();
}

MOboolean
moEffectLiveDrawing2::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( LIVEDRAW2_BACK_TEXTURES, moText("back_textures") );
	moDefineParamIndex( LIVEDRAW2_ICO_TEXTURES, moText("ico_textures") );
	moDefineParamIndex( LIVEDRAW2_SHADERS, moText("shaders") );
	moDefineParamIndex( LIVEDRAW2_SCRIPTS, moText("scripts") );
	moDefineParamIndex( LIVEDRAW2_CANVAS_MARGIN, moText("canvas_margin") );
	moDefineParamIndex( LIVEDRAW2_MIN_PRESSURE, moText("min_pressure") );
	moDefineParamIndex( LIVEDRAW2_MAX_PRESSURE, moText("max_pressure") );
	moDefineParamIndex( LIVEDRAW2_MAX_THICKNESS, moText("max_thickness") );
	moDefineParamIndex( LIVEDRAW2_MAX_LINE_LENGTH, moText("max_line_length") );
	moDefineParamIndex( LIVEDRAW2_MIN_LINE_LENGTH, moText("min_line_length") );
	moDefineParamIndex( LIVEDRAW2_FLOW_VELOCITY, moText("flow_velocity") );
	moDefineParamIndex( LIVEDRAW2_FLOW_PERIOD, moText("flow_period") );
	moDefineParamIndex( LIVEDRAW2_ROTOSKETCH_DURATION, moText("rotosketch_duration") );
	moDefineParamIndex( LIVEDRAW2_ROTOSKETCH_START, moText("rotosketch_start") );
	moDefineParamIndex( LIVEDRAW2_DISSOLVE_START, moText("dissolve_start") );
	moDefineParamIndex( LIVEDRAW2_DISSOLVE_TIME, moText("dissolve_time") );
	moDefineParamIndex( LIVEDRAW2_PEN_COLOR, moText("pen_color") );
	moDefineParamIndex( LIVEDRAW2_COLOR_SENSIBILITY, moText("color_sensibility") );
	moDefineParamIndex( LIVEDRAW2_BLENDING, moText("blending") );

	moDefineParamIndex( LIVEDRAW2_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( LIVEDRAW2_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( LIVEDRAW2_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( LIVEDRAW2_ROTATEX, moText("rotatex") );
	moDefineParamIndex( LIVEDRAW2_ROTATEY, moText("rotatey") );
	moDefineParamIndex( LIVEDRAW2_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( LIVEDRAW2_SCALEX, moText("scalex") );
	moDefineParamIndex( LIVEDRAW2_SCALEY, moText("scaley") );
	moDefineParamIndex( LIVEDRAW2_SCALEZ, moText("scalez") );

	//backTextures.MODebug = MODebug;
	//backTextures.Init(GetConfig(), moParamReference(LIVEDRAW2_BACK_TEXTURES), m_pResourceManager->GetTextureMan() );

	//icoTextures.MODebug = MODebug;
	//icoTextures.Init(GetConfig(), moParamReference(LIVEDRAW2_ICO_TEXTURES), m_pResourceManager->GetTextureMan() );

    FeatureActivity.Stop();
    MotionActivity.Stop();
    NoMotionActivity.Stop();
    normalf = 10.0f;
    drawing_features = 2;
    m_bTrackerInit = false;

	inInteractionMode = false;
	updateParameters();
	updateInteractionParameters();

    // Some initialization.
    drawing = false;
	resumed_drawing = false;

	//pentip = MO_TABLET_CURSOR_UNDEFINED;
	pentip = MO_TABLET_CURSOR_PEN;

	tabletDetected = false;
	min_pressure = 0;
    penX = penY = 0;

	icon_mode = false;
	repeat_icon = false;
	stretch_tex = false;

	show_pen = true;
	show_colorbx = true;
	show_timerbx = true;

	tex_tab = 0;

    canvasWidth = m_pResourceManager->GetRenderMan()->RenderWidth();
    canvasHeight = m_pResourceManager->GetRenderMan()->RenderHeight();

    flow_velocity_bak = flow_velocity = flow_velocity0;

	initShaders();
	initScripts();

	return true;
}

void moEffectLiveDrawing2::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw(tempogral, parentstate);

    /*
    // Rehacer --

	if (scripted)
	{
		SelectScriptFunction("Draw");
		RunSelectedFunction();
	}
    */

	m_pResourceManager->GetGLMan()->SetOrthographicView(canvasWidth, canvasHeight);

    // Draw //

	glTranslatef(   m_Config[moR(LIVEDRAW2_TRANSLATEX)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(LIVEDRAW2_TRANSLATEY)].GetData()->Fun()->Eval(state.tempo.ang),
					m_Config[moR(LIVEDRAW2_TRANSLATEZ)].GetData()->Fun()->Eval(state.tempo.ang));

	glRotatef(  m_Config[moR(LIVEDRAW2_ROTATEX)].GetData()->Fun()->Eval(state.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(LIVEDRAW2_ROTATEY)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(LIVEDRAW2_ROTATEZ)].GetData()->Fun()->Eval(state.tempo.ang), 0.0, 0.0, 1.0 );
	glScalef(   m_Config[moR(LIVEDRAW2_SCALEX)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(LIVEDRAW2_SCALEY)].GetData()->Fun()->Eval(state.tempo.ang),
                m_Config[moR(LIVEDRAW2_SCALEZ)].GetData()->Fun()->Eval(state.tempo.ang));

	updateParameters();

	//if (!inInteractionMode)
	updateInteractionParameters();

	if (icon_mode)
		sel_tex = m_Config[ moParamReference(LIVEDRAW2_ICO_TEXTURES) ].GetIndexValue();
	else
		sel_tex = m_Config[ moParamReference(LIVEDRAW2_BACK_TEXTURES) ].GetIndexValue();

	updateFlowVelocity();


	//DrawTracker();


	//CODIGO PARA EL LASER!!!!
	if (m_bTrackerInit ) {/*
        if ( NoMotionActivity.Duration()<100 ) {
            inInteractionMode = true;

            pressure = 0.9;
            show_pen = true;

            penX = m_TrackerBarycenter.X()*canvasWidth;
            if(penX==0.0) penX0 = penX;
            penX = momin(penX, canvasWidth - canvas_margin);
            penX = momax(penX, canvas_margin);

            penY = m_TrackerBarycenter.Y()*canvasHeight;
            if(penY==0.0) penY0 = penY;
            penY = momin(penY, canvasHeight - canvas_margin);
            penY = momax(penY, canvas_margin);
        } else {
            pressure = (float)min_pressure / (float)max_pressure;
            inInteractionMode = false;
        }*/
	}

     MODebug2->Push( moText("Current pressure = ") + FloatToStr(pressure) );

    if (pressure <= (float)min_pressure / (float)max_pressure) drawing = false;
	else
		if (!drawing)
		{
			drawing = true;
	        resumed_drawing = true;
		}
		else resumed_drawing = false;

    if (drawing && (pentip != MO_TABLET_CURSOR_UNDEFINED))
	{
        if (resumed_drawing) addGesture();

		float d = lastGesture->distToLast(penX, penY);

		if (d > max_line_length) addGesture();
        else if (d > min_line_length)
	    {
            lastGesture->AddPoint(penX, penY, pressure, pen_color_rgb);
			lastGesture->compile();
        }
		else
		{
			// draw point.
		}
	}


	if (flow_velocity != 0.0) displaceGestures();
	setBlendMode();

	renderGestures();


	if (show_pen) drawPen();
	if (show_colorbx) drawPalette();
	if (show_timerbx) drawTimer();


	inInteractionMode = false;

}

void moEffectLiveDrawing2::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp,*aux;
	MOint did,cid,state,e,valor,nval,estaux,elaux;

	moEffect::Interaction( IODeviceManager );

	inInteractionMode = true;
	leftClicked = false;

	if (devicecode!=NULL)
	for(int i=0; i<ncodes; i++) {

		temp = devicecode[i].First;

		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);
			nval = IODeviceManager->IODevices().Get(did)->GetNValues(cid);

			if (state)
				if (i == MO_DRAW_TRANSLATE_X)
				{
					if (0 < nval)
					{
						penX0 = penX;
						penX = (float) IODeviceManager->IODevices().Get(did)->GetValue(cid, nval - 1);
						penX = momin(penX, canvasWidth - canvas_margin);
						penX = momax(penX, canvas_margin);
						//MODebug2->Push(IntToStr(penX));
					}
					tabletDetected = true;
				}
				else if (i == MO_DRAW_TRANSLATE_Y)
				{
					if (0 < nval)
					{
						penY0 = penY;
						penY = (int) IODeviceManager->IODevices().Get(did)->GetValue(cid, nval - 1);
						penY = momin(penY, canvasHeight - canvas_margin);
						penY = momax(penY, canvas_margin);
					}
					tabletDetected = true;
				}
				else if (i == MO_DRAW_PRESSURE)
				{
					if (0 < nval)
					{
						pressure = max_pressure;
						for (int n = 0; n < nval; n++)
						{
							valor = (float) IODeviceManager->IODevices().Get(did)->GetValue(cid, n);
							if (valor < pressure) pressure = valor;
						}

						pressure /= max_pressure;
						pressure = momin(pressure, 1.0);
						pressure = momax(pressure, (float)min_pressure / (float)max_pressure);
					}
					tabletDetected = true;
				}
				else if (i == MO_DRAW_AZIMUTH)
				{
					tabletDetected = true;
				}
				else if (i == MO_DRAW_TILT)
				{
					tabletDetected = true;
				}
				else if (i == MO_DRAW_TWIST)
				{
					tabletDetected = true;
				}
				else if (i == MO_DRAW_CURSOR)
				{
					if (0 < nval) pentip = (int) IODeviceManager->IODevices().Get(did)->GetValue(cid, nval - 1);
					tabletDetected = true;
				}
				else if (i == MO_DRAW_BUTTON)
				{
					tabletDetected = true;
				}
				else if (i == MO_KEY_PRESS_1)
				{
					m_Config[ moParamReference(LIVEDRAW2_BACK_TEXTURES) ].SetIndexValue(0 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_2)
				{
					m_Config[ moParamReference(LIVEDRAW2_BACK_TEXTURES) ].SetIndexValue(1 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_3)
				{
					m_Config[ moParamReference(LIVEDRAW2_BACK_TEXTURES) ].SetIndexValue(2 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_4)
				{
					m_Config[ moParamReference(LIVEDRAW2_BACK_TEXTURES) ].SetIndexValue(3 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_5)
				{
					m_Config[ moParamReference(LIVEDRAW2_BACK_TEXTURES) ].SetIndexValue(4 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_6)
				{
					m_Config[ moParamReference(LIVEDRAW2_ICO_TEXTURES) ].SetIndexValue(0 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_7)
				{
					m_Config[ moParamReference(LIVEDRAW2_ICO_TEXTURES) ].SetIndexValue(1 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_8)
				{
					m_Config[ moParamReference(LIVEDRAW2_ICO_TEXTURES) ].SetIndexValue(2 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_9)
				{
					m_Config[ moParamReference(LIVEDRAW2_ICO_TEXTURES) ].SetIndexValue(3 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_0)
				{
					m_Config[ moParamReference(LIVEDRAW2_ICO_TEXTURES) ].SetIndexValue(4 + tex_tab);
				}
				else if (i == MO_KEY_PRESS_I)
				{
					icon_mode = !icon_mode;
				}
				else if (i == MO_KEY_UP)
				{
					pen_color_rgb[3] += 0.1 * pen_color_rgb[3];
					pen_color_rgb[3] = momin(1.0, pen_color_rgb[3]);
				}
				else if (i == MO_KEY_DOWN)
				{
					pen_color_rgb[3] -= 0.1 * pen_color_rgb[3];
					pen_color_rgb[3] = momax(0.0, pen_color_rgb[3]);
				}
				else if (i == MO_KEY_LEFT)
				{
					max_thickness -= 0.1 * max_thickness;
					max_thickness = momax(1.0, max_thickness);
				}
				else if (i == MO_KEY_RIGHT)
				{
					max_thickness += 0.1 * max_thickness;
					max_thickness = momin(500.0, max_thickness);
				}
				else if (i == MO_MOVE_MOUSE_X)
				{
					if (!tabletDetected)
					{
						penX0 = penX;
						penX = valor;
						penX = momin(penX, canvasWidth - canvas_margin);
						penX = momax(penX, canvas_margin);
					}
				}
				else if (i == MO_MOVE_MOUSE_Y)
				{
					if (!tabletDetected)
					{
						penY0 = penY;
						penY = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
						penY = momin(penY, canvasHeight - canvas_margin);
						penY = momax(penY, canvas_margin);
					}
				}
				else if (i == MO_PRESS_MOUSE_LEFT)
				{
					leftClicked = true;
				}
				else if (i == MO_DRAW_COLOR_Z)
				{
					UpdateHSLColor(0, valor);
					ConvertHSLtoRGB();
				}
				else if (i == MO_DRAW_COLOR_X)
				{
					UpdateHSLColor(1, valor);
					ConvertHSLtoRGB();
				}
				else if (i == MO_DRAW_COLOR_Y)
				{
					UpdateHSLColor(2, valor);
					ConvertHSLtoRGB();
				}
				else if (i == MO_KEY_PRESS_X)
				{
					show_pen = !show_pen;
				}
				else if (i == MO_KEY_PRESS_C)
				{
					show_colorbx = !show_colorbx;
				}
				else if (i == MO_KEY_PRESS_Z)
				{
					show_timerbx = !show_timerbx;
				}
				else if (i == MO_KEY_PRESS_R)
				{
					pen_color_rgb[0] = 1.0;
					pen_color_rgb[1] = 0.0;
					pen_color_rgb[2] = 0.0;
					ConvertRGBtoHSL();
				}
				else if (i == MO_KEY_PRESS_A)
				{
					pen_color_rgb[0] = 0.0;
					pen_color_rgb[1] = 0.0;
					pen_color_rgb[2] = 1.0;
					ConvertRGBtoHSL();
				}
				else if (i == MO_KEY_PRESS_V)
				{
					pen_color_rgb[0] = 0.0;
					pen_color_rgb[1] = 1.0;
					pen_color_rgb[2] = 0.0;
					ConvertRGBtoHSL();
				}
				else if (i == MO_KEY_PRESS_B)
				{
					pen_color_rgb[0] = 1.0;
					pen_color_rgb[1] = 1.0;
					pen_color_rgb[2] = 1.0;
					ConvertRGBtoHSL();
				}
				else if (i == MO_KEY_PRESS_N)
				{
					pen_color_rgb[0] = 0.0;
					pen_color_rgb[1] = 0.0;
					pen_color_rgb[2] = 0.0;
					ConvertRGBtoHSL();
				}
				else if (i == MO_KEY_PRESS_S)
				{
					stretch_tex = !stretch_tex;
				}
				else if (i == MO_KEY_PRESS_M)
				{
					repeat_icon = !repeat_icon;
				}
				else if (i == MO_KEY_PRESS_Q)
				{
					scripted = !scripted;
				}
				else if (i == MO_KEY_PRESS_W)
				{
					shaded = !shaded;
				}
				else if (i == MO_KEY_PRESS_BACKSPACE)
				{
					delLastGesture();
				}
				else if (i == MO_KEY_PRESS_DELETE)
				{
					clearGestures();
				}
				else if (i == MO_KEY_PRESS_HOME)
				{
					rotosketch_start += 100;
				}
				else if (i == MO_KEY_PRESS_END)
				{
					rotosketch_start -= 100;
					rotosketch_start = max(0, rotosketch_start);
				}
				else if (i == MO_KEY_PRESS_PAGEUP)
				{
					rotosketch_duration += 100;
				}
				else if (i == MO_KEY_PRESS_PAGEDOWN)
				{
					rotosketch_duration -= 100;
					rotosketch_duration = max(0, rotosketch_duration);
				}
				else if (i == MO_KEY_PRESS_RSHIFT)
				{
					dissolve_time += 100;
				}
				else if (i == MO_KEY_PRESS_LSHIFT)
				{
					dissolve_start += 100;
				}
				else if (i == MO_KEY_PRESS_RCTRL)
				{
					dissolve_time -= 100;
					dissolve_time = max(0, dissolve_time);
				}
				else if (i == MO_KEY_PRESS_LCTRL)
				{
					dissolve_start -= 100;
					dissolve_start = max(0, dissolve_start);
				}
				else if (i == MO_KEY_PRESS_F1)
				{
					tex_tab = 0;
				}
				else if (i == MO_KEY_PRESS_F2)
				{
					tex_tab = 5;
				}

		temp = temp->next;
		}
	}

	if (!tabletDetected)
	{
		if (leftClicked)
		{
			// Cuando no hay una tableta disponible, la "presión" queda determinada por la velocidad del mouse.
			float dx = penX - penX0;
			float dy = penY - penY0;
			float diff = sqrt(dx * dx + dy * dy);

			// La presión está definida como la recíproca de la velocidad: cuanto más rápido
			// se mueva el mouse, más delgada será la línea.
			float pressure1;
			if (0 < diff) pressure1 = 1.0 / diff;
			else pressure1 = 1.0;

			// Utilizando la ecuación de suavizado para que los cambio en presión sean contínuos.
			pressure += 0.1 * (pressure1 - pressure);
			pressure = momin(pressure, 1.0);
			pressure = momax(pressure, (float)min_pressure / (float)max_pressure);
		}
		else pressure = (float)min_pressure / (float)max_pressure;

	}

/*
    //  Rehacer!!!!

	if (scripted)
	{
		SelectScriptFunction("Interaction");
		AddFunctionParam(penX);
		AddFunctionParam(penY);
		AddFunctionParam(pressure);
		RunSelectedFunction();
	}
	*/
}

moConfigDefinition *
moEffectLiveDrawing2::GetDefinition( moConfigDefinition *p_configdefinition ) {
	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("back_textures"), MO_PARAM_TEXTURE, LIVEDRAW2_BACK_TEXTURES );
	p_configdefinition->Add( moText("ico_textures"), MO_PARAM_TEXTURE, LIVEDRAW2_ICO_TEXTURES );
	p_configdefinition->Add( moText("shaders"), MO_PARAM_TEXT, LIVEDRAW2_SHADERS );
	p_configdefinition->Add( moText("scripts"), MO_PARAM_TEXT, LIVEDRAW2_SCRIPTS );

	p_configdefinition->Add( moText("canvas_margin"), MO_PARAM_NUMERIC, LIVEDRAW2_CANVAS_MARGIN, moValue("40","NUM").Ref() );
	p_configdefinition->Add( moText("min_pressure"), MO_PARAM_NUMERIC, LIVEDRAW2_MIN_PRESSURE, moValue("16","NUM").Ref() );
	p_configdefinition->Add( moText("max_pressure"), MO_PARAM_NUMERIC, LIVEDRAW2_MAX_PRESSURE, moValue("1024","NUM").Ref() );

	p_configdefinition->Add( moText("max_thickness"), MO_PARAM_FUNCTION, LIVEDRAW2_MAX_THICKNESS, moValue("2.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("max_line_length"), MO_PARAM_FUNCTION, LIVEDRAW2_MAX_LINE_LENGTH, moValue("200.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("min_line_length"), MO_PARAM_FUNCTION, LIVEDRAW2_MIN_LINE_LENGTH, moValue("1.0","FUNCTION").Ref() );

	p_configdefinition->Add( moText("flow_velocity"), MO_PARAM_FUNCTION, LIVEDRAW2_FLOW_VELOCITY, moValue("0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("flow_period"), MO_PARAM_NUMERIC, LIVEDRAW2_FLOW_PERIOD, moValue("0","NUM").Ref() );

	p_configdefinition->Add( moText("rotosketch_duration"), MO_PARAM_NUMERIC, LIVEDRAW2_ROTOSKETCH_DURATION, moValue("0","NUM").Ref() );
	p_configdefinition->Add( moText("rotosketch_start"), MO_PARAM_NUMERIC, LIVEDRAW2_ROTOSKETCH_START, moValue("0","NUM").Ref() );
	p_configdefinition->Add( moText("dissolve_start"), MO_PARAM_NUMERIC, LIVEDRAW2_DISSOLVE_START, moValue("1000","NUM").Ref() );
	p_configdefinition->Add( moText("dissolve_time"), MO_PARAM_NUMERIC, LIVEDRAW2_DISSOLVE_TIME, moValue("5000","NUM").Ref() );

	p_configdefinition->Add( moText("pen_color"), MO_PARAM_COLOR, LIVEDRAW2_PEN_COLOR );

	p_configdefinition->Add( moText("color_sensibility"), MO_PARAM_FUNCTION, LIVEDRAW2_COLOR_SENSIBILITY, moValue("1.0","FUNCTION").Ref()  );

	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, LIVEDRAW2_BLENDING );

	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, LIVEDRAW2_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, LIVEDRAW2_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, LIVEDRAW2_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, LIVEDRAW2_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, LIVEDRAW2_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, LIVEDRAW2_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, LIVEDRAW2_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, LIVEDRAW2_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, LIVEDRAW2_SCALEZ );

	return p_configdefinition;
}

void moEffectLiveDrawing2::updateParameters()
{
	canvas_margin = m_Config[moR(LIVEDRAW2_CANVAS_MARGIN)].GetData()->Int();
	min_pressure = m_Config[moR(LIVEDRAW2_MIN_PRESSURE)].GetData()->Int();
	max_pressure = m_Config[moR(LIVEDRAW2_MAX_PRESSURE)].GetData()->Int();
	max_thickness = m_Config[moR(LIVEDRAW2_MAX_THICKNESS)].GetData()->Fun()->Eval(state.tempo.ang);
	max_line_length = m_Config[moR(LIVEDRAW2_MAX_LINE_LENGTH)].GetData()->Fun()->Eval(state.tempo.ang);
	min_line_length = m_Config[moR(LIVEDRAW2_MIN_LINE_LENGTH)].GetData()->Fun()->Eval(state.tempo.ang);
	flow_velocity0 = m_Config[moR(LIVEDRAW2_FLOW_VELOCITY)].GetData()->Fun()->Eval(state.tempo.ang);
	flow_period = m_Config[moR(LIVEDRAW2_FLOW_PERIOD)].GetData()->Int();
	color_sens = m_Config[moR(LIVEDRAW2_COLOR_SENSIBILITY)].GetData()->Fun()->Eval(state.tempo.ang);
	blending = m_Config[moR(LIVEDRAW2_BLENDING)].GetData()->Int();
}

void moEffectLiveDrawing2::updateInteractionParameters()
{
	for (int i = MO_RED; i <= MO_ALPHA; i++)
		pen_color_rgb[i] = m_Config[moR(LIVEDRAW2_PEN_COLOR)][MO_SELECTED][i].GetData()->Fun()->Eval(state.tempo.ang);
	ConvertRGBtoHSL();
	rotosketch_duration = m_Config[moR(LIVEDRAW2_ROTOSKETCH_DURATION)].GetData()->Int();
	rotosketch_start = m_Config[moR(LIVEDRAW2_ROTOSKETCH_START)].GetData()->Int();
	dissolve_start = m_Config[moR(LIVEDRAW2_DISSOLVE_START)].GetData()->Int();
	dissolve_time = m_Config[moR(LIVEDRAW2_DISSOLVE_TIME)].GetData()->Int();

	int back_idx = m_Config[moR(LIVEDRAW2_BACK_TEXTURES)].GetIndexValue();
	m_Config[ moParamReference(LIVEDRAW2_BACK_TEXTURES) ].SetIndexValue(back_idx);

	int ico_idx = m_Config[moR(LIVEDRAW2_ICO_TEXTURES)].GetIndexValue();
	m_Config[ moParamReference(LIVEDRAW2_ICO_TEXTURES) ].SetIndexValue(ico_idx);
}

void moEffectLiveDrawing2::initShaders()
{

    moText shader_fn = m_Config[moR(LIVEDRAW2_SHADERS)].GetData()->Text();
	if (shader_fn != moText(""))
	{
		MODebug2->Push("Loading livedrawing shader...");
		moShaderGLSL* pglsl;
		line_shader = m_pResourceManager->GetShaderMan()->AddShader(shader_fn);
		pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(line_shader);
		pglsl->PrintFragShaderLog();

		line_shader_tex_unit = pglsl->GetUniformID("src_tex_unit0");
		line_shader_tex_offset = pglsl->GetUniformID("src_tex_offset0");
		line_shader_tempo_angle = pglsl->GetUniformID("tempo_angle");
		line_shader_point0 = pglsl->GetAttribID("point0");
		line_shader_point1 = pglsl->GetAttribID("point1");
		line_shader_point2 = pglsl->GetAttribID("point2");
		line_shader_quad_width = pglsl->GetAttribID("quad_width");
		line_shader_pressure = pglsl->GetAttribID("pressure");
	}
	else
	{
		line_shader = -1;
	}
	shaded = false;
}

void moEffectLiveDrawing2::initScripts()
{
    /*
    /////Rehacer!!!

	moText script_fn = m_Config[moParamReference(LIVEDRAW2_SCRIPTS)][MO_SELECTED][0].Text();
	if (script_fn != moText(""))
	{
		script_fn = (moText)m_pResourceManager->GetDataMan()->GetDataPath()+ moText("/") + (moText)script_fn;

		moScript::InitScript();
		RegisterFunctions();
		CompileFile(script_fn);

		SelectScriptFunction("Init");
		AddFunctionParam(canvasWidth);
		AddFunctionParam(canvasHeight);
		RunSelectedFunction();
	}

	scripted = false;
	*/
}

void moEffectLiveDrawing2::RegisterFunctions()
{
    m_iMethodBase = RegisterFunction("PushDebugString");
}

int moEffectLiveDrawing2::ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber)
{
    switch (iFunctionNumber - m_iMethodBase)
    {
        case 0:
            return PushDebugString(vm);
	}
    return 0;
}

void moEffectLiveDrawing2::HandleReturns(moLuaVirtualMachine& vm, const char *strFunc)
{
	/*
    if (strcmp (strFunc, "Draw") == 0)
    {
        lua_State *state = (lua_State *) vm;
        MOint script_result = (MOint) lua_tonumber (state, 1);
		if (script_result != 0)
			MODebug->Push("Script returned error code: " + IntToStr(script_result));
    }
	*/
}

int moEffectLiveDrawing2::PushDebugString(moLuaVirtualMachine& vm)
{
	/*
    lua_State *state = (lua_State *) vm;
	if (lua_isboolean(state,1)) {
		bool vb = lua_toboolean(state,1);
		vb ? m_tScriptDebug.Push(moText("true")) : m_tScriptDebug.Push(moText("false"));
	} else {
		char *text = (char *) lua_tostring (state, 1);
		m_tScriptDebug.Push(moText(text));
	}
*/
    return 0;
}

MOboolean moEffectLiveDrawing2::Finish()
{
    /*
    // Rehacer
	if (scripted)
	{
		SelectScriptFunction("Finish");
		RunSelectedFunction();
	}
	*/

	clearGestures();
	//backTextures.Finish();
	//icoTextures.Finish();
    return PreFinish();
}

void moEffectLiveDrawing2::setBlendMode()
{
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

void moEffectLiveDrawing2::drawPen()
{
    glBindTexture(GL_TEXTURE_2D,0);
    glColor4f(pen_color_rgb[0],
              pen_color_rgb[1],
              pen_color_rgb[2],
              1.0);

	//float l = 0.5 + 4.5 * pow(max_thickness / 500.0, 2);
	float l = 1.0;

    glBegin(GL_QUADS);
	    glVertex2f(penX - 4, penY - 4);
	    glVertex2f(penX - 4, penY + 4);
	    glVertex2f(penX + 4, penY + 4);
	    glVertex2f(penX + 4, penY - 4);
    glEnd();
}

void moEffectLiveDrawing2::drawPalette()
{
	// Relleno.
	glBindTexture(GL_TEXTURE_2D,0);
    glColor4f(pen_color_rgb[0],
              pen_color_rgb[1],
              pen_color_rgb[2],
              pen_color_rgb[3]);

    glBegin(GL_QUADS);
	    glVertex2f(0, 0);
	    glVertex2f(0, 10.0);
	    glVertex2f(0 + 10.0, 10.0);
	    glVertex2f(0 + 10.0, 0);
    glEnd();

	// Bordes.
    glColor4f(0.0, 0.0, 0.0, 1.0);
	glLineWidth(1.0);
	glBegin(GL_LINES);
		glVertex2f(0, 10);
		glVertex2f(10, 10);
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(10, 10);
		glVertex2f(10, 0);
	glEnd();
}

void moEffectLiveDrawing2::drawTimer()
{
	drawTimer(20.0, 100.0, 5 * (float)rotosketch_start / 1000.0, 5 * (float)rotosketch_duration / 1000.0);
	drawTimer(110.0, 190.0, 5 * (float)dissolve_start / 1000.0, 5 * (float)dissolve_time / 1000.0);
}

void moEffectLiveDrawing2::drawTimer(float box0, float box1, float time0, float time1)
{
	// Fondo.
	glBindTexture(GL_TEXTURE_2D,0);
    glColor4f(0, 0, 0, 1.0);
    glBegin(GL_QUADS);
	    glVertex2f(box0, 0);
	    glVertex2f(box0, 10.0);
	    glVertex2f(box1, 10.0);
	    glVertex2f(box1, 0);
    glEnd();

	// Lineas de comienzo.
	glLineWidth(2.0);
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glBegin(GL_LINES);
		glVertex2f(box0, 5);
		glVertex2f(min(box1, box0 + time0), 5);
	glEnd();
	// Marcas de segundo.
	glLineWidth(1.0);
	for (float i = 0; i < time0; i += 5.0)
	{
		glBegin(GL_LINES);
			glVertex2f(min(box1, box0 + i), 5);
			glVertex2f(min(box1, box0 + i), 10);
		glEnd();
	}

	// Lineas de duracion.
	glLineWidth(2.0);
	glColor4f(0.0, 1.0, 1.0, 1.0);
	glBegin(GL_LINES);
		glVertex2f(min(box1, box0 + time0), 5);
		glVertex2f(min(box1, box0 + time0 + time1), 5);
	glEnd();
	// Marcas de segundo.
	glLineWidth(1.0);
	for (float i = 0; i < time1; i += 5.0)
	{
		glBegin(GL_LINES);
			glVertex2f(min(box1, box0 + time0 + i), 5);
			glVertex2f(min(box1, box0 + time0 + i), 10);
		glEnd();
	}
}

void moEffectLiveDrawing2::updateFlowVelocity()
{
	flow_velocity_bak = flow_velocity;
	if (state.tempo.delta <= 1.0) flow_velocity = state.tempo.delta * flow_velocity0;
	else flow_velocity = (1.0 + 10.0 * (state.tempo.delta - 1.0)) * flow_velocity0;
	//if (flow_velocity != flow_velocity_bak) MODebug->Push("Flow velocity: " + FloatToStr(flow_velocity));
}

void moEffectLiveDrawing2::displaceGestures()
{
	Gesture* p = gestures;
	while ((p != NULL) && (p->next != NULL))
	{
		p->displacePolygons(flow_velocity, flow_period);
		p = p->next;
	}

	if (!drawing && (lastGesture != NULL)) lastGesture->displacePolygons(flow_velocity, flow_period);
}

void moEffectLiveDrawing2::renderGestures()
{
	Gesture* p = gestures;
	while (p != NULL)
	{
		p->RotosketchUpdate();
		p->RenderPolygons();
		p = p->next;
	}
}

void moEffectLiveDrawing2::delLastGesture()
{
	Gesture* p = gestures;

	if (p == NULL) return;

	if (p == lastGesture)
	{
		// Hay una solo trazo.
		delete gestures;
		gestures = lastGesture = NULL;
	}
	else
	{
		// Hay mas de un trazo.
		while (p != NULL)
		{
			if (p->next == lastGesture)
			{
				p->next = NULL;
				delete lastGesture;
				lastGesture = p;
			}
			p = p->next;
		}
	}
}

void moEffectLiveDrawing2::clearGestures()
{
	Gesture* p = gestures;
	Gesture* temp;
	while (p != NULL)
	{
		temp = p;
		p = p->next;
		delete temp;
	}
	gestures = lastGesture = NULL;
}

void moEffectLiveDrawing2::addGesture()
{
	moShaderGLSL* pglsl;
	moData* pDataIconTexture = m_Config[moR(LIVEDRAW2_ICO_TEXTURES)].GetData();
	moData* pDataBackTexture = m_Config[moR(LIVEDRAW2_BACK_TEXTURES)].GetData();


	if (icon_mode || (line_shader == -1) || !shaded) pglsl = NULL;
	else pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(line_shader);


	if (lastGesture == NULL)
	{
		if (icon_mode)
			gestures = new Gesture(canvasWidth, canvasHeight, max_thickness, icon_mode,
									stretch_tex, repeat_icon, pDataIconTexture, sel_tex, &state,
									rotosketch_duration, rotosketch_start,
									dissolve_time, dissolve_start,
									pglsl, -1, -1, -1, -1, -1, -1, -1, -1);
		else
			gestures = new Gesture(canvasWidth, canvasHeight, max_thickness, icon_mode,
									stretch_tex, repeat_icon, pDataBackTexture, sel_tex, &state,
									rotosketch_duration, rotosketch_start,
									dissolve_time, dissolve_start,
									pglsl, line_shader_tex_unit, line_shader_tex_offset, line_shader_tempo_angle,
									line_shader_point0, line_shader_point1, line_shader_point2,
									line_shader_quad_width, line_shader_pressure);
		lastGesture = gestures;
	}
	else
	{
		if (icon_mode)
			lastGesture->next = new Gesture(canvasWidth, canvasHeight, max_thickness, icon_mode,
											stretch_tex, repeat_icon, pDataIconTexture, sel_tex, &state,
											rotosketch_duration, rotosketch_start,
											dissolve_time, dissolve_start,
											NULL, -1, -1, -1, -1, -1, -1, -1, -1);
		else
			lastGesture->next = new Gesture(canvasWidth, canvasHeight, max_thickness, icon_mode,
											stretch_tex, repeat_icon, pDataBackTexture, sel_tex, &state,
											rotosketch_duration, rotosketch_start,
											dissolve_time, dissolve_start,
											pglsl, line_shader_tex_unit, line_shader_tex_offset, line_shader_tempo_angle,
											line_shader_point0, line_shader_point1, line_shader_point2,
											line_shader_quad_width, line_shader_pressure);
		lastGesture = lastGesture->next;
	}



	nGestures++;
}

void moEffectLiveDrawing2::UpdateHSLColor(int i, float v)
{
	pen_color_hsl[i] += v / color_sens;
	if (pen_color_hsl[i] < 0.0) pen_color_hsl[i] = 0.0;
	if (1.0 < pen_color_hsl[i]) pen_color_hsl[i] = 1.0;
}

void moEffectLiveDrawing2::ConvertHSLtoRGB()
{
	float m1, m2;
	float h = pen_color_hsl[0];
	float s = pen_color_hsl[1];
	float l = pen_color_hsl[2];

	if (s == 0.0)
	{
		pen_color_rgb[0] = l;
		pen_color_rgb[1] = l;
		pen_color_rgb[2] = l;
	}
	else
	{
		if (l <= 0.5) m2 = l * (1.0 + s);
		else m2 = l + s - (l * s);
		m1 = 2.0 * l - m2;
		pen_color_rgb[0] = HuetoRGB(m1, m2, h + 1.0/3.0);
		pen_color_rgb[1] = HuetoRGB(m1, m2, h);
		pen_color_rgb[2] = HuetoRGB(m1, m2, h - 1.0/3.0);
	}
}

void moEffectLiveDrawing2::ConvertRGBtoHSL()
{
	float cmax, cmin, delta;
	float r = pen_color_rgb[0];
	float g = pen_color_rgb[1];
	float b = pen_color_rgb[2];

	cmax = b;
	cmin = b;

	if (r > cmax) cmax = r;
	if (g > cmax) cmax = g;
	if (r < cmin) cmin = r;
	if (g < cmin) cmin = g;
	pen_color_hsl[2] = (cmax + cmin) / 2.0;

	if (cmax == cmin)
	{
	   pen_color_hsl[1] = 0.0;
	   pen_color_hsl[0] = 0.0;   // actually it's undefined.
	}
	else
	{
		  delta = cmax - cmin;
		  if (pen_color_hsl[2] < 0.5) pen_color_hsl[1] = delta / (cmax + cmin);
		  else pen_color_hsl[1] = delta / (2.0 - cmax - cmin);

		  if (r == cmax) pen_color_hsl[0] = (g - b) / delta;
		  else if (g == cmax) pen_color_hsl[0] = 2.0 + (b - r) / delta;
		  else pen_color_hsl[0] = 4.0 + (r - g) / delta;

		  pen_color_hsl[0] = pen_color_hsl[0] / 6.0;
		  if (pen_color_hsl[0] < 0.0) pen_color_hsl[0] = pen_color_hsl[0] + 1;
	}
}

float moEffectLiveDrawing2::HuetoRGB(float m1, float m2, float h)
{
	if (h < 0) h = h + 1.0;
	if (h > 1) h = h - 1.0;
	if (6.0 * h < 1) return (m1+(m2-m1)*h*6.0);
	else
	{
		if (2.0 * h < 1) return m2;
		else
			if (3.0*h < 2.0) return (m1+(m2-m1)*((2.0/3.0)-h)*6.0);
			else return m1;
	}
}

void Gesture::AddPoint(float x, float y, float p, float* color)
{
	if (lastPoint == NULL)
	{
		points = new Vec3f(x, y , p);
		lastPoint = points;
	}
	else
	{
	    lastPoint->next = new Vec3f(x, y , p);
		lastPoint = lastPoint->next;
	}
	lastPoint->set(color[0], color[1], color[2], color[3]);
	nPoints++;
}

void Gesture::AddPoly()
{
	if (lastPoly == NULL)
	{
		polygons = new Poly2f(w, h);
		lastPoly = polygons;
	}
	else
	{
	    lastPoly->next = new Poly2f(w, h);
		lastPoly = lastPoly->next;
	}
	nPolys++;
}

void Gesture::RenderPolygons()
{
	Poly2f* p = polygons;
	int texw = 1;
	int texh = 1;

	if (datatexture!=NULL) {

	    moTexture* pTexture = (moTexture*) datatexture->Pointer();
	    if (pTexture!=NULL) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, datatexture->GetGLId( &(state->tempo), 1, NULL ) );
            texw = pTexture->GetWidth();
            texh = pTexture->GetHeight();
	    } else {
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0 );
        }
	} else {
	    glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0 );
    }

    float icof = 0.0;

	if (line_shader != NULL)
	{
		line_shader->StartShader();
		glUniform1iARB(line_shader_tex_unit, 0);
		if (-1 < line_shader_tex_offset)
		{
			glUniform2fARB(line_shader_tex_offset, 1.0 / float(texw), 1.0 / float(texh));
		}
		if (-1 < line_shader_tempo_angle)
		{
			float a = state->tempo.ang;
			float f = fmod(float(a), float(2.0 * moMathf::PI)) / (2.0 * moMathf::PI);
			glUniform2fARB(line_shader_tempo_angle, a, f);
		}
	}

	int t0 = moGetTicks();
	float f = 1.0;
    int t = t0 - compileTime;
	if ((0 < dissolve_start) && (0 < dissolve_time) && (-1 < compileTime) && (t > dissolve_start))
		f = min(1.0, 1.0 - float(t - dissolve_start) / dissolve_time);

	if (0 < f)
	{
		int i = 0;
		while (p != NULL)
		{
			if (icon_mode)
			{
				if (i == 0) icof += p->press;

				if (repeat_icon || (p->next == NULL))
				{
					p->DrawIcon(state->tintr, state->tintg, state->tintb, state->alpha * f, texw, texh, icof);
				}
			}
			else
			{
				if ((rotsketchIndex == i) && (t0 - lastRotosketchUpdateTime < rotosketch_duration))
				{
					float fr = min(1.0, 1.0 - float(t0 - lastRotosketchUpdateTime) / rotosketch_duration);
					p->Draw(state->tintr, state->tintg, state->tintb, state->alpha * f * fr, stretch_tex,
						line_shader_point0, line_shader_point1, line_shader_point2,
						line_shader_quad_width, line_shader_pressure);
				}
				else if (rotsketchIndex < i)
					p->Draw(state->tintr, state->tintg, state->tintb, state->alpha * f, stretch_tex,
						line_shader_point0, line_shader_point1, line_shader_point2,
						line_shader_quad_width, line_shader_pressure);
			}

			p = p->next;

			i++;
		}
	}

	if (line_shader != NULL) line_shader->StopShader();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Gesture::RotosketchUpdate()
{
	if ((rotosketch_duration == 0) || (rotosketch_start == 0)) return;

    int t = moGetTicks();
	if ((-1 < compileTime) && (t - compileTime > rotosketch_start) && (t - lastRotosketchUpdateTime > rotosketch_duration))
	{
		rotsketchIndex++;
		lastRotosketchUpdateTime = t;
	}
}

void Gesture::displacePolygons(float vel, int period)
{
	Poly2f* p = polygons;

    while (p != NULL)
	{
		p->Displace(vel, period);
	    p = p->next;
	}
}

void Gesture::clearPoints()
{
	Vec3f* p = points;
	Vec3f* temp;
	while (p != NULL)
	{
		temp = p;
		p = p->next;
		delete temp;
	}
	points = lastPoint = NULL;
}

void Gesture::clearPolys()
{
	Poly2f* p = polygons;
	Poly2f* temp;
	while (p != NULL)
	{
		temp = p;
		p = p->next;
		delete temp;
	}
	polygons = lastPoly = NULL;
}

void Gesture::compile()
{
    // compute the polygons from the path of Vec3f's
	if (nPoints <= 3) return;

      clearPolys();

	  Poly2f* apoly;

      Vec3f *p0, *p1, *p2;
      float radius0, radius1;
      float ax, bx, cx, dx;
      float ay, by, cy, dy;
      int   axi, bxi, cxi, dxi, axip, axid;
      int   ayi, byi, cyi, dyi, ayip, ayid;
      float p1x, p1y;
      float dx01, dy01, hp01, si01, co01;
      float dx02, dy02, hp02, si02, co02;
      float taper = 1.0f;

      int  nPathPoints = nPoints - 1;
      int  lastPolyIndex = nPathPoints - 1;
      float npm1finv =  1.0f/max(1, nPathPoints - 1);

      // handle the first point
      p0 = points;
	  p1 = points->next;
      radius0 = p0->p*thickness;
      dx01 = p1->x - p0->x;
      dy01 = p1->y - p0->y;
      hp01 = (float) sqrt(dx01*dx01 + dy01*dy01);
      if (hp01 == 0) {
        hp02 = 0.0001f;
      }
      co01 = radius0 * dx01 / hp01;
      si01 = radius0 * dy01 / hp01;
      ax = p0->x - si01; ay = p0->y + co01;
      bx = p0->x + si01; by = p0->y - co01;

      float* xpts;
      float* ypts;

      float mint = 0.618f;
      double tapow = 0.4f;

      // handle the middle points
	  p1 = points;
	  p2 = points->next;
	  while ((p2 != NULL) && (p2->next != NULL))
	  {
        taper = (float)pow( (float)(lastPolyIndex-1)*npm1finv, (float)tapow);

        p0 = p1;
        p1 = p2;
		p2 = p2->next;

        p1x = p1->x;
        p1y = p1->y;
        radius1 = max(mint,taper*p1->p*thickness);

        // assumes all segments are roughly the same length...
        dx02 = p2->x - p0->x;
        dy02 = p2->y - p0->y;
        hp02 = (float) sqrt(dx02*dx02 + dy02*dy02);
        if (hp02 != 0) {
          hp02 = radius1/hp02;
        }
        co02 = dx02 * hp02;
        si02 = dy02 * hp02;

        // translate the integer coordinates to the viewing rectangle
        axi = axip = (int)ax;
        ayi = ayip = (int)ay;
        axi= (axi<0) ? (w -((-axi) % w)) : axi % w;
        axid = axi-axip;
        ayi= (ayi<0) ? (h -((-ayi) % h)) : ayi % h;
        ayid = ayi-ayip;

        // set the vertices of the polygon
		AddPoly();
        apoly = lastPoly;

		apoly->SetColor0(p1->r, p1->g, p1->b, p1->a);
		apoly->SetColor1(p2->r, p2->g, p2->b, p2->a);
		apoly->SetPressure(0.5 * (p1->p + p2->p));

        xpts = apoly->xpoints;
        ypts = apoly->ypoints;

        xpts[0] = axi = axid + axip;
        xpts[1] = bxi = axid + (int) bx;
        xpts[2] = cxi = axid + (int)(cx = p1x + si02);
        xpts[3] = dxi = axid + (int)(dx = p1x - si02);
        ypts[0] = ayi = ayid + ayip;
        ypts[1] = byi = ayid + (int) by;
        ypts[2] = cyi = ayid + (int)(cy = p1y - co02);
        ypts[3] = dyi = ayid + (int)(dy = p1y + co02);

        //swap data for next time
        ax = dx; ay = dy;
        bx = cx; by = cy;
      }

	  if (p2 != NULL)
	  {
		  // handle the last point
		  AddPoly();
		  apoly = lastPoly;

  		  apoly->SetColor0(p1->r, p1->g, p1->b, p1->a);
		  apoly->SetColor1(p2->r, p2->g, p2->b, p2->a);
		  apoly->SetPressure(0.5 * (p1->p + p2->p));

		  xpts = apoly->xpoints;
		  ypts = apoly->ypoints;

		  xpts[0] = (int)ax;
		  xpts[1] = (int)bx;
		  xpts[2] = (int)(p2->x);
		  xpts[3] = (int)(p2->x);

		  ypts[0] = (int)ay;
		  ypts[1] = (int)by;
		  ypts[2] = (int)(p2->y);
		  ypts[3] = (int)(p2->y);
	  }
	  lastRotosketchUpdateTime = compileTime = moGetTicks();
}

float Gesture::distToLast(float ix, float iy)
{
    if (lastPoint != NULL)
	{
      float dx = lastPoint->x - ix;
      float dy = lastPoint->y - iy;
      return (float) sqrt(dx*dx + dy*dy);
    }
	else
	{
      return 5;
    }
}

void Gesture::smooth()
{
    // average neighboring points
	if (nPoints <= 3) return;

    float weight = 18.0;
    float scale  = 1.0/(weight + 2.0);
    int nPointsMinusTwo = nPoints - 2;
    Vec3f *lower, *upper, *center;

	center = points;
	upper = points->next;
	while ((upper != NULL) && (upper->next != NULL))
	{
      lower = center;
      center = upper;
	  upper = upper->next;

      center->x = (lower->x + weight*center->x + upper->x)*scale;
      center->y = (lower->y + weight*center->y + upper->y)*scale;
    }
}

void Poly2f::DrawIcon(float tintr, float tintg, float tintb, float alpha, int icow, int icoh, float icof)
{
	bool inside_screen = ((0 <= xpoints[0]) && (xpoints[0] <= w) || (0 <= ypoints[0]) && (ypoints[0] <= h)) ||
						 ((0 <= xpoints[1]) && (xpoints[1] <= w) || (0 <= ypoints[1]) && (ypoints[1] <= h)) ||
						 ((0 <= xpoints[2]) && (xpoints[2] <= w) || (0 <= ypoints[2]) && (ypoints[2] <= h)) ||
						 ((0 <= xpoints[3]) && (xpoints[3] <= w) || (0 <= ypoints[3]) && (ypoints[3] <= h));

		if (inside_screen)
		{
			float f = 0.5 + 0.5 * icof;

			glBegin(GL_QUADS);
				glColor4f(red0 * tintr, green0 * tintg, blue0 * tintb, alpha0 * alpha);

				glTexCoord2f(0.0, 0.0);
				glVertex2f(xpoints[0] - f * 0.5 * icow, ypoints[0] - f * 0.5 * icoh);

				glTexCoord2f(0.0, 1.0);
				glVertex2f(xpoints[0] - f * 0.5 * icow, ypoints[0] + f * 0.5 * icoh);

				glColor4f(red1 * tintr, green1 * tintg, blue1 * tintb, alpha1 * alpha);
				glTexCoord2f(1.0, 1.0);
				glVertex2f(xpoints[0] + f * 0.5 * icow, ypoints[0] + f * 0.5 * icoh);

				glTexCoord2f(1.0, 0.0);
				glVertex2f(xpoints[0] + f * 0.5 * icow, ypoints[0] - f * 0.5 * icoh);
			glEnd();
		}
}

void Poly2f::Draw(float tintr, float tintg, float tintb, float alpha, bool stretch,
				  int line_shader_point0, int line_shader_point1, int line_shader_point2,
				  int line_shader_quad_width, int line_shader_pressure)
{

	bool inside_screen = ((0 <= xpoints[0]) && (xpoints[0] <= w) || (0 <= ypoints[0]) && (ypoints[0] <= h)) ||
						 ((0 <= xpoints[1]) && (xpoints[1] <= w) || (0 <= ypoints[1]) && (ypoints[1] <= h)) ||
						 ((0 <= xpoints[2]) && (xpoints[2] <= w) || (0 <= ypoints[2]) && (ypoints[2] <= h)) ||
						 ((0 <= xpoints[3]) && (xpoints[3] <= w) || (0 <= ypoints[3]) && (ypoints[3] <= h));

		if (inside_screen)
		{
			// El shader necesita los vectores que definen el rectángulo que se está dibujando en este
			// momento: (x0, y0) es el punto medio entre los dos vertices que definen el primer lado
			// del rectángulo, (x2, y2) es el punto medio del lado opuesto. La distancia l es la mitad
			// del ancho del rectángulo.
			float x0 = 0.5 * (xpoints[0] + xpoints[1]);
			float y0 = 0.5 * (ypoints[0] + ypoints[1]);
			float x2 = 0.5 * (xpoints[2] + xpoints[3]);
			float y2 = 0.5 * (ypoints[2] + ypoints[3]);
			float l;
			if (-1 < line_shader_quad_width)
				l = sqrt((x0 - xpoints[1]) * (x0 - xpoints[1]) + (y0 - ypoints[1]) * (y0 - ypoints[1]));
			else l = 0.0;

			float s0, t0, s1, t1, s2, t2, s3, t3;
			if (stretch)
			{
				s0 = 0.0; t0 = 0.0;
				s1 = 0.0; t1 = 1.0;
				s2 = 1.0; t2 = 1.0;
				s3 = 1.0; t3 = 0.0;
			}
			else
			{
				s0 = (float)xpoints[0] / (float)w; t0 = (float)ypoints[0] / (float)h;
				s1 = (float)xpoints[1] / (float)w; t1 = (float)ypoints[1] / (float)h;
				s2 = (float)xpoints[2] / (float)w; t2 = (float)ypoints[2] / (float)h;
				s3 = (float)xpoints[3] / (float)w; t3 = (float)ypoints[3] / (float)h;
			}

			glBegin(GL_QUADS);
				SetVertexAttrib(red0 * tintr, green0 * tintg, blue0 * tintb, alpha0 * alpha,
								x0, y0, xpoints[1], ypoints[1], x2, y2, l, press,
								line_shader_point0, line_shader_point1, line_shader_point2,
								line_shader_quad_width, line_shader_pressure);
				glTexCoord2f(s0, t0);
				glVertex2f(xpoints[0], ypoints[0]);

				SetVertexAttrib(red0 * tintr, green0 * tintg, blue0 * tintb, alpha0 * alpha,
								x0, y0, xpoints[1], ypoints[1], x2, y2, l, press,
								line_shader_point0, line_shader_point1, line_shader_point2,
								line_shader_quad_width, line_shader_pressure);
				glTexCoord2f(s1, t1);
				glVertex2f(xpoints[1], ypoints[1]);

				SetVertexAttrib(red1 * tintr, green1 * tintg, blue1 * tintb, alpha1 * alpha,
								x0, y0, xpoints[1], ypoints[1], x2, y2, l, press,
								line_shader_point0, line_shader_point1, line_shader_point2,
								line_shader_quad_width, line_shader_pressure);
				glTexCoord2f(s2, t2);
				glVertex2f(xpoints[2], ypoints[2]);

				SetVertexAttrib(red1 * tintr, green1 * tintg, blue1 * tintb, alpha1 * alpha,
								x0, y0, xpoints[1], ypoints[1], x2, y2, l, press,
								line_shader_point0, line_shader_point1, line_shader_point2,
								line_shader_quad_width, line_shader_pressure);
				glTexCoord2f(s3, t3);
				glVertex2f(xpoints[3], ypoints[3]);
			glEnd();
		}
}


void Poly2f::SetVertexAttrib(float r, float g, float b, float a,
							 float x0, float y0, float x1, float y1, float x2, float y2,
							 float l, float p,
							 int line_shader_point0, int line_shader_point1, int line_shader_point2,
							 int line_shader_quad_width, int line_shader_pressure)
{
	glColor4f(r, g, b, a);
	if (-1 < line_shader_point0) glVertexAttrib2f(line_shader_point0, x0, y0);
	if (-1 < line_shader_point1) glVertexAttrib2f(line_shader_point1, x1, y1);
	if (-1 < line_shader_point2) glVertexAttrib2f(line_shader_point2, x2, y2);
	if (-1 < line_shader_quad_width) glVertexAttrib1f(line_shader_quad_width, l);
	if (-1 < line_shader_pressure) glVertexAttrib1f(line_shader_pressure, p);
}

void Poly2f::Displace(float vel, int period)
{
	int i;

	for (i = 0; i < 4; i++) xpoints[i] += vel;

	float d = period * w;
	if (0 < vel)
	{
		bool wrap = false;
        for (i = 0; i < 4; i++)
			if (d < xpoints[i])
			{
				wrap = true;
				break;
			}
		if (wrap)
			for (i = 0; i < 4; i++) xpoints[i] -= d;
	}
	else
	{
		bool wrap = false;
        for (i = 0; i < 4; i++)
			if (xpoints[i] - w < -d)
			{
				wrap = true;
				break;
			}
		if (wrap)
			for (i = 0; i < 4; i++) xpoints[i] += d;
	}
}

void Poly2f::SetColor0(float r, float g, float b, float a)
{
	red0 = r;
	green0 = g;
	blue0 = b;
	alpha0 = a;
}

void Poly2f::SetColor0(float r, float g, float b)
{
	red0 = r;
	green0 = g;
	blue0 = b;
}

void Poly2f::SetAlpha0(float a)
{
	alpha0 = a;
}

void Poly2f::SetColor1(float r, float g, float b, float a)
{
	red1 = r;
	green1 = g;
	blue1 = b;
	alpha1 = a;
}

void Poly2f::SetColor1(float r, float g, float b)
{
	red1 = r;
	green1 = g;
	blue1 = b;
}

void Poly2f::SetAlpha1(float a)
{
	alpha1 = a;
}

void Poly2f::SetPressure(float p)
{
	press = p;
}



void moEffectLiveDrawing2::DrawTracker() {

    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    moTrackerSystemData* m_pTrackerData = NULL;


    bool has_motion = false;
    bool has_features = false;

	for(int i=0; i<m_Inlets.Count(); i++) {
		moInlet* pInlet = m_Inlets[i];
		if (pInlet->Updated() && ( pInlet->GetConnectorLabelName()==moText("TRACKERKLT") || pInlet->GetConnectorLabelName()==moText("TRACKERGPUKLT") || pInlet->GetConnectorLabelName()==moText("TRACKERGPUKLT2")) ) {

			m_pTrackerData = (moTrackerSystemData *)pInlet->GetData()->Pointer();
			if (m_pTrackerData ) {
				m_bTrackerInit = true;

				//SelectScriptFunction("Reset");
				//RunSelectedFunction();

				//MODebug2->Push(IntToStr(TrackerId));

				//MODebug2->Push(moText("Receiving:") + IntToStr(m_pTrackerData->GetFeaturesCount()) );
				if (m_pTrackerData->GetFeaturesCount()>0) {
                    int tw = m_pTrackerData->GetVideoFormat().m_Width;
                    int th = m_pTrackerData->GetVideoFormat().m_Height;
                    //MODebug2->Push(moText("vformat:")+IntToStr(tw)+moText("th")+IntToStr(th));

                    m_TrackerBarycenter = moVector2f( ( ( m_pTrackerData->GetBarycenter().X() / (float) tw )),
                                                      ( 1.0 - ( m_pTrackerData->GetBarycenter().Y() / (float) th )) );

                    //MODebug2->Push(moText("Barycenter x:")+FloatToStr(m_TrackerBarycenter.X()) + moText(" y:")+FloatToStr(m_TrackerBarycenter.Y()) );
/*
                    glBindTexture(GL_TEXTURE_2D,0);
                    glColor4f(0.7, 1.0, 0.5, 1.0);

                    glBegin(GL_QUADS);
                        glVertex2f((m_TrackerBarycenter.X() - 0.002)*w, (m_TrackerBarycenter.Y() - 0.002)*h);
                        glVertex2f((m_TrackerBarycenter.X() - 0.002)*w, (m_TrackerBarycenter.Y() + 0.002)*h);
                        glVertex2f((m_TrackerBarycenter.X() + 0.002)*w, (m_TrackerBarycenter.Y() + 0.002)*h);
                        glVertex2f((m_TrackerBarycenter.X() + 0.002)*w, (m_TrackerBarycenter.Y() - 0.002)*h);
                    glEnd();

*/

                    for (i = 0; i < m_pTrackerData->GetFeaturesCount(); i++)
                    {

                        moTrackerFeature* pF = m_pTrackerData->GetFeature(i);

                        //if (pF && pF->valid) {

                        float x = (pF->x / (float)tw) - 0.5;
                        float y = -(pF->y / (float)th) + 0.5;
                        float tr_x = (pF->tr_x / (float)tw) - 0.5;
                        float tr_y = -(pF->tr_y / (float)th) + 0.5;
                        float v_x = (pF->v_x / (float)tw);
                        float v_y = -(pF->v_y / (float)th);
                        float vel = sqrtf( v_x*v_x+v_y*v_y );
                        int v = pF->val;



                        //MODebug2->Log(moText("    x:")+FloatToStr(pF->x) + moText(" y:")+FloatToStr(pF->y) );
/*
                        glBindTexture(GL_TEXTURE_2D,0);
                        glColor4f(1.0, 0.0, 0.0, 1.0);

                        if (v >= 0) glColor4f(0.0, 1.0, 0.0, 1.0);
                        else if (v == -1) glColor4f(1.0, 0.0, 1.0, 1.0);
                        else if (v == -2) glColor4f(1.0, 0.0, 1.0, 1.0);
                        else if (v == -3) glColor4f(1.0, 0.0, 1.0, 1.0);
                        else if (v == -4) glColor4f(1.0, 0.0, 1.0, 1.0);
                        else if (v == -5) glColor4f(1.0, 0.0, 1.0, 1.0);
*/

                        if ( pF->valid ) {
                            has_features = true;

                            if (drawing_features > 1 ) {
/*
                                glPointSize((GLfloat)2);
                                glLineWidth((GLfloat)2.0);



                                glBegin(GL_QUADS);
                                    glVertex2f((tr_x - 0.008)*normalf, (tr_y - 0.008)*normalf);
                                    glVertex2f((tr_x - 0.008)*normalf, (tr_y + 0.008)*normalf);
                                    glVertex2f((tr_x + 0.008)*normalf, (tr_y + 0.008)*normalf);
                                    glVertex2f((tr_x + 0.008)*normalf, (tr_y - 0.008)*normalf);
                                glEnd();

                                glBegin(GL_QUADS);
                                    glVertex2f((x - 0.008)*normalf, (y - 0.008)*normalf);
                                    glVertex2f((x - 0.008)*normalf, (y + 0.008)*normalf);
                                    glVertex2f((x + 0.008)*normalf, (y + 0.008)*normalf);
                                    glVertex2f((x + 0.008)*normalf, (y - 0.008)*normalf);
                                glEnd();

                                glDisable(GL_TEXTURE_2D);
                                glColor4f(1.0, 1.0, 1.0, 1.0);
                                glBegin(GL_LINES);
                                    glVertex2f( x*normalf, y*normalf);
                                    glVertex2f( tr_x*normalf, tr_y*normalf);
                                glEnd();
 */                           }

                            if ( vel > 0.01 && vel < 0.1) {

                                has_motion = true;

                                if (drawing_features > 0 ) {
  /*                                  glDisable(GL_TEXTURE_2D);
                                    glColor4f(0.0, 0.0, 1.0, 1.0);
                                    //glPointSize((GLfloat)10);
                                    glLineWidth((GLfloat)10.0);

                                    glBegin(GL_LINES);
                                        glVertex2f( x*normalf, y*normalf);
                                        glVertex2f( (x+v_x)*normalf, (y+v_y)*normalf);
                                    glEnd();
 */                               }

                                ///ParticlesSimpleInfluence( x*normalf, y*normalf, (x+v_x)*normalf, (y+v_y)*normalf, vel*normalf );
                            }


                        }


                    }

                    ///post process:
                    ///add threshold??? some number of frames for really check...
                    if (has_motion) {

                        if (!MotionActivity.Started()) {
                            MotionActivity.Start();
                        }

                    } else {
                        MotionActivity.Stop();
                    }

                    if (has_features) {

                        if (!FeatureActivity.Started()) {
                            FeatureActivity.Start();
                        }

                    } else {
                        FeatureActivity.Stop();
                    }


                }

			}
		}
	}


}
