/*******************************************************************************

                               moEffectPainter.cpp

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

  Copyright(C) 2006 Andres Colubri

  Authors:
  Andrés Colubri
  Fabricio Costa

*******************************************************************************/

#include "moEffectPainter.h"

//========================
//  Factory
//========================

moEffectPainterFactory *m_EffectIconoFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if (m_EffectIconoFactory==NULL)
		m_EffectIconoFactory = new moEffectPainterFactory();
	return (moEffectFactory*) m_EffectIconoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectIconoFactory;
	m_EffectIconoFactory = NULL;
}

moEffect*  moEffectPainterFactory::Create() {
	return new moEffectPainter();
}

void moEffectPainterFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectPainter::moEffectPainter()
{
	SetName("painter");
	posArray = NULL;
}

moEffectPainter::~moEffectPainter()
{
	Finish();
}
MOboolean moEffectPainter::Init()
{
    if (!PreInit()) return false;

	painterSupported = m_pResourceManager->GetRenderMan()->ShadersSupported() &&
						m_pResourceManager->GetRenderMan()->FramebufferObjectSupported() &&
						m_pResourceManager->GetRenderMan()->MultiTextureSupported();
	if (!painterSupported)
	{
		MODebug->Push("Error: painter cannot run on this machine");
		return false;
	}

    initRandom();
	initParameters();
	initTex();
	initFBOs();
	init2DTex();
	initFPTex();
	initShaders();
	initParticles();
	initClock();

	return true;
}

void moEffectPainter::Draw( moTempo* tempogral, moEffectState* parentstate)
{
    PreDraw(tempogral, parentstate);

	if (!painterSupported) return;

	setGLState();
	updateParameters();
	updateVel();
	updateBrushes();
	drawBrushes();
	if (renderMode) drawToScreen();

	updateClock();
}

MOboolean moEffectPainter::Finish()
{
    PreFinish();
	BrushTextures.Finish();
	finishParticles();
    return PreFinish();
}

void moEffectPainter::Interaction( moIODeviceManager *IODeviceManager )
{
	moDeviceCode *temp;
	MOint did,cid,state,valor;
	MOfloat f, d;

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
				case MO_MOUSE_MOTION_Y_RIGHT:
					f = (0 != valor) ? float(valor) / abs(valor) : 0.0;
					d = -1.0 * f;
					brushLength = momax(0.0, momin(brushLength + d, 100));
					break;
				case MO_MOUSE_MOTION_Y_LEFT:
					f = (0 != valor) ? float(valor) / abs(valor) : 0.0;
					d = -0.2 * f;
					noiseMag = momax(0.0, momin(noiseMag + d, 10.0));
					break;
				case MO_MOUSE_MOTION_Y_MIDDLE:
					f = (0 != valor) ? float(valor) / abs(valor) : 0.0;
					d = -1.0 * f;
					brushSize = momax(0.0, momin(brushSize + d, 100.0));
					break;
				case MO_BUTTON_PLUS:
					velMean = momin(velMean + 0.2, 20.0);
					break;
				case MO_BUTTON_MINUS:
					velMean = momax(0.0, velMean - 0.2);
					break;
				case MO_KEY_PRESS_F:
					followGrad = !followGrad;
					break;
				case MO_KEY_PRESS_R:
					updateParameters();
					break;
				case MO_KEY_PRESS_1:
					currentBrush = 0;
					break;
				case MO_KEY_PRESS_2:
					currentBrush = 1;
					break;
				case MO_KEY_PRESS_3:
					currentBrush = 2;
					break;
				case MO_KEY_PRESS_4:
					currentBrush = 3;
					break;
				case MO_KEY_PRESS_5:
					currentBrush = 4;
					break;
				case MO_KEY_PRESS_6:
					currentBrush = 5;
					break;
				case MO_KEY_PRESS_7:
					currentBrush = 6;
					break;
				case MO_KEY_PRESS_8:
					currentBrush = 7;
					break;
				case MO_KEY_PRESS_9:
					currentBrush = 8;
					break;
				case MO_KEY_PRESS_0:
					currentBrush = 9;
					break;
			}
		temp = temp->next;
		}
	}
}

void moEffectPainter::initRandom()
{
    srand(time(NULL));
}

void moEffectPainter::initParameters()
{
	moDefineParamIndex( PAINTER_FILTER_SHADER, moText("filterShader") );
	moDefineParamIndex( PAINTER_MOVEPART_SHADER, moText("movePartShader") );
	moDefineParamIndex( PAINTER_COLORPART_SHADER, moText("colorPartShader") );
	moDefineParamIndex( PAINTER_RENDERGRAD2FP_SHADER, moText("renderGrad2fpShader") );
	moDefineParamIndex( PAINTER_RENDERAVEGRAD_SHADER, moText("renderAveGradShader") );
	moDefineParamIndex( PAINTER_RENDERBRUSHES_SHADER, moText("renderBrushesShader") );
	moDefineParamIndex( PAINTER_SOURCE_TEXTURE, moText("sourceTexture") );
	moDefineParamIndex( PAINTER_BRUSH_TEXTURES, moText("brushTextures") );
	moDefineParamIndex( PAINTER_RENDER_MODE, moText("renderMode") );
	moDefineParamIndex( PAINTER_NUM_PARTICLES, moText("numParticles") );
	moDefineParamIndex( PAINTER_BRUSH_SIZE, moText("brushSize") );
	moDefineParamIndex( PAINTER_BRUSH_LENGTH, moText("brushLength") );
	moDefineParamIndex( PAINTER_BRUSH_MIN_LENGHT_COEFF, moText("brushMinLengthCoeff") );
	moDefineParamIndex( PAINTER_BRUSH_MAX_LENGTH_COEFF, moText("brushMaxLengthCoeff") );
	moDefineParamIndex( PAINTER_BRUSH_CHANGE_FRAC, moText("brushChangeFrac") );
	moDefineParamIndex( PAINTER_BRUSH_CHANGE_POW, moText("brushChangePow") );
	moDefineParamIndex( PAINTER_VEL_MEAN, moText("velMean") );
	moDefineParamIndex( PAINTER_VEL_COEFF_MIN, moText("velCoeffMin") );
	moDefineParamIndex( PAINTER_VEL_COEFF_MAX, moText("velCoeffMax") );
	moDefineParamIndex( PAINTER_STILL_TIME, moText("stillTime") );
	moDefineParamIndex( PAINTER_CHANGE_TIME, moText("changeTime") );
	moDefineParamIndex( PAINTER_UPDATE_NOISE_TIME, moText("updateNoiseTime") );
	moDefineParamIndex( PAINTER_NUM_AVE_ITER, moText("numAveIter") );
	moDefineParamIndex( PAINTER_AVE_INTERVAL, moText("aveInterval") );
	moDefineParamIndex( PAINTER_FOLLOW_GRAD, moText("followGrad") );
	moDefineParamIndex( PAINTER_NOISE_PROB, moText("noiseProb") );
	moDefineParamIndex( PAINTER_NOISE_MAG, moText("noiseMag") );
	moDefineParamIndex( PAINTER_COLOR, moText("color") );
	moDefineParamIndex( PAINTER_BLENDING, moText("blending") );

	loadInitParameters();

	// Canvas size.
    canvasWidth = m_pResourceManager->GetRenderMan()->RenderWidth();
    canvasHeight = m_pResourceManager->GetRenderMan()->RenderHeight();

	// Calculating number of particles and size of particle box.
	// Generating a power-of-two box width closest to int(sqrt(numParticles0)).
	for (boxWidth = 2; boxWidth < int(sqrt((float)numParticles0)); boxWidth = boxWidth * 2);
	int n0 = boxWidth * boxWidth;
	int n1 = boxWidth * boxWidth / 2;
	if (abs(n0 - numParticles0) < abs(n1 - numParticles0)) boxHeight = boxWidth;
	else boxHeight = boxWidth / 2;
	numParticles = boxWidth * boxHeight;

	updateParameters();
}

