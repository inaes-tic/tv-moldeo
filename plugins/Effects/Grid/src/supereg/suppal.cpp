#include"supereg/suppal.h"
  
void DestruirPaleta(TPaletaAltura *ALPAL) {
delete[] ALPAL->P;
delete ALPAL;
ALPAL = NULL;
}

TPaletaAltura * CrearPaleta(int MAXALT) {
TPaletaAltura *ALPAL;
ALPAL = new TPaletaAltura;
ALPAL->N = 9;
ALPAL->P = new TColorAltura [ALPAL->N];
ALPAL->P[0].alturaf = 0.0;
ALPAL->P[0].alturai = 0;
ALPAL->P[0].C.r = 0.0; ALPAL->P[0].C.g = 0.0; ALPAL->P[0].C.b = 0.9;
ALPAL->P[1].alturaf = 0.1;
ALPAL->P[1].alturai =(int) 10*(MAXALT/100);
ALPAL->P[1].C.r = 0.8; ALPAL->P[1].C.g = 0.8; ALPAL->P[1].C.b = 0.4;
ALPAL->P[2].alturaf = 0.2;
ALPAL->P[2].alturai =(int) 20*(MAXALT/100);
ALPAL->P[2].C.r = 0.5; ALPAL->P[2].C.g = 0.9; ALPAL->P[2].C.b = 0.0;
ALPAL->P[3].alturaf = 0.3f;
ALPAL->P[3].alturai =(int) 30*(MAXALT/100);
ALPAL->P[3].C.r = 0.2; ALPAL->P[3].C.g = 0.8; ALPAL->P[3].C.b = 0.2;
ALPAL->P[4].alturaf = 0.4f;
ALPAL->P[4].alturai =(int) 40*(MAXALT/100);
ALPAL->P[4].C.r = 0.5f; ALPAL->P[4].C.g = 0.4; ALPAL->P[4].C.b = 0.0;
ALPAL->P[5].alturaf = 0.5;
ALPAL->P[5].alturai =(int) 50*(MAXALT/100);
ALPAL->P[5].C.r = 0.3; ALPAL->P[5].C.g = 0.3; ALPAL->P[5].C.b = 0.3;
ALPAL->P[6].alturaf = 0.6;
ALPAL->P[6].alturai =(int) 60*(MAXALT/100);
ALPAL->P[6].C.r = 0.6; ALPAL->P[6].C.g = 0.6; ALPAL->P[6].C.b = 0.5;
ALPAL->P[7].alturaf = 0.7;
ALPAL->P[7].alturai =(int) 70*(MAXALT/100);
ALPAL->P[7].C.r = 0.8; ALPAL->P[7].C.g = 0.8; ALPAL->P[7].C.b = 0.8;
ALPAL->P[8].alturaf = 0.8;
ALPAL->P[8].alturai =(int) 80*(MAXALT/100);
ALPAL->P[8].C.r = 0.8; ALPAL->P[8].C.g = 0.9; ALPAL->P[8].C.b = 1.0;
return(ALPAL);
}

TRGB PaletaAlturas(float altitudf, int altitudi, TPaletaAltura *ALPAL) {
	int i,listo;
	float altitudsf;
	int altitudsi,altitudsi2;
	TRGB C;

	C.r = 1.0;
	C.g = 1.0;
	C.b = 1.0;
	listo=0;
	/*
	for(i=1;i<ALPAL->N;i++) {
	  if(altitudi<=ALPAL->P[i].alturai && listo==0) {
		altitudsi = altitudi - ALPAL->P[i-1].alturai;
		altitudsi2 = ALPAL->P[i].alturai - ALPAL->P[i-1].alturai;
		altitudsf =(float)(altitudsi) /(float)(altitudsi2);
		C.r = ALPAL->P[i-1].C.r+(ALPAL->P[i].C.r-ALPAL->P[i-1].C.r)*altitudsf;
		C.g = ALPAL->P[i-1].C.g+(ALPAL->P[i].C.g-ALPAL->P[i-1].C.g)*altitudsf;  
		C.b = ALPAL->P[i-1].C.b+(ALPAL->P[i].C.b-ALPAL->P[i-1].C.b)*altitudsf;
		listo = 1;
		}
	}
	
	if(listo==0) {
		if(altitudi<=ALPAL->P[0].alturai) {
		C = ALPAL->P[0].C;
		listo = 1;
		}
	}
	*/

//C.r = 0.1+((float)(altitudi) /(float) MAXALT);
//C.g = 0.1+((float)(altitudi) /(float) MAXALT);
//C.b = 0.1+((float)(altitudi) /(float) MAXALT);
//printf("rgb:%f,%f,%f",C.r,C.g,C.b);
return(C);
}



void setvgapal(unsigned char PAL[3*256],TPALETA *TPAL)
{
    int counter;
    for(counter = 0 ; counter < 768 ; counter++)
    TPAL->PaletaGlobal[counter] = PAL[counter];
}


void Grises(TPALETA *TPAL)
{
 int i;
 unsigned char PAL[3*256];


 for(i = 0; i < 63; i++)
 {
     PAL[i*3*3] = i;
     PAL[i*3*3+1] = i;
     PAL[i*3*3+2] = i;

     PAL[(i*3+1)*3] = i+1;
     PAL[(i*3+1)*3+1] = i;
     PAL[(i*3+1)*3+2] = i;

     PAL[(i*3+2)*3] = i+1;
     PAL[(i*3+2)*3+1] = i+1;
     PAL[(i*3+2)*3+2] = i;

 }

 for(i = 63*3; i < 256; i++)
 {
     PAL[i*3] = 63;
     PAL[i*3+1] = 63;
     PAL[i*3+2] = 63;

 }

 setvgapal(PAL,TPAL);

}

void SetPaleta_File(char *filename,TPALETA *TPAL)
{
    FILE *P;
    unsigned char PALETA[3*256];    

    P = fopen(filename,"rb");
    fread(PALETA,sizeof(char),3*256,P);
    fclose(P);
    setvgapal(PALETA,TPAL);
}


void SetColor0(TPALETA *TPAL)
{
TPAL->PaletaGlobal[0] = 0;
TPAL->PaletaGlobal[1] = 0;
TPAL->PaletaGlobal[2] = 0;
setvgapal(TPAL->PaletaGlobal,TPAL);
}

void SetColor0Agua(TPALETA *TPAL)
{
TPAL->PaletaGlobal[0] = 0;
TPAL->PaletaGlobal[1] = 100;
TPAL->PaletaGlobal[2] = 180;
setvgapal(TPAL->PaletaGlobal,TPAL);
}

void fade_to_black(TPALETA *TPAL)
{
int i,j;

for(j=0;j<256;j++)
{
 for(i=0;i<256*3;i++)
 {
  if(TPAL->PaletaGlobal[i]>0) TPAL->PaletaGlobal[i]--;
  }
}
}

void fade_in(unsigned char palette[3*256], TPALETA * TPAL)
{
int i,j;

for(i=0;i<256*3;i++) TPAL->PaletaGlobal[i]=0;

for(j=0;j<256;j++)
 {
  for(i=0;i<256*3;i++)
   {
    if(TPAL->PaletaGlobal[i]<palette[i]) TPAL->PaletaGlobal[i]++;
   }
 }
}
