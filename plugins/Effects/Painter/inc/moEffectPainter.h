/*******************************************************************************

                              moEffectPaint.h

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

  Copyright(C) 2006 Andres Colubri

  Authors:
  Andrés Colubri
  Fabricio Costa
  
*******************************************************************************/

#ifndef __MO_EFFECT_PAINTER_H__
#define __MO_EFFECT_PAINTER_H__

#include "moPlugin.h"

enum moPainterParamIndex {
	PAINTER_FILTER_SHADER,
	PAINTER_MOVEPART_SHADER,
	PAINTER_COLORPART_SHADER,
	PAINTER_RENDERGRAD2FP_SHADER,
	PAINTER_RENDERAVEGRAD_SHADER,
	PAINTER_RENDERBRUSHES_SHADER,
	PAINTER_SOURCE_TEXTURE,
	PAINTER_BRUSH_TEXTURES,
	PAINTER_RENDER_MODE,
	PAINTER_NUM_PARTICLES,
	PAINTER_BRUSH_SIZE,
	PAINTER_BRUSH_LENGTH,
	PAINTER_BRUSH_MIN_LENGHT_COEFF,
	PAINTER_BRUSH_MAX_LENGTH_COEFF,
	PAINTER_BRUSH_CHANGE_FRAC,
	PAINTER_BRUSH_CHANGE_POW,
	PAINTER_VEL_MEAN,
	PAINTER_VEL_COEFF_MIN,
	PAINTER_VEL_COEFF_MAX,
	PAINTER_STILL_TIME,
	PAINTER_CHANGE_TIME,
	PAINTER_UPDATE_NOISE_TIME,
	PAINTER_NUM_AVE_ITER,
	PAINTER_AVE_INTERVAL,
	PAINTER_FOLLOW_GRAD,
	PAINTER_NOISE_PROB,
	PAINTER_NOISE_MAG,
	PAINTER_COLOR,
	PAINTER_SYNCRO,
	PAINTER_BLENDING
};

#define MO_MOUSE_MOTION_Y_RIGHT		0
#define MO_MOUSE_MOTION_Y_LEFT		1
#define MO_MOUSE_MOTION_Y_MIDDLE	2
#define MO_BUTTON_PLUS				3
#define MO_BUTTON_MINUS				4
#define MO_KEY_PRESS_F				5
#define MO_KEY_PRESS_R				6
#define MO_KEY_PRESS_1				7
#define MO_KEY_PRESS_2				8
#define MO_KEY_PRESS_3				9
#define MO_KEY_PRESS_4				10
#define MO_KEY_PRESS_5				11
#define MO_KEY_PRESS_6				12
#define MO_KEY_PRESS_7				13
#define MO_KEY_PRESS_8				14
#define MO_KEY_PRESS_9				15
#define MO_KEY_PRESS_0				16

class moEffectPainter: public moEffect
{
public:
    moEffectPainter();
    virtual ~moEffectPainter();

    MOboolean Init();
    MOboolean Finish();
    void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
    void Interaction( moIODeviceManager * );
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );
private:
	bool painterSupported;
	bool copyScreen;
	int renderMode;

	// Parameters.
	moText sourceTexture;
	int currentBrush;
	int canvasWidth; 
    int canvasHeight;
    int boxWidth;
    int boxHeight;
	int numParticles0, numParticles;
    float brushSize;
    int brushLength;
    float brushMinLengthCoeff;
    float brushMaxLengthCoeff;
    float brushChangeFrac;
    float brushChangePow;
    float velMean;
    float velCoeffMin;
    float velCoeffMax;
    float stillTime;
    float changeTime;
    int numAveIter;
    int aveInterval;
    bool followGrad;
    float noiseProb;
    float noiseMag;
	float color[4];
	int blending;
	GLenum fptexTarget;

	// Textures.
	moTextureIndex BrushTextures;
	MOint tex2dSourceImage;
	MOint tex2dCanvas;
	MOint tex2dPaint;
	MOint tex2dImage[2];
	MOint texfpRand;
	MOint texfpVel;
	MOint texfpGrad[2];
	MOint texfpNewGrad[2];
	MOint texfpPos[2];
	MOint texfpColor[2];
	MOint texfpColorAux[2];

    int writeGradTex, readGradTex;
    int writeNewGradTex, readNewGradTex;
    int newImageTex, oldImageTex;
    int readPosTex, writePosTex, readColorTex, writeColorTex;

	MOint FBO[5];

	MOuint attachPoints[2];

	// Shaders.
	moText filterShaderFn;
	MOint filterShader;
    int filterShaderTexUnitImage, filterShaderTexScale;
	moText movePartShaderFn;
	MOint movePartShader;
	int movePartShaderTexUnitPos, movePartShaderTexUnitGrad, movePartShaderTexUnitVel, movePartShaderMaxPos, movePartShaderVelMean, movePartShaderFollowGrad, movePartShaderApplyNoise, movePartShaderNoiseMag;
	moText colorPartShaderFn;
	MOint colorPartShader;
	int colorPartShaderTexUnitImage, colorPartShaderTexUnitNewImage, colorPartShaderTexUnitColor, colorPartShaderTexUnitColorAux, colorPartShaderBrushLength, colorPartShaderNewImageCoeff, colorPartShaderColorChgFrac, colorPartShaderColorChgPow;
	moText renderGrad2fpShaderFn;
	MOint renderGrad2fpShader;
    int renderGrad2fpShaderTexUnitImage, renderGrad2fpShaderTexUnitRandom, renderGrad2fpShaderTexScale;
	moText renderAveGradShaderFn;
	MOint renderAveGradShader;
    int renderAveGradShaderTexUnitGrad, renderAveGradShaderTexUnitRandom, renderAveGradShaderTexUnitNewGrad, renderAveGradShaderTexScale, renderAveGradShaderNewGradCoeff;
	moText renderBrushesShaderFn;
	MOint renderBrushesShader;
    int renderBrushesShaderTexUnitBrush, renderBrushesShaderTexUnitGrad, renderBrushesShaderTexUnitColor;

	// Array of positions.
	GLfloat *posArray;

    // Time.
    float currentTime, lastChangeTime, changeCoeff;
    bool swapedImageTex;
    int aveCount;
    int startClock;	
	
    void initRandom();
	void initParameters();
	void initTex();
	void addTextureToFBO(MOuint fbo, MOuint tex);
	void initFBOs();
	void initFPTex();
	void init2DTex();
	void initPosTex();
	void initAuxTex();
	void initVelTex();
	void initRandTex();
	void initColorTex();
	void initGradTex();	
	void initShaders();
	void initParticles();
	void finishParticles();
	void initClock();

	float random(float a, float b);
	void updateVel();
	void updateBrushes();
	void drawBrushes();
	void drawToScreen();
	void updateClock();

	void setBoxView();
	void moveParticles();
	void colorParticles();
	void setCanvasView();
	void setBlendMode();
	void setGLState();

	void applyFilter();
	void renderImageQuad();
	void renderGradQuad();

	void swapPosTex();
	void swapColorTex();
	void swapImageTex();
	void swapGradTex();

	void loadInitParameters();
	void updateParameters();
	void updateInteractionParameters();
};

class moEffectPainterFactory : public moEffectFactory {

public:
    moEffectPainterFactory() {}
    virtual ~moEffectPainterFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