void moEffectPainter::initTex()
{
	fptexTarget = GL_TEXTURE_2D;

	copyScreen =  ( sourceTexture == moText("screen_texture"));
	tex2dSourceImage = m_pResourceManager->GetTextureMan()->GetTextureMOId(sourceTexture, false);

	moTexParam FPTexParams = m_pResourceManager->GetGLMan()->BuildFPTexParam(true, 4);
	FPTexParams.target = fptexTarget;
	FPTexParams.internal_format = GL_RGBA16F_ARB;

	BrushTextures.MODebug = MODebug;
	BrushTextures.Init(GetConfig(), moParamReference(PAINTER_BRUSH_TEXTURES), m_pResourceManager->GetTextureMan() );
	currentBrush = 0;

	tex2dCanvas = m_pResourceManager->GetTextureMan()->AddTexture("painter_tex2dCanvas", canvasWidth, canvasHeight, MODefTex2DParams);
	tex2dPaint = m_pResourceManager->GetTextureMan()->AddTexture("painter_tex2dPaint", canvasWidth, canvasHeight, MODefTex2DParams);

	tex2dImage[0] = m_pResourceManager->GetTextureMan()->AddTexture("painter_tex2dImage0", canvasWidth, canvasHeight, MODefTex2DParams);
	tex2dImage[1] = m_pResourceManager->GetTextureMan()->AddTexture("painter_tex2dImage1", canvasWidth, canvasHeight, MODefTex2DParams);

	texfpRand = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpRand", canvasWidth, canvasHeight, FPTexParams);
	texfpVel = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpVel", boxWidth, boxHeight, FPTexParams);

	texfpGrad[0] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpGrad0", canvasWidth, canvasHeight, FPTexParams);
	texfpGrad[1] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpGrad1", canvasWidth, canvasHeight, FPTexParams);

	texfpNewGrad[0] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpNewGrad0", canvasWidth, canvasHeight, FPTexParams);
	texfpNewGrad[1] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpNewGrad1", canvasWidth, canvasHeight, FPTexParams);

	texfpPos[0] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpPos0", boxWidth, boxHeight, FPTexParams);
	texfpPos[1] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpPos1", boxWidth, boxHeight, FPTexParams);

	texfpColor[0] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpColor0", boxWidth, boxHeight, FPTexParams);
	texfpColor[1] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpColor1", boxWidth, boxHeight, FPTexParams);

	texfpColorAux[0] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpColorAux0", boxWidth, boxHeight, FPTexParams);
	texfpColorAux[1] = m_pResourceManager->GetTextureMan()->AddTexture("painter_texfpColorAux1", boxWidth, boxHeight, FPTexParams);

	// Setting ping-pong variables.
	newImageTex = 0;
	oldImageTex = 1;
	readGradTex = 0;
	writeGradTex = 1;
	readPosTex = 0;
	writePosTex = 1;
	readColorTex = 0;
	writeColorTex = 1;
}

void moEffectPainter::addTextureToFBO(MOuint fbo, MOuint tex)
{
	MOuint attach_point;
	m_pResourceManager->GetFBMan()->GetFBO(fbo)->AddTexture(canvasWidth, canvasHeight,
									m_pResourceManager->GetTextureMan()->GetTexParam(tex),
									m_pResourceManager->GetTextureMan()->GetGLId(tex), attach_point);
	m_pResourceManager->GetTextureMan()->GetTexture(tex)->SetFBO(m_pResourceManager->GetFBMan()->GetFBO(fbo));
	m_pResourceManager->GetTextureMan()->GetTexture(tex)->SetFBOAttachPoint(attach_point);
}

