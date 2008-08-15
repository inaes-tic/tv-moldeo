# Makefile - libmoldeo
MO_SRC_DIR=./src/libmoldeo/
MO_INC_DIR=./inc/
MO_EXT_LIBS_DIR=../../externlibs/

BIN_DIR=./bin/debug/

COMPILER=g++
LIB_FLAGS=-shared -fPIC
TARGET=bin/debug/libmoldeod
CPP_FLAGS=-I$(MO_INC_DIR) -I/usr/include/SDL -I/usr/include/GL -I/usr/include/smpeg -I/usr/include -I/usr/include/freetype2 -I$(MO_EXT_LIBS_DIR)/tinyxml/inc
LIB_FLAGS+=-L/usr/X11R6/lib -L/usr/lib -Llib -Llib/smpegd -Llib/sdld -Llib/sdlimg -L$(MO_EXT_LIBS_DIR)/tinyxml/lib
LIBS=-lpthread -lSDL -lSDL_image -lSDL_net -lGL -lGLU -lglut -lCg -lCgGL -lsmpeg -ljpeg -llua -lgle -lOGLFT -ltinyxml

SRC=$(MO_SRC_DIR)mo3dModelManager.cpp\
	$(MO_SRC_DIR)moAbstract.cpp\
	$(MO_SRC_DIR)moArray.cpp\
	$(MO_SRC_DIR)moBuckets.cpp\
	$(MO_SRC_DIR)moConfig.cpp\
	$(MO_SRC_DIR)moConnectionHandler.cpp\
	$(MO_SRC_DIR)moConsole.cpp\
	$(MO_SRC_DIR)moConsoleState.cpp\
	$(MO_SRC_DIR)moCurve.cpp\
	$(MO_SRC_DIR)moDeviceCode.cpp\
	$(MO_SRC_DIR)moDsGraph.cpp\
	$(MO_SRC_DIR)moEffect.cpp\
	$(MO_SRC_DIR)moEffectState.cpp\
	$(MO_SRC_DIR)moEventList.cpp\
	$(MO_SRC_DIR)moFontManager.cpp\
	$(MO_SRC_DIR)moGUIManager.cpp\
	$(MO_SRC_DIR)moImages.cpp\
	$(MO_SRC_DIR)moIODeviceManager.cpp\
	$(MO_SRC_DIR)moKeyboard.cpp\
	$(MO_SRC_DIR)moLive.cpp\
	$(MO_SRC_DIR)moLock.cpp\
	$(MO_SRC_DIR)moLuaBase.cpp\
	$(MO_SRC_DIR)moMasterEffect.cpp\
	$(MO_SRC_DIR)moMasterPlugin.cpp\
	$(MO_SRC_DIR)moMidi.cpp\
	$(MO_SRC_DIR)moMixer.cpp\
	$(MO_SRC_DIR)moMoldeoObject.cpp\
	$(MO_SRC_DIR)moMotion.cpp\
	$(MO_SRC_DIR)moMouse.cpp\
	$(MO_SRC_DIR)moNetTCPIn.cpp\
	$(MO_SRC_DIR)moNetTCPOut.cpp\
	$(MO_SRC_DIR)moNetUDPIn.cpp\
	$(MO_SRC_DIR)moNetUDPOut.cpp\
	$(MO_SRC_DIR)moOGLFT.cpp\
	$(MO_SRC_DIR)moParam.cpp\
	$(MO_SRC_DIR)moPlugin.cpp\
	$(MO_SRC_DIR)moPostEffect.cpp\
	$(MO_SRC_DIR)moPostPlugin.cpp\
	$(MO_SRC_DIR)moPreConfig.cpp\
	$(MO_SRC_DIR)moPreEffect.cpp\
	$(MO_SRC_DIR)moPrePlugin.cpp\
	$(MO_SRC_DIR)moPresets.cpp\
	$(MO_SRC_DIR)moRenderManager.cpp\
	$(MO_SRC_DIR)moResourceManager.cpp\
	$(MO_SRC_DIR)moSceneEffect.cpp\
	$(MO_SRC_DIR)moScript.cpp\
	$(MO_SRC_DIR)moScriptManager.cpp\
	$(MO_SRC_DIR)moSequenceEffect.cpp\
	$(MO_SRC_DIR)moShaderCG.cpp\
	$(MO_SRC_DIR)moShader.cpp\
	$(MO_SRC_DIR)moShaderGLSL.cpp\
	$(MO_SRC_DIR)moSoundManager.cpp\
	$(MO_SRC_DIR)moText.cpp\
	$(MO_SRC_DIR)moTextureManager.cpp\
	$(MO_SRC_DIR)moThread.cpp\
	$(MO_SRC_DIR)moTracker.cpp\
	$(MO_SRC_DIR)moTypes.cpp\
	$(MO_SRC_DIR)moVideoGraph.cpp\
	$(MO_SRC_DIR)moXmlConfig.cpp\
	$(MO_SRC_DIR)glee/GLee.c\
	$(MO_SRC_DIR)3ds/3ds.cpp

