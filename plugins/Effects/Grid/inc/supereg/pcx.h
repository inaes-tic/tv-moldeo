//////////////////////////////////////////////////////////////////
//
//        Funciones para cargar y crear archivos pcx
//                    en DJGPP
//
//          
//////////////////////////////////////////////////////////////////


//#include <dos.h>
//#include <conio.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
#    include <conio.h>
#else
#    include "../../../../../inc/port/getc.h"
#endif

#ifndef __PCX_H__
#define __PCX_H__
///////////////////////////////////////////////////
//       Estructura del encabezado de un pcx     //
///////////////////////////////////////////////////

typedef struct
{
 char manufacturer;
 char version;
 char encoding;
 char bits_per_pixel;
 short xmin,ymin;
 short xmax,ymax;
 short hres;
 short vres;
 char palette16[48];
 char reserved;
 char color_planes;
 short bytes_per_line;
 short palette_type;
 short hscreensize;
 short vscreensize;
 char filler[54];

} PcxHeader;

typedef struct
{
  PcxHeader hdr;
  char *bitmap;
  char  pal[768];
  unsigned int imagebytes;
  unsigned short width,height;

} PcxFile;


char *Read_PCX(char *filename, char PcxPal[768],char EcoData);
void Grabar_PCX(char *buffer, short width, short height,char palette[768],char mode_color, char *filename);


#endif