void moEffectPainter::initFBOs()
{
	FBO[0] = m_pResourceManager->GetFBMan()->CreateFBO();
	addTextureToFBO(FBO[0], texfpGrad[0]);
	addTextureToFBO(FBO[0], texfpGrad[1]);
	addTextureToFBO(FBO[0], texfpNewGrad[0]);
	addTextureToFBO(FBO[0], texfpNewGrad[1]);

	FBO[1] = m_pResourceManager->GetFBMan()->CreateFBO();
	addTextureToFBO(FBO[1], tex2dImage[0]);
	addTextureToFBO(FBO[1], tex2dImage[1]);
	addTextureToFBO(FBO[1], tex2dPaint);

	FBO[2] = m_pResourceManager->GetFBMan()->CreateFBO();
	addTextureToFBO(FBO[2], texfpPos[0]);
	addTextureToFBO(FBO[2], texfpPos[1]);
	addTextureToFBO(FBO[2], texfpVel);

	FBO[3] = m_pResourceManager->GetFBMan()->CreateFBO();
	addTextureToFBO(FBO[3], texfpColor[0]);
	addTextureToFBO(FBO[3], texfpColor[1]);
	addTextureToFBO(FBO[3], texfpColorAux[0]);
	addTextureToFBO(FBO[3], texfpColorAux[1]);

	FBO[4] = m_pResourceManager->GetFBMan()->CreateFBO();
	addTextureToFBO(FBO[4], texfpRand);
}

void moEffectPainter::initFPTex()
{
	initPosTex();
	initAuxTex();
	initVelTex();
	initRandTex();

	initColorTex();
	initGradTex();
}

void moEffectPainter::init2DTex()
{
	GLubyte *blackBuffer = new GLubyte[4 * canvasWidth * canvasHeight];
	for (int j = 0; j < canvasHeight; j++)
		for (int i = 0; i < canvasWidth; i++)
		{
			blackBuffer[i * 4 + j * canvasWidth * 4] = 0;
			blackBuffer[i * 4 + j * canvasWidth * 4 + 1] = 0;
			blackBuffer[i * 4 + j * canvasWidth * 4 + 2] = 0;
			blackBuffer[i * 4 + j * canvasWidth * 4 + 3] = 255;
		}
	m_pResourceManager->GetTextureMan()->GetTexture(tex2dCanvas)->SetBuffer(blackBuffer, GL_RGBA, GL_UNSIGNED_BYTE);
	m_pResourceManager->GetTextureMan()->GetTexture(tex2dPaint)->SetBuffer(blackBuffer, GL_RGBA, GL_UNSIGNED_BYTE);
	m_pResourceManager->GetTextureMan()->GetTexture(tex2dImage[0])->SetBuffer(blackBuffer, GL_RGBA, GL_UNSIGNED_BYTE);
	m_pResourceManager->GetTextureMan()->GetTexture(tex2dImage[1])->SetBuffer(blackBuffer, GL_RGBA, GL_UNSIGNED_BYTE);
	delete blackBuffer;
}

void moEffectPainter::initShaders()
{
	moShaderGLSL* pglsl;

	MODebug->Push("Loading filter shader...");
	filterShader = m_pResourceManager->GetShaderMan()->AddShader(filterShaderFn);
	pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(filterShader);
	pglsl->PrintFragShaderLog();
	filterShaderTexUnitImage = pglsl->GetUniformID("src_tex_unit0");
	filterShaderTexScale = pglsl->GetUniformID("src_tex_offset0");

	MODebug->Push("Loading movePart shader...");
	movePartShader = m_pResourceManager->GetShaderMan()->AddShader(movePartShaderFn);
	pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(movePartShader);
	movePartShaderTexUnitPos = pglsl->GetUniformID("tex_unit_pos");
	movePartShaderTexUnitGrad = pglsl->GetUniformID("tex_unit_grad");
	movePartShaderTexUnitVel = pglsl->GetUniformID("tex_unit_vel");
	movePartShaderMaxPos = pglsl->GetUniformID("max_pos");
	movePartShaderVelMean = pglsl->GetUniformID("vel_mean");
	movePartShaderFollowGrad = pglsl->GetUniformID("follow_grad");
	movePartShaderApplyNoise = pglsl->GetUniformID("apply_noise");
	movePartShaderNoiseMag = pglsl->GetUniformID("noise_mag");

	MODebug->Push("Loading colorPart shader...");
	colorPartShader = m_pResourceManager->GetShaderMan()->AddShader(colorPartShaderFn);
	pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(colorPartShader);
	colorPartShaderTexUnitImage = pglsl->GetUniformID("tex_unit_image");
	colorPartShaderTexUnitNewImage = pglsl->GetUniformID("tex_unit_newimage");
	colorPartShaderTexUnitColor = pglsl->GetUniformID("tex_unit_color");
	colorPartShaderTexUnitColorAux = pglsl->GetUniformID("tex_unit_coloraux");
	colorPartShaderBrushLength = pglsl->GetUniformID("brush_length");
	colorPartShaderNewImageCoeff = pglsl->GetUniformID("newimage_coeff");
	colorPartShaderColorChgFrac = pglsl->GetUniformID("colorchg_frac");
	colorPartShaderColorChgPow = pglsl->GetUniformID("colorchg_pow");

	MODebug->Push("Loading renderGrad2fp shader...");
	renderGrad2fpShader = m_pResourceManager->GetShaderMan()->AddShader(renderGrad2fpShaderFn);
	pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(renderGrad2fpShader);
	renderGrad2fpShaderTexUnitImage = pglsl->GetUniformID("tex_unit_image");
	renderGrad2fpShaderTexUnitRandom = pglsl->GetUniformID("tex_unit_random");
	renderGrad2fpShaderTexScale = pglsl->GetUniformID("tex_scale");

	MODebug->Push("Loading renderAveGrad shader...");
	renderAveGradShader = m_pResourceManager->GetShaderMan()->AddShader(renderAveGradShaderFn);
	pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(renderAveGradShader);
	renderAveGradShaderTexUnitGrad = pglsl->GetUniformID("tex_unit_grad");
	renderAveGradShaderTexUnitRandom = pglsl->GetUniformID("tex_unit_random");
	renderAveGradShaderTexUnitNewGrad = pglsl->GetUniformID("tex_unit_newgrad");
	renderAveGradShaderTexScale = pglsl->GetUniformID("tex_scale");
	renderAveGradShaderNewGradCoeff = pglsl->GetUniformID("newgrad_coeff");

	MODebug->Push("Loading renderBrushes shader...");
	renderBrushesShader = m_pResourceManager->GetShaderMan()->AddShader(renderBrushesShaderFn);
	pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(renderBrushesShader);
	renderBrushesShaderTexUnitGrad = pglsl->GetUniformID("tex_unit_grad");
	renderBrushesShaderTexUnitBrush = pglsl->GetUniformID("tex_unit_brush");
	renderBrushesShaderTexUnitColor = pglsl->GetUniformID("tex_unit_color");
}