OBJ=$(MO_SRC_DIR)mo3dModelManager.o\
	$(MO_SRC_DIR)moAbstract.o\
	$(MO_SRC_DIR)moArray.o\
	$(MO_SRC_DIR)moBuckets.o\
	$(MO_SRC_DIR)moConfig.o\
	$(MO_SRC_DIR)moConnectionHandler.o\
	$(MO_SRC_DIR)moConsole.o\
	$(MO_SRC_DIR)moConsoleState.o\
	$(MO_SRC_DIR)moCurve.o\
	$(MO_SRC_DIR)moDeviceCode.o\
	$(MO_SRC_DIR)moDsGraph.o\
	$(MO_SRC_DIR)moEffect.o\
	$(MO_SRC_DIR)moEffectState.o\
	$(MO_SRC_DIR)moEventList.o\
	$(MO_SRC_DIR)moFontManager.o\
	$(MO_SRC_DIR)moGUIManager.o\
	$(MO_SRC_DIR)moImages.o\
	$(MO_SRC_DIR)moIODeviceManager.o\
	$(MO_SRC_DIR)moKeyboard.o\
	$(MO_SRC_DIR)moLive.o\
	$(MO_SRC_DIR)moLock.o\
	$(MO_SRC_DIR)moLuaBase.o\
	$(MO_SRC_DIR)moMasterEffect.o\
	$(MO_SRC_DIR)moMasterPlugin.o\
	$(MO_SRC_DIR)moMidi.o\
	$(MO_SRC_DIR)moMixer.o\
	$(MO_SRC_DIR)moMoldeoObject.o\
	$(MO_SRC_DIR)moMotion.o\
	$(MO_SRC_DIR)moMouse.o\
	$(MO_SRC_DIR)moNetTCPIn.o\
	$(MO_SRC_DIR)moNetTCPOut.o\
	$(MO_SRC_DIR)moNetUDPIn.o\
	$(MO_SRC_DIR)moNetUDPOut.o\
	$(MO_SRC_DIR)moOGLFT.o\
	$(MO_SRC_DIR)moParam.o\
	$(MO_SRC_DIR)moPlugin.o\
	$(MO_SRC_DIR)moPostEffect.o\
	$(MO_SRC_DIR)moPostPlugin.o\
	$(MO_SRC_DIR)moPreConfig.o\
	$(MO_SRC_DIR)moPreEffect.o\
	$(MO_SRC_DIR)moPrePlugin.o\
	$(MO_SRC_DIR)moPresets.o\
	$(MO_SRC_DIR)moRenderManager.o\
	$(MO_SRC_DIR)moResourceManager.o\
	$(MO_SRC_DIR)moSceneEffect.o\
	$(MO_SRC_DIR)moScript.o\
	$(MO_SRC_DIR)moScriptManager.o\
	$(MO_SRC_DIR)moSequenceEffect.o\
	$(MO_SRC_DIR)moShaderCG.o\
	$(MO_SRC_DIR)moShader.o\
	$(MO_SRC_DIR)moShaderGLSL.o\
	$(MO_SRC_DIR)moSoundManager.o\
	$(MO_SRC_DIR)moText.o\
	$(MO_SRC_DIR)moTextureManager.o\
	$(MO_SRC_DIR)moThread.o\
	$(MO_SRC_DIR)moTracker.o\
	$(MO_SRC_DIR)moTypes.o\
	$(MO_SRC_DIR)moVideoGraph.o\
	$(MO_SRC_DIR)moXmlConfig.o\
	$(MO_SRC_DIR)glee/GLee.o\
	$(MO_SRC_DIR)3ds/3ds.o

compile:$(OBJ)
	$(COMPILER) ${CPP_FLAGS} $(LIB_FLAGS) -o $(BIN_DIR)libmoldeod.so $(OBJ) ${LIBS}

%.o: %.c
	$(COMPILER) ${CPP_FLAGS} -o $@ -c $<

%.o: %.cpp
	$(COMPILER) ${CPP_FLAGS} -o $@ -c $<

install:	
	mkdir /usr/local/lib/moldeo -p
	cp $(BIN_DIR)libmoldeod.so /usr/local/lib/moldeo -f
	chmod 775 /usr/local/lib/moldeo/libmoldeod.so
	echo /usr/local/lib/moldeo > /etc/ld.so.conf.d/moldeo.conf
	ldconfig

uninstall:	
	rm -Rf /usr/local/lib/moldeo
	rm -f /etc/ld.so.conf.d/moldeo.conf
	ldconfig
			
clean:
	rm $(MO_SRC_DIR)*.o -f
	rm $(BIN_DIR)libmoldeo* -f
