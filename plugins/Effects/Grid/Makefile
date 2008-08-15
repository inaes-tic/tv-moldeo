# Makefile - grilla
MO_SRC_DIR=../../../src/libmoldeo/
MO_INC_DIR=../../../inc/
MO_EXT_LIBS_DIR=../../../../../externlibs

SRC_DIR=./src/
INC_DIR=./inc/
BIN_DIR=../../../bin/debug/plugins/effects/
LIB_DIR=../../../bin/debug/

COMPILER=g++
LIB_FLAGS=-shared -fPIC -lmoldeod -L${LIB_DIR}
CPP_FLAGS=-I$(INC_DIR) -I$(INC_DIR)supereg -I$(MO_INC_DIR) -I/usr/include/SDL -I/usr/include/GL -I/usr/include/smpeg -I/usr/include -I/usr/include/freetype2 -I$(MO_EXT_LIBS_DIR)tinyxml/inc 

SRC=$(SRC_DIR)moEffectGrid.cpp\
	$(SRC_DIR)supereg/pcx.cpp\
	$(SRC_DIR)supereg/sup3d.cpp\
	$(SRC_DIR)supereg/suppal.cpp\
	$(SRC_DIR)supereg/suprand.cpp\
	$(SRC_DIR)supereg/tgrdutil.cpp\
	$(SRC_DIR)supereg/tgrid.cpp\
	$(SRC_DIR)supereg/tipos.cpp\
	$(SRC_DIR)supereg/tmapa.cpp\
	$(SRC_DIR)supereg/tmotor.cpp
# 	$(SRC_DIR)supereg/trdrmap.cpp
	

OBJ=$(SRC_DIR)moEffectGrid.o\
	$(SRC_DIR)supereg/pcx.o\
	$(SRC_DIR)supereg/sup3d.o\
	$(SRC_DIR)supereg/suppal.o\
	$(SRC_DIR)supereg/suprand.o\
	$(SRC_DIR)supereg/tgrdutil.o\
	$(SRC_DIR)supereg/tgrid.o\
	$(SRC_DIR)supereg/tipos.o\
	$(SRC_DIR)supereg/tmapa.o\
	$(SRC_DIR)supereg/tmotor.o
# 	$(SRC_DIR)supereg/trdrmap.o

compile:$(OBJ)
	$(COMPILER) $(LIB_FLAGS) -o $(BIN_DIR)libgrid.so $(OBJ)
	
%.o: %.cpp
	$(COMPILER) ${CPP_FLAGS} -o $@ -c $<
	
clean:
	rm $(OBJ) -f
	rm $(BIN_DIR)libgrid* -f