void moEffectPainter::initParticles()
{
    posArray = new GLfloat[4 * numParticles];
}

void moEffectPainter::finishParticles()
{
	if (posArray != NULL)
	{
		delete posArray;
		posArray = NULL;
	}
}

void moEffectPainter::initClock()
{
	aveCount = 0;
	lastChangeTime = -stillTime;
	changeCoeff = -1.0;

	startClock = clock();
}

void moEffectPainter::initPosTex()
{
	GLfloat *initPosData = new GLfloat[4 * boxWidth * boxHeight];
	for (int j = 0; j < boxHeight; j++)
		for (int i = 0; i < boxWidth; i++)
		{
			initPosData[i * 4 + j * boxWidth * 4] = random(0, canvasWidth);
			initPosData[i * 4 + j * boxWidth * 4 + 1] = random(0, canvasHeight);
			initPosData[i * 4 + j * boxWidth * 4 + 2] = 0.0;
			initPosData[i * 4 + j * boxWidth * 4 + 3] = 0.0;
		}
	m_pResourceManager->GetTextureMan()->GetTexture(texfpPos[0])->SetBuffer(initPosData, GL_RGBA, GL_FLOAT);
	delete initPosData;
}

void moEffectPainter::initAuxTex()
{
	GLfloat *initAuxData = new GLfloat[4 * boxWidth * boxHeight];
	for (int j = 0; j < boxHeight; j++)
		for (int i = 0; i < boxWidth; i++)
		{
			initAuxData[i * 4 + j * boxWidth * 4] = 0.0;
			initAuxData[i * 4 + j * boxWidth * 4 + 1] = 0.0;
			initAuxData[i * 4 + j * boxWidth * 4 + 2] = 0.0;
			initAuxData[i * 4 + j * boxWidth * 4 + 3] = random(brushMinLengthCoeff, brushMaxLengthCoeff);
		}
	m_pResourceManager->GetTextureMan()->GetTexture(texfpColorAux[0])->SetBuffer(initAuxData, GL_RGBA, GL_FLOAT);
	delete initAuxData;
}

void moEffectPainter::initVelTex()
{
	GLfloat *initVelData = new GLfloat[4 * boxWidth * boxHeight];
	for (int j = 0; j < boxHeight; j++)
		for (int i = 0; i < boxWidth; i++)
		{
			initVelData[i * 4 + j * boxWidth * 4] = random(velCoeffMin, velCoeffMax);
			initVelData[i * 4 + j * boxWidth * 4 + 1] = 0.0;
			initVelData[i * 4 + j * boxWidth * 4 + 2] = 0.0;
			initVelData[i * 4 + j * boxWidth * 4 + 3] = 0.0;
		}
	m_pResourceManager->GetTextureMan()->GetTexture(texfpVel)->SetBuffer(initVelData, GL_RGBA, GL_FLOAT);
	delete initVelData;
}

void moEffectPainter::initRandTex()
{
	float a;
	GLfloat *randData = new GLfloat[4 * canvasWidth * canvasHeight];
	for (int j = 0; j < canvasHeight; j++)
		for (int i = 0; i < canvasWidth; i++)
		{
			a = random(0.0, 2 * moMathf::PI);
			randData[i * 4 + j * canvasWidth * 4] = cos(a);
			randData[i * 4 + j * canvasWidth * 4 + 1] = sin(a);
			randData[i * 4 + j * canvasWidth * 4 + 2] = 0.0;
			randData[i * 4 + j * canvasWidth * 4 + 3] = 0.0;
		}
	m_pResourceManager->GetTextureMan()->GetTexture(texfpRand)->SetBuffer(randData, GL_RGBA, GL_FLOAT);
	delete randData;
}

void moEffectPainter::initColorTex()
{
    GLfloat *initAuxData = new GLfloat[boxWidth * boxHeight * 4];
    for (int j = 0; j < boxHeight; j++)
        for (int i = 0; i < boxWidth; i++)
        {
            initAuxData[i * 4 + j * boxWidth * 4] = 0.0;
            initAuxData[i * 4 + j * boxWidth * 4 + 1] = 0.0;
            initAuxData[i * 4 + j * boxWidth * 4 + 2] = 0.0;
            initAuxData[i * 4 + j * boxWidth * 4 + 3] = 0.0;
        }

    m_pResourceManager->GetTextureMan()->GetTexture(texfpColor[0])->SetBuffer(initAuxData, GL_RGBA, GL_FLOAT);

    m_pResourceManager->GetTextureMan()->GetTexture(texfpColor[1])->SetBuffer(initAuxData, GL_RGBA, GL_FLOAT);

	delete initAuxData;
}

void moEffectPainter::initGradTex()
{
    GLfloat *initGradData = new GLfloat[4 * canvasWidth * canvasHeight];
    for (int j = 0; j < canvasHeight; j++)
        for (int i = 0; i < canvasWidth; i++)
        {
            initGradData[i * 4 + j * canvasWidth * 4] = 0.0;
            initGradData[i * 4 + j * canvasWidth * 4 + 1] = 0.0;
            initGradData[i * 4 + j * canvasWidth * 4 + 2] = 0.0;
            initGradData[i * 4 + j * canvasWidth * 4 + 3] = 0.0;
        }

    m_pResourceManager->GetTextureMan()->GetTexture(texfpGrad[0])->SetBuffer(initGradData, GL_RGBA, GL_FLOAT);
    m_pResourceManager->GetTextureMan()->GetTexture(texfpGrad[1])->SetBuffer(initGradData, GL_RGBA, GL_FLOAT);

    m_pResourceManager->GetTextureMan()->GetTexture(texfpNewGrad[0])->SetBuffer(initGradData, GL_RGBA, GL_FLOAT);
    m_pResourceManager->GetTextureMan()->GetTexture(texfpNewGrad[1])->SetBuffer(initGradData, GL_RGBA, GL_FLOAT);

    delete initGradData;
}

