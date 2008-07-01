/*******************************************************************************

                             moEffectLiveDrawing2.h

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

#include "moPlugin.h"
#include "moMathFunction.h"
#include "moScript.h"

#ifndef __MO_EFFECT_LIVE_DRAWING_H__
#define __MO_EFFECT_LIVE_DRAWING_H__

enum moLiveDraw2ParamIndex {
	LIVEDRAW2_BACK_TEXTURES,
	LIVEDRAW2_ICO_TEXTURES,
	LIVEDRAW2_SHADERS,
	LIVEDRAW2_SCRIPTS,
	LIVEDRAW2_CANVAS_MARGIN,
	LIVEDRAW2_MIN_PRESSURE,
	LIVEDRAW2_MAX_PRESSURE,
	LIVEDRAW2_MAX_THICKNESS,
	LIVEDRAW2_MAX_LINE_LENGTH,
	LIVEDRAW2_MIN_LINE_LENGTH,
	LIVEDRAW2_FLOW_VELOCITY,
	LIVEDRAW2_FLOW_PERIOD,
	LIVEDRAW2_ROTOSKETCH_DURATION,
	LIVEDRAW2_ROTOSKETCH_START,
	LIVEDRAW2_DISSOLVE_START,
	LIVEDRAW2_DISSOLVE_TIME,
	LIVEDRAW2_PEN_COLOR,
	LIVEDRAW2_COLOR_SENSIBILITY,
	LIVEDRAW2_BLENDING,
	LIVEDRAW2_TRANSLATEX,
	LIVEDRAW2_TRANSLATEY,
	LIVEDRAW2_TRANSLATEZ,
	LIVEDRAW2_ROTATEX,
	LIVEDRAW2_ROTATEY,
	LIVEDRAW2_ROTATEZ,
	LIVEDRAW2_SCALEX,
	LIVEDRAW2_SCALEY,
	LIVEDRAW2_SCALEZ
};

#define MO_DRAW_TRANSLATE_X		0
#define MO_DRAW_TRANSLATE_Y		1
#define MO_DRAW_TRANSLATE_Z		2
#define MO_DRAW_PRESSURE		3
#define MO_DRAW_AZIMUTH			4
#define MO_DRAW_TILT			5
#define MO_DRAW_TWIST			6
#define MO_DRAW_CURSOR			7
#define MO_DRAW_BUTTON			8
#define MO_KEY_PRESS_1			9
#define MO_KEY_PRESS_2			10
#define MO_KEY_PRESS_3			11
#define MO_KEY_PRESS_4			12
#define MO_KEY_PRESS_5			13
#define MO_KEY_PRESS_6			14
#define MO_KEY_PRESS_7			15
#define MO_KEY_PRESS_8			16
#define MO_KEY_PRESS_9			17
#define MO_KEY_PRESS_0			18
#define MO_KEY_PRESS_I			19
#define MO_KEY_UP				20
#define MO_KEY_DOWN				21
#define MO_KEY_LEFT				22
#define MO_KEY_RIGHT			23
#define MO_MOVE_MOUSE_X			24
#define MO_MOVE_MOUSE_Y			25
#define MO_PRESS_MOUSE_LEFT		26
#define MO_DRAW_COLOR_X			27
#define MO_DRAW_COLOR_Y			28
#define MO_DRAW_COLOR_Z			29
#define MO_KEY_PRESS_X			30
#define MO_KEY_PRESS_C			31
#define MO_KEY_PRESS_Z			32
#define MO_KEY_PRESS_R			33
#define MO_KEY_PRESS_A			34
#define MO_KEY_PRESS_V			35
#define MO_KEY_PRESS_B			36
#define MO_KEY_PRESS_N			37
#define MO_KEY_PRESS_S			38
#define MO_KEY_PRESS_M			39
#define MO_KEY_PRESS_Q			40
#define MO_KEY_PRESS_W			41
#define MO_KEY_PRESS_BACKSPACE	42
#define MO_KEY_PRESS_DELETE		43
#define MO_KEY_PRESS_HOME		44
#define MO_KEY_PRESS_END		45
#define MO_KEY_PRESS_PAGEUP		46
#define MO_KEY_PRESS_PAGEDOWN	47
#define MO_KEY_PRESS_RSHIFT		48
#define MO_KEY_PRESS_LSHIFT		49
#define MO_KEY_PRESS_RCTRL		50
#define MO_KEY_PRESS_LCTRL		51
#define MO_KEY_PRESS_F1			52
#define MO_KEY_PRESS_F2			53

#define MO_TABLET_CURSOR_UNDEFINED	0
#define MO_TABLET_CURSOR_PEN		1
#define MO_TABLET_CURSOR_ERASER		2

class Vec3f
{
public:
	Vec3f() { set(0,0,0); next = NULL; }
	Vec3f(float ix, float iy, float ip) { set(ix, iy, ip); next = NULL; }
	void set(float ix, float iy, float ip) { x = ix; y = iy; p = ip; }
	void set(float ir, float ig, float ib, float ia) { r = ir; g = ig; b = ib; a = ia; }

	float x;
	float y;
	float p; // pressure
	float r; // red
	float g; // green
	float b; // blue
	float a; // alpha


	Vec3f* next;
};

class Poly2f
{
public:
	Poly2f(int iw, int ih)
	{
		red0 = green0 = blue0 = 1.0;
		red1 = green1 = blue1 = 1.0;
		alpha0 = alpha1 = 1.0;
		press = 0.0;
		next = NULL;
		w = iw;
		h = ih;
	}

	int w;
	int h;
	float alpha0, alpha1, press;
	float red0, green0, blue0;
	float red1, green1, blue1;
	float xpoints[4];
	float ypoints[4];

	Poly2f* next;

	void SetColor0(float r, float g, float b, float a);
	void SetColor0(float r, float g, float b);
	void SetAlpha0(float a);
	void SetPressure(float p);

	void SetColor1(float r, float g, float b, float a);
	void SetColor1(float r, float g, float b);
	void SetAlpha1(float a);

	void DrawIcon(float tintr, float tintg, float tintb, float alpha, int icow, int icoh, float icof);
	void Draw(float tintr, float tintg, float tintb, float alpha, bool stretch,
				int line_shader_point0, int line_shader_point1, int line_shader_point2,
				int line_shader_quad_width, int line_shader_pressure);
	void SetVertexAttrib(float r, float g, float b, float a,
						 float x0, float y0, float x1, float y1, float x2, float y2,
						 float l, float p,
						 int line_shader_point0, int line_shader_point1, int line_shader_point2,
						 int line_shader_quad_width, int line_shader_pressure);
	void Displace(float vel, int period);
};

class Gesture
{
public:
	 Gesture(int iw, int ih, int it, bool imod, bool stex, bool rico, moData *pdatatexture, int t, moEffectState* s,
				int rduration, int rstart, int dtime, int dstart,
				moShaderGLSL* shader,
				int shader_tex_unit, int shader_tex_offset, int shader_tempo_angle,
				int shader_point0, int shader_point1, int shader_point2,
				int shader_quad_width, int shader_pressure)
	 {
		 nPoints = nPolys = 0; w = iw; h = ih; thickness = it;
		 lastPoint = points = NULL;
		 polygons = lastPoly = NULL;
		 next = NULL;
		 compileTime = -1;
		 rotsketchIndex = -1;
		 dissolve_start = 1000;
		 dissolve_time = 5000;
		 icon_mode = imod;
		 stretch_tex = stex;
		 repeat_icon = rico;
		 datatexture = pdatatexture;
		 tex = t;
		 state = s;
		 rotosketch_duration = rduration;
		 rotosketch_start = rstart;
		 dissolve_time = dtime;
		 dissolve_start = dstart;

		 line_shader = shader;
		 line_shader_tex_unit = shader_tex_unit;
		 line_shader_tex_offset = shader_tex_offset;
		 line_shader_tempo_angle = shader_tempo_angle;

		 line_shader_point0 = shader_point0;
		 line_shader_point1 = shader_point1;
		 line_shader_point2 = shader_point2;
		 line_shader_quad_width = shader_quad_width;
		 line_shader_pressure = shader_pressure;
	 }
	 ~Gesture() { clearPoints(); clearPolys(); }

	Vec3f* points;
	Vec3f* lastPoint;
	Poly2f* polygons;
	Poly2f* lastPoly;

	Gesture* next;

	int rotsketchIndex;
	int lastRotosketchUpdateTime;
	int compileTime;
	bool icon_mode;
	bool stretch_tex;
	bool repeat_icon;
	int tex;
	moData* datatexture;
	moEffectState* state;
	int rotosketch_duration, rotosketch_start;
	int dissolve_time, dissolve_start;

	moShaderGLSL* line_shader;
	int line_shader_tex_unit, line_shader_tex_offset, line_shader_tempo_angle;
	int line_shader_point0, line_shader_point1, line_shader_point2;
	int line_shader_quad_width, line_shader_pressure;

	int w;
	int h;
	float thickness;

	int nPoints;
	int nPolys;

	void AddPoint(float x, float y, float p, float* color);
	void AddPoly();
	void displacePolygons(float vel, int period);
	void RenderPolygons();
	void RotosketchUpdate();

	void clearPoints();
	void clearPolys();
	void compile();
	float distToLast(float ix, float iy);
	void smooth();
};

class moEffectLiveDrawing2 : public moEffect
{
public:
    moEffectLiveDrawing2();
    virtual ~moEffectLiveDrawing2();

    MOboolean Init();
	MOboolean Finish();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
    void Interaction( moIODeviceManager * );
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    // Public script functions.
	int ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber);
	void HandleReturns(moLuaVirtualMachine& vm, const char *strFunc);
protected:
	int dissolve_start, dissolve_time;
	int rotosketch_start;
	int rotosketch_duration;
	int canvas_margin;
	int min_pressure;
	int max_pressure;
	float max_thickness;
	int blending;
	float color_sens;
    float flow_velocity0, flow_velocity, flow_velocity_bak;
	int flow_period;
	float pen_color_rgb[4];
	float pen_color_hsl[3];
	float max_line_length;
	float min_line_length;
    moTextureIndex backTextures;
	moTextureIndex icoTextures;

	MOboolean icon_mode;
	MOboolean stretch_tex;
	MOboolean repeat_icon;

	MOint sel_tex;
	MOint tex_tab;

	MOint line_shader;
	MOint line_shader_tex_unit;
	MOint line_shader_tex_offset;
	MOint line_shader_tempo_angle;
	MOint line_shader_point0;
	MOint line_shader_point1;
	MOint line_shader_point2;
	MOint line_shader_quad_width;
	MOint line_shader_pressure;
	int canvasWidth, canvasHeight;

	MOboolean leftClicked;
	MOboolean tabletDetected;

	float penX, penY, penX0, penY0;
	float pressure;
	bool drawing, resumed_drawing;
	int pentip;

	bool show_pen, show_colorbx, show_timerbx;

	bool scripted;
	bool shaded;

    Gesture* gestures;
	Gesture* lastGesture;
	int nGestures;

	MOboolean inInteractionMode;

	void drawPen();
	void drawPalette();
	void drawTimer();
	void drawTimer(float box0, float box1, float time0, float time1);
	void drawCanvas();
	void updateFlowVelocity();
	void updateParameters();
	void updateInteractionParameters();
	void initShaders();
	void initScripts();
	void setBlendMode();
	void displaceGestures();
	void renderGestures();
	void delLastGesture();
	void clearGestures();
	void addGesture();

	// The following code is from Foley & Van Dam:  "Fundamentals of Interactive Computer Graphics".
	void UpdateHSLColor(int i, float v);
	void ConvertHSLtoRGB();
	void ConvertRGBtoHSL();
	float HuetoRGB(float m1, float m2, float h);

    // Script functions.
	void RegisterFunctions();
	int PushDebugString(moLuaVirtualMachine& vm);

	// Script variables.
	int m_iMethodBase;
	//moTextHeap m_tScriptDebug;

};

class moEffectLiveDrawing2Factory : public moEffectFactory
{
public:
    moEffectLiveDrawing2Factory() {}
    virtual ~moEffectLiveDrawing2Factory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
