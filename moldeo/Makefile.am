# additional include paths necessary to compile the C++ library
AM_CPPFLAGS = -I$(top_srcdir)/src @AM_CFLAGS@ @SDL_CFLAGS@ @SDL_IMAGE_CFLAGS@ @SDL_NET_CFLAGS@ @GL_CFLAGS@ @GLU_CFLAGS@ @LUA_CFLAGS@ @FREETYPE_CFLAGS@ @TINYXML_CFLAGS@

libmoldeo_CPPFLAGS = $(SDL_CFLAGS) $(SDL_IMAGE_CFLAGS) $(SDL_NET_CFLAGS) $(GL_CFLAGS) $(GLU_CFLAGS) $(LUA_CFLAGS) $(FREETYPE_CFLAGS) $(TINYXML_CFLAGS)
libmoldeo_LIBS = $(SDL_LIBS) $(SDL_IMAGE_LIBS) $(SDL_NET_LIBS) $(GL_LIBS) $(GLU_LIBS) $(LUA_LIBS) $(FREETYPE_LIBS) $(TINYXML_LIBS)

# the library names to build (note we are building static libs only)
lib_LTLIBRARIES = libmoldeo-1.0.la

# where to install the headers on the system
libmoldeo_1_0_ladir = $(includedir)/moldeo

# the list of header files that belong to the library (to be installed later)
libmoldeo_1_0_la_HEADERS = \
	mo3dModelManager.h \
	moAbstract.h \
	moActions.h \
	moArcBall.h \
	moArray.h \
	moBasePlugin.h \
	moBuckets.h \
	moConfig.h \
	moConnectors.h \
	moConsole.h \
	moConsoleState.h \
	moDataManager.h \
	moDebugManager.h \
	moDecoderManager.h \
	moDeviceCode.h \
	moDsGraph.h \
	moEffect.h \
	moEffectManager.h \
	moEffectState.h \
	moEffectTemplate.h \
	moEventList.h \
	moFBManager.h \
	moFBO.h \
	moFileManager.h \
	moFileRead.h \
	moFileWrite.h \
	moFilterManager.h \
	moFontManager.h \
	moGLManager.h \
	moGsGraph.h \
	moGUIManager.h \
	moIODeviceManager.h \
	moIODevicePlugin.h \
	moldeo.h \
	moLock.h \
	moLuaBase.h \
	moLunaClasses.h \
	moLuna.h \
	moMasterEffect.h \
	moMasterPlugin.h \
	moMathCurve.h \
	moMathDMatrix.h \
	moMathDVector.h \
	moMathFFT.h \
	moMathFunction.h \
	moMathFunctionIndex.h \
	moMath.h \
	moMathManager.h \
	moMathMatrix.h \
	moMathNumericalAnalysis.h \
	moMathPolynomial.h \
	moMathQuaternion.h \
	moMathVector3.h \
	moMathVector4.h \
	moMathVector.h \
	moMoldeoObject.h \
	moMotion.h \
	moNetManager.h \
	moP5.h \
	moParam.h \
	moPlugin.h \
	moPostEffect.h \
	moPostPlugin.h \
	moPreConfig.h \
	moPreEffect.h \
	moPrePlugin.h \
	moPresets.h \
	moRenderManager.h \
	moResourceManager.h \
	moResourcePlugin.h \
	moSceneEffect.h \
	moScript.h \
	moScriptManager.h \
	moSequenceEffect.h \
	moShaderCG.h \
	moShaderGLSL.h \
	moShader.h \
	moShaderManager.h \
	moSoundManager.h \
	moTempo.h \
	moTextFilterParam.h \
	moText.h \
	moTextureFilter.h \
	moTextureFilterIndex.h \
	moTexture.h \
	moTextureIndex.h \
	moTextureManager.h \
	moThread.h \
	moTimeManager.h \
	moTypes.h \
	moValue.h \
	moVdpauGraph.h \
	moVideoGraph.h \
	moVideoManager.h \
	moXmlConfig.h \
	${NULL}

# the sources to add to the library and to add to the source distribution
libmoldeo_1_0_la_SOURCES =  $(libmoldeo_1_0_la_HEADERS) \
	mo3dModelManager.cpp \
	moAbstract.cpp \
	moArray.cpp \
	moBuckets.cpp \
	moConfig.cpp \
	moConnectionHandler.cpp \
	moConsole.cpp \
	moConsoleState.cpp \
	moCurve.cpp \
	moDeviceCode.cpp \
	moDsGraph.cpp \
	moEffect.cpp \
	moEffectState.cpp \
	moEventList.cpp \
	moFontManager.cpp \
	moGUIManager.cpp \
	moImages.cpp \
	moIODeviceManager.cpp \
	moKeyboard.cpp \
	moLive.cpp \
	moLock.cpp \
	moLuaBase.cpp \
	moMasterEffect.cpp \
	moMasterPlugin.cpp \
	moMidi.cpp \
	moMixer.cpp \
	moMoldeoObject.cpp \
	moMotion.cpp \
	moMouse.cpp \
	moNetTCPIn.cpp \
	moNetTCPOut.cpp \
	moNetUDPIn.cpp \
	moNetUDPOut.cpp \
	moOGLFT.cpp \
	moParam.cpp \
	moPlugin.cpp \
	moPostEffect.cpp \
	moPostPlugin.cpp \
	moPreConfig.cpp \
	moPreEffect.cpp \
	moPrePlugin.cpp \
	moPresets.cpp \
	moRenderManager.cpp \
	moResourceManager.cpp \
	moSceneEffect.cpp \
	moScript.cpp \
	moScriptManager.cpp \
	moSequenceEffect.cpp \
	moShaderCG.cpp \
	moShader.cpp \
	moShaderGLSL.cpp \
	moSoundManager.cpp \
	moText.cpp \
	moTextureManager.cpp \
	moThread.cpp \
	moTracker.cpp \
	moTypes.cpp \
	moVideoGraph.cpp \
	moXmlConfig.cpp \
	glee/GLee.c \
	3ds/3ds.cpp \
	${NULL}

libmoldeo_1_0_la_LDFLAGS = -no-undefined -version-info @MOLDEO_LT_CURRENT@:@MOLDEO_LT_REVISION@:@MOLDEO_LT_AGE@