float moEffectPainter::random(float a, float b)
{
    return a + (b - a) * (float)rand() / (float)RAND_MAX;
}

void moEffectPainter::updateVel()
{
	velMean *= state.tempo.delta;
}

void moEffectPainter::updateBrushes()
{
	setBoxView();
	moveParticles();
	colorParticles();
	setCanvasView();

	m_pResourceManager->GetFBMan()->BindFBO(FBO[0]);

	currentTime = float(clock()) / float(CLOCKS_PER_SEC);
	if (currentTime - lastChangeTime >= stillTime)
	{
		// Start changing image...

		// Preprocessing filter is applied to the source image to generate the new image.
		if (copyScreen) m_pResourceManager->GetRenderMan()->SaveScreen();
		applyFilter();

		// The gradient of the new image is calculated and stored in the texture texfpNewGrad[0].
		renderImageQuad();

		// Initializing variables to control transition between old and new image/gradient.
		changeCoeff = 0.0;       // Linear interpolation coefficient.
		swapedImageTex = false;

		// Used to control the averaging of the gradient of the new image during the transition
		// period.
		writeNewGradTex = 1;
		readNewGradTex = 0;

		lastChangeTime = currentTime;
	}

	if ((0.0 <= changeCoeff) && (changeCoeff < 1.0))
	{
		// Updating linear interpolation coefficient.
		changeCoeff = (currentTime - lastChangeTime) / changeTime;
		if (1.0 < changeCoeff) changeCoeff = 1.0;
	}
	else if (!swapedImageTex)
	{
		// Transition period is finished.

		// ...done changing image.
		swapImageTex();
		swapedImageTex = true;
		changeCoeff = -1.0; // With this value, the shaders don't enter into the transition mode.
	}

	aveCount++;
	if (aveCount == aveInterval)
	{
		// Gradient average.
		aveCount = 0;
		for (int n = 0; n < numAveIter; n++)
		{
			renderGradQuad();
			swapGradTex();
		}
	}

	m_pResourceManager->GetFBMan()->UnbindFBO();
}

void moEffectPainter::drawToScreen()
{
    glColor4f(color[0] * state.tintr,
              color[1] * state.tintg,
              color[2] * state.tintb,
              color[3] * state.alpha);

	setBlendMode();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetTextureMan()->GetGLId(tex2dCanvas));
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, 0.0);

        glTexCoord2f(1.0, 0.0);
        glVertex2f(canvasWidth, 0.0);

        glTexCoord2f(1.0, 1.0);
        glVertex2f(canvasWidth, canvasHeight);

        glTexCoord2f(0.0, 1.0);
        glVertex2f(0.0, canvasHeight);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetTextureMan()->GetGLId(tex2dPaint));
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(0.0, 0.0);

        glTexCoord2f(1.0, 0.0);
        glVertex2f(canvasWidth, 0.0);

        glTexCoord2f(1.0, 1.0);
        glVertex2f(canvasWidth, canvasHeight);

        glTexCoord2f(0.0, 1.0);
        glVertex2f(0.0, canvasHeight);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void moEffectPainter::setBlendMode()
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
			// [Rs * momin(As,1-Ad) ] + [ Rd * Ad]
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

void moEffectPainter::setGLState()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void moEffectPainter::updateClock()
{
	if (clock() - startClock >= CLOCKS_PER_SEC) startClock = clock();
}

void moEffectPainter::setBoxView()
{
	m_pResourceManager->GetGLMan()->SetOrthographicView(boxWidth, boxHeight);
}

void moEffectPainter::setCanvasView()
{
	m_pResourceManager->GetGLMan()->SetOrthographicView(canvasWidth, canvasHeight);
}

