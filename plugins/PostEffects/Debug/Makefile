# Makefile - debug
MO_SRC_DIR=../../../src/libmoldeo/
MO_INC_DIR=../../../inc/
MO_EXT_LIBS_DIR=../../../../../externlibs/

SRC_DIR=./src/
INC_DIR=./inc/
BIN_DIR=../../../bin/debug/plugins/posteffects/
LIB_DIR=../../../bin/debug/

COMPILER=g++
LIB_FLAGS=-shared -fPIC -lmoldeod -L${LIB_DIR}
CPP_FLAGS=-I$(INC_DIR) -I$(MO_INC_DIR) -I/usr/include/SDL -I/usr/include/GL -I/usr/include/smpeg -I/usr/include -I/usr/include/freetype2 -I$(MO_EXT_LIBS_DIR)tinyxml/inc

SRC=$(SRC_DIR)moPostEffectDebug.cpp

OBJ=$(SRC_DIR)moPostEffectDebug.o

compile:$(OBJ)
	$(COMPILER) $(LIB_FLAGS) -o $(BIN_DIR)libdebug.so $(OBJ)
	
%.o: %.cpp
	$(COMPILER) ${CPP_FLAGS} -o $@ -c $<
	
clean:
	rm $(SRC_DIR)*.o -f
	rm $(BIN_DIR)libdebug.so -f
