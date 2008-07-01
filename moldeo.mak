# Makefile - moldeo
MO_INC_DIR=./inc/
MO_EXT_LIBS_DIR=../../externlibs/

CC=gcc
CFLAGS=
CXX=g++
CXXFLAGS=$(CFLAGS)
RC=windres -O COFF

CPPFLAGS= -W -fexceptions -g -O0 -I$(MO_INC_DIR) -I/usr/include/SDL -I/usr/include/GL -I/usr/include/smpeg -I/usr/include -I/usr/include/freetype2 -I$(MO_EXT_LIBS_DIR)/tinyxml/inc
LD=$(CXX) $(CXXFLAGS)
LDFLAGS=
TARGET=bin/debug/moldeod
LDFLAGS+=-L/usr/X11R6/lib -L/usr/lib -Llib -Llib/smpegd -Llib/sdld -Llib/sdlimg -L$(MO_EXT_LIBS_DIR)/tinyxml/lib -L/usr/lib -Lbin/debug
LIBS+=-lmoldeod

.PHONY: all
all: $(TARGET)

PORT_H= \
	inc/port/getc.h \
	inc/port/itoa.h \
	inc/port/ptypes.h \
	inc/port/strconst.h \
	inc/port/stricmp.h

SRCS=$(PORT_H)\
	src/main/moldeomain.cpp
	

OBJS=$(patsubst %.rc,%.res,$(patsubst %.cxx,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.c %.cc %.cpp %.cxx %.rc,$(SRCS)))))))

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<	
	
.PHONY: clean
clean:
	-rm -f $(OBJS) $(TARGET) moldeo.dep

.PHONY: depends
depends:
	-$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $(filter %.c %.cc %.cpp %.cxx,$(SRCS)) > moldeo.dep

-include moldeo.dep