void moEffectPainter::applyFilter()
{
	m_pResourceManager->GetFBMan()->BindFBO(FBO[1], m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(tex2dImage[newImageTex]));

	moShaderGLSL* pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(filterShader);

	pglsl->StartShader();
	glUniform1iARB(filterShaderTexUnitImage, 0);
	glUniform2fARB(filterShaderTexScale, 1.0 / float(m_pResourceManager->GetTextureMan()->GetWidth(tex2dSourceImage)), 1.0 / float(m_pResourceManager->GetTextureMan()->GetHeight(tex2dSourceImage)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetTextureMan()->GetGLId(tex2dSourceImage));
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex2f(0.0, 0.0);

		glTexCoord2f(1.0, 0.0);
		glVertex2f(canvasWidth, 0.0);

		glTexCoord2f(1.0, 1.0);
		glVertex2f(canvasWidth, canvasHeight);

		glTexCoord2f(0.0, 1.0);
		glVertex2f(0.0, canvasHeight);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	pglsl->StopShader();

	m_pResourceManager->GetFBMan()->UnbindFBO();
}

void moEffectPainter::moveParticles()
{
	int i, j;

	m_pResourceManager->GetFBMan()->BindFBO(FBO[2], m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(texfpPos[writePosTex]));

	// Update positions.
	moShaderGLSL* pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(movePartShader);
	pglsl->StartShader();
	glUniform1iARB(movePartShaderTexUnitPos, 0);
	glUniform1iARB(movePartShaderTexUnitGrad, 1);
	glUniform1iARB(movePartShaderTexUnitVel, 2);
	glUniform2fARB(movePartShaderMaxPos, canvasWidth, canvasHeight);
	glUniform1fARB(movePartShaderVelMean, velMean);
	if (followGrad) glUniform1iARB(movePartShaderFollowGrad, 1);
	else glUniform1iARB(movePartShaderFollowGrad, 0);
	if (random(0.0, 1.0) < noiseProb) glUniform1iARB(movePartShaderApplyNoise, 1);
	else glUniform1iARB(movePartShaderApplyNoise, 0);
	glUniform1fARB(movePartShaderNoiseMag, noiseMag);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpPos[readPosTex]));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpGrad[readGradTex]));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpVel));
	glBegin(GL_POINTS);
		for (i = 0; i < boxWidth; i++)
			for (j = 1; j <= boxHeight; j++)
			{
				glMultiTexCoord2f(GL_TEXTURE0, float(i) / boxWidth, float(j - 1) / boxHeight);
				glMultiTexCoord2f(GL_TEXTURE1, random(-1.0, 1.0), random(-1.0, 1.0));
				glVertex2f(i, j);
			}
	glEnd();
	glBindTexture(fptexTarget, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(fptexTarget, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(fptexTarget, 0);

	pglsl->StopShader();

	swapPosTex();

	// Read particle_pos from texture texfp_pos.
	m_pResourceManager->GetFBMan()->GetFBO(FBO[2])->SetReadTexture(m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(texfpPos[readPosTex]));
    glReadPixels(0, 0, boxWidth, boxHeight, GL_RGBA, GL_FLOAT, posArray);

	m_pResourceManager->GetFBMan()->UnbindFBO();
}

void moEffectPainter::colorParticles()
{
	float x, y, u, v;
	int i, j;

	attachPoints[0] = m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(texfpColor[writeColorTex]);
	attachPoints[1] = m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(texfpColorAux[writeColorTex]);
	m_pResourceManager->GetFBMan()->BindFBO(FBO[3], 2, attachPoints);

	moShaderGLSL* pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(colorPartShader);
	pglsl->StartShader();

	glUniform1iARB(colorPartShaderTexUnitImage, 0);
	glUniform1iARB(colorPartShaderTexUnitNewImage, 1);
	glUniform1iARB(colorPartShaderTexUnitColor, 2);
	glUniform1iARB(colorPartShaderTexUnitColorAux, 3);
	glUniform1iARB(colorPartShaderBrushLength, brushLength);
	glUniform1fARB(colorPartShaderNewImageCoeff, changeCoeff);
	glUniform1fARB(colorPartShaderColorChgFrac, brushChangeFrac);
	glUniform1fARB(colorPartShaderColorChgPow, brushChangePow);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetTextureMan()->GetGLId(tex2dImage[oldImageTex]));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetTextureMan()->GetGLId(tex2dImage[newImageTex]));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpColor[readColorTex]));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpColorAux[readColorTex]));
	glBegin(GL_POINTS);
		for (i = 0; i < boxWidth; i++)
			for (j = 0; j < boxHeight; j++)
			{
				x = posArray[i * 4 + j * boxWidth * 4];
				y = posArray[i * 4 + j * boxWidth * 4 + 1];

				u = x / float(canvasWidth);
				v = y / float(canvasHeight);
				glMultiTexCoord2f(GL_TEXTURE0, u, v);
				glMultiTexCoord2f(GL_TEXTURE1, u, v);
				glMultiTexCoord2f(GL_TEXTURE2, float(i) / boxWidth, float(j) / boxHeight);
				glMultiTexCoord2f(GL_TEXTURE3, float(i) / boxWidth, float(j) / boxHeight);
				glVertex2f(i, j + 1);
			}
	glEnd();
	glBindTexture(fptexTarget, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(fptexTarget, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	pglsl->StopShader();

	swapColorTex();

	m_pResourceManager->GetFBMan()->UnbindFBO();
}

void moEffectPainter::drawBrushes()
{
	int i, j;
	float x, y;

	m_pResourceManager->GetFBMan()->BindFBO(FBO[1], m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(tex2dPaint));

	moShaderGLSL* pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(renderBrushesShader);
	pglsl->StartShader();

	glUniform1iARB(renderBrushesShaderTexUnitGrad, 0);
	glUniform1iARB(renderBrushesShaderTexUnitBrush, 1);
	glUniform1iARB(renderBrushesShaderTexUnitColor, 2);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpGrad[readGradTex]));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, BrushTextures.GetGLId(currentBrush));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpColor[readColorTex]));

	// Rendering brushes.
	float halfBrushSize = 0.5 * brushSize;
	for (i = 0; i < boxWidth; i++)
		for (j = 0; j < boxHeight; j++)
		{
			x = posArray[i * 4 + j * boxWidth * 4];
			y = posArray[i * 4 + j * boxWidth * 4 + 1];
			glBegin(GL_TRIANGLE_STRIP); // Build Quad From A Triangle Strip
				glMultiTexCoord2f(GL_TEXTURE0, x / canvasWidth, y / canvasHeight);
				glMultiTexCoord2f(GL_TEXTURE1, 1, 1);
				glMultiTexCoord2f(GL_TEXTURE2, float(i) / boxWidth, float(j) / boxHeight);
				glVertex2f(x + halfBrushSize, y + halfBrushSize); // Top Right

				glMultiTexCoord2f(GL_TEXTURE0, x / canvasWidth, y / canvasHeight);
				glMultiTexCoord2f(GL_TEXTURE1, 0, 1);
				glMultiTexCoord2f(GL_TEXTURE2, float(i) / boxWidth, float(j) / boxHeight);
				glVertex2f(x - halfBrushSize, y + halfBrushSize); // Top Left

				glMultiTexCoord2f(GL_TEXTURE0, x / canvasWidth, y / canvasHeight);
				glMultiTexCoord2f(GL_TEXTURE1, 1, 0);
				glMultiTexCoord2f(GL_TEXTURE2, float(i) / boxWidth, float(j) / boxHeight);
				glVertex2f(x + halfBrushSize, y - halfBrushSize); // Bottom Right

				glMultiTexCoord2f(GL_TEXTURE0, x / canvasWidth, y / canvasHeight);
				glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
				glMultiTexCoord2f(GL_TEXTURE2, float(i) / boxWidth, float(j) / boxHeight);
				glVertex2f(x - halfBrushSize, y - halfBrushSize); // Bottom Left
			glEnd(); // Done Building Triangle Strip
		}

	glBindTexture(fptexTarget, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(fptexTarget, 0);

	glDisable(GL_BLEND);

	pglsl->StopShader();

	m_pResourceManager->GetFBMan()->UnbindFBO();
}

