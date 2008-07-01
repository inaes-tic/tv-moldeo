#include "supereg/tipos.h"

#ifndef __SUPPAL_H__
#define __SUPPAL_H__

typedef struct SPALETA {
        unsigned char PaletaGlobal[768];
} TPALETA;

typedef struct SRGB {
float r,g,b;
} TRGB;

typedef struct SColorAltura {
	TRGB C;
	int alturai;
	float alturaf;//entre 0 y 1
} TColorAltura;

typedef struct SPaletaAltura {
	TColorAltura *P;
	int N;//cantidad de alturas
} TPaletaAltura;

void setvgapal(unsigned char PAL[3*256],TPALETA *TPAL);
void Grises(TPALETA *TPAL);
void SetPaleta_File(char *filename,TPALETA *TPAL);
void SetColor0(TPALETA *TPAL);
void SetColor0Agua(TPALETA *TPAL);
void fade_to_black(TPALETA *TPAL);
void fade_in(unsigned char*palette,TPALETA *TPAL);
TPaletaAltura* CrearPaleta(int);
void DestruirPaleta(TPaletaAltura*);
TRGB PaletaAlturas(float,int,TPaletaAltura*);
#endif