void moEffectPainter::renderImageQuad()
{
	m_pResourceManager->GetFBMan()->GetFBO(FBO[0])->SetDrawTexture(m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(texfpNewGrad[0]));

	moShaderGLSL* pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(renderGrad2fpShader);
	pglsl->StartShader();

	glUniform1iARB(renderGrad2fpShaderTexUnitImage, 0);
	glUniform1iARB(renderGrad2fpShaderTexUnitRandom, 1);
	glUniform2fARB(renderGrad2fpShaderTexScale, 1.0 / float(canvasWidth), 1.0 / float(canvasHeight));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetTextureMan()->GetGLId(tex2dSourceImage) /*m_pResourceManager->GetTextureMan()->GetGLId(tex2dImage[newImageTex])*/);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpRand));
	glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
		glVertex2f(0.0, 0.0);

		glMultiTexCoord2f(GL_TEXTURE0, 1.0, 0.0);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
		glVertex2f(canvasWidth, 0.0);

		glMultiTexCoord2f(GL_TEXTURE0, 1.0, 1.0);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0, 1.0);
		glVertex2f(canvasWidth, canvasHeight);

		glMultiTexCoord2f(GL_TEXTURE0, 0.0, 1.0);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0, 1.0);
		glVertex2f(0.0, canvasHeight);
	glEnd();
	glBindTexture(fptexTarget, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	pglsl->StopShader();
}

void moEffectPainter::renderGradQuad()
{
	if (changeCoeff == -1)
		// The average shader writes to texfpGrad[writeGradTex], using the ping-poing
		// technique to iteratively smooth the gradient field.
		m_pResourceManager->GetFBMan()->GetFBO(FBO[0])->SetDrawTexture(m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(texfpGrad[writeGradTex]));
	else
	{
		// During the image transition period, the average shader also writes to
		// the texture texfpNewGrad[writeNewGradTex], so that the gradient of the
		// the new image also gets averaged during the transition.
		attachPoints[0] = m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(texfpGrad[writeGradTex]);
		attachPoints[1] = m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(texfpNewGrad[writeNewGradTex]);
		m_pResourceManager->GetFBMan()->SetDrawTextures(2, attachPoints);
	}

	moShaderGLSL* pglsl = (moShaderGLSL*)m_pResourceManager->GetShaderMan()->GetShader(renderAveGradShader);
	pglsl->StartShader();

	glUniform1iARB(renderAveGradShaderTexUnitGrad, 0);
	glUniform1iARB(renderAveGradShaderTexUnitRandom, 1);
	glUniform1iARB(renderAveGradShaderTexUnitNewGrad, 2);
	glUniform2fARB(renderAveGradShaderTexScale, 1.0 / float(canvasWidth), 1.0 / float(canvasHeight));
	glUniform1fARB(renderAveGradShaderNewGradCoeff, changeCoeff);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpGrad[readGradTex]));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpRand));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(fptexTarget, m_pResourceManager->GetTextureMan()->GetGLId(texfpNewGrad[readNewGradTex]));
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex2f(0.0, 0.0);

		glTexCoord2f(1.0, 0.0);
		glVertex2f(canvasWidth, 0.0);

		glTexCoord2f(1.0, 1.0);
		glVertex2f(canvasWidth, canvasHeight);

		glTexCoord2f(0.0, 1.0);
		glVertex2f(0.0, canvasHeight);
	glEnd();
	glBindTexture(fptexTarget, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(fptexTarget, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(fptexTarget, 0);

	pglsl->StopShader();
}

void moEffectPainter::swapPosTex()
{
	int t = writePosTex;
	writePosTex = readPosTex;
	readPosTex = t;
}

void moEffectPainter::swapColorTex()
{
	int t = writeColorTex;
	writeColorTex = readColorTex;
	readColorTex = t;
}

void moEffectPainter::swapImageTex()
{
	int t = newImageTex;
	newImageTex = oldImageTex;
	oldImageTex = t;
}

void moEffectPainter::swapGradTex()
{
	int t = writeGradTex;
	writeGradTex = readGradTex;
	readGradTex = t;

	t = writeNewGradTex;
	writeNewGradTex = readNewGradTex;
	readNewGradTex = t;
}

moConfigDefinition *
moEffectPainter::GetDefinition( moConfigDefinition *p_configdefinition ) {
	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("filterShader"), MO_PARAM_TEXT, PAINTER_FILTER_SHADER );
	p_configdefinition->Add( moText("movePartShader"), MO_PARAM_TEXT, PAINTER_MOVEPART_SHADER );
	p_configdefinition->Add( moText("colorPartShader"), MO_PARAM_TEXT, PAINTER_COLORPART_SHADER );
	p_configdefinition->Add( moText("renderGrad2fpShader"), MO_PARAM_TEXT, PAINTER_RENDERGRAD2FP_SHADER );
	p_configdefinition->Add( moText("renderAveGradShader"), MO_PARAM_TEXT, PAINTER_RENDERAVEGRAD_SHADER );
	p_configdefinition->Add( moText("renderBrushesShader"), MO_PARAM_TEXT, PAINTER_RENDERBRUSHES_SHADER );
	p_configdefinition->Add( moText("sourceTexture"), MO_PARAM_TEXTURE, PAINTER_SOURCE_TEXTURE );
	p_configdefinition->Add( moText("brushTextures"), MO_PARAM_TEXTURE, PAINTER_BRUSH_TEXTURES );
	p_configdefinition->Add( moText("renderMode"), MO_PARAM_FUNCTION, PAINTER_RENDER_MODE );
	p_configdefinition->Add( moText("numParticles"), MO_PARAM_FUNCTION, PAINTER_NUM_PARTICLES );
	p_configdefinition->Add( moText("brushSize"), MO_PARAM_FUNCTION, PAINTER_BRUSH_SIZE );
	p_configdefinition->Add( moText("brushLength"), MO_PARAM_FUNCTION, PAINTER_BRUSH_LENGTH );
	p_configdefinition->Add( moText("brushMinLengthCoeff"), MO_PARAM_FUNCTION, PAINTER_BRUSH_MIN_LENGHT_COEFF );
	p_configdefinition->Add( moText("brushMaxLengthCoeff"), MO_PARAM_FUNCTION, PAINTER_BRUSH_MAX_LENGTH_COEFF );
	p_configdefinition->Add( moText("brushChangeFrac"), MO_PARAM_FUNCTION, PAINTER_BRUSH_CHANGE_FRAC );
	p_configdefinition->Add( moText("brushChangePow"), MO_PARAM_FUNCTION, PAINTER_BRUSH_CHANGE_POW );
	p_configdefinition->Add( moText("velMean"), MO_PARAM_FUNCTION, PAINTER_VEL_MEAN );
	p_configdefinition->Add( moText("velCoeffMin"), MO_PARAM_FUNCTION, PAINTER_VEL_COEFF_MIN );
	p_configdefinition->Add( moText("velCoeffMax"), MO_PARAM_FUNCTION, PAINTER_VEL_COEFF_MAX );
	p_configdefinition->Add( moText("stillTime"), MO_PARAM_FUNCTION, PAINTER_STILL_TIME );
	p_configdefinition->Add( moText("changeTime"), MO_PARAM_FUNCTION, PAINTER_CHANGE_TIME );
	p_configdefinition->Add( moText("updateNoiseTime"), MO_PARAM_FUNCTION, PAINTER_UPDATE_NOISE_TIME );
	p_configdefinition->Add( moText("numAveIter"), MO_PARAM_FUNCTION, PAINTER_NUM_AVE_ITER );
	p_configdefinition->Add( moText("aveInterval"), MO_PARAM_FUNCTION, PAINTER_AVE_INTERVAL );
	p_configdefinition->Add( moText("followGrad"), MO_PARAM_FUNCTION, PAINTER_FOLLOW_GRAD );
	p_configdefinition->Add( moText("noiseProb"), MO_PARAM_FUNCTION, PAINTER_NOISE_PROB );
	p_configdefinition->Add( moText("noiseMag"), MO_PARAM_FUNCTION, PAINTER_NOISE_MAG );
	p_configdefinition->Add( moText("color"), MO_PARAM_FUNCTION, PAINTER_COLOR );
	p_configdefinition->Add( moText("syncro"), MO_PARAM_FUNCTION, PAINTER_SYNCRO );
	p_configdefinition->Add( moText("blending"), MO_PARAM_FUNCTION, PAINTER_BLENDING );
	return p_configdefinition;
}

void moEffectPainter::loadInitParameters()
{
	filterShaderFn = m_Config[moParamReference(PAINTER_FILTER_SHADER)][MO_SELECTED][0].Text();
	movePartShaderFn = m_Config[moParamReference(PAINTER_MOVEPART_SHADER)][MO_SELECTED][0].Text();
	colorPartShaderFn = m_Config[moParamReference(PAINTER_COLORPART_SHADER)][MO_SELECTED][0].Text();
	renderGrad2fpShaderFn = m_Config[moParamReference(PAINTER_RENDERGRAD2FP_SHADER)][MO_SELECTED][0].Text();
	renderAveGradShaderFn = m_Config[moParamReference(PAINTER_RENDERAVEGRAD_SHADER)][MO_SELECTED][0].Text();
	renderBrushesShaderFn = m_Config[moParamReference(PAINTER_RENDERBRUSHES_SHADER)][MO_SELECTED][0].Text();

	renderMode = m_Config[moR(PAINTER_RENDER_MODE)].GetData()->Int();
	numParticles0 = m_Config[moR(PAINTER_NUM_PARTICLES)].GetData()->Int();

	sourceTexture = m_Config[moParamReference(PAINTER_SOURCE_TEXTURE)][MO_SELECTED][0].Text();
}

void moEffectPainter::updateParameters()
{
	brushSize = m_Config[moR(PAINTER_BRUSH_SIZE)].GetData()->Fun()->Eval(state.tempo.ang);
	brushLength = m_Config[moR(PAINTER_BRUSH_LENGTH)].GetData()->Fun()->Eval(state.tempo.ang);
	brushMinLengthCoeff = m_Config[moR(PAINTER_BRUSH_MIN_LENGHT_COEFF)].GetData()->Fun()->Eval(state.tempo.ang);
	brushMaxLengthCoeff = m_Config[moR(PAINTER_BRUSH_MAX_LENGTH_COEFF)].GetData()->Fun()->Eval(state.tempo.ang);
	brushChangeFrac = m_Config[moR(PAINTER_BRUSH_CHANGE_FRAC)].GetData()->Fun()->Eval(state.tempo.ang);
	brushChangePow = m_Config[moR(PAINTER_BRUSH_CHANGE_POW)].GetData()->Fun()->Eval(state.tempo.ang);
	velMean = m_Config[moR(PAINTER_VEL_MEAN)].GetData()->Fun()->Eval(state.tempo.ang);
	velCoeffMin = m_Config[moR(PAINTER_VEL_COEFF_MIN)].GetData()->Fun()->Eval(state.tempo.ang);
	velCoeffMax = m_Config[moR(PAINTER_VEL_COEFF_MAX)].GetData()->Fun()->Eval(state.tempo.ang);
	stillTime = m_Config[moR(PAINTER_STILL_TIME)].GetData()->Fun()->Eval(state.tempo.ang);
	changeTime = m_Config[moR(PAINTER_CHANGE_TIME)].GetData()->Fun()->Eval(state.tempo.ang);
	 //m_Config[moR(PAINTER_UPDATE_NOISE_TIME)].GetData()->Float();
	numAveIter = m_Config[moR(PAINTER_NUM_AVE_ITER)].GetData()->Int();
	aveInterval = m_Config[moR(PAINTER_AVE_INTERVAL)].GetData()->Int();
	followGrad = m_Config[moR(PAINTER_FOLLOW_GRAD)].GetData()->Int();
	noiseProb = m_Config[moR(PAINTER_NOISE_PROB)].GetData()->Fun()->Eval(state.tempo.ang);
	noiseMag = m_Config[moR(PAINTER_NOISE_MAG)].GetData()->Fun()->Eval(state.tempo.ang);
	for (int i = MO_RED; i <= MO_ALPHA; i++)
		color[i] = m_Config[moR(PAINTER_COLOR)][MO_SELECTED][i].GetData()->Fun()->Eval(state.tempo.ang);
	blending = m_Config[moR(PAINTER_BLENDING)].GetIndexValue();
}

void moEffectPainter::updateInteractionParameters()
{
}
