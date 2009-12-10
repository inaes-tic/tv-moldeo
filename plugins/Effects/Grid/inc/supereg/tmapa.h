#include"supereg/tipos.h"
#include"supereg/suppal.h"

#ifndef __TMAPA_H__
#define __TMAPA_H__

//Por defecto el mapa es de 256x256 //division por poligonos
//El bitmap de alturas es de 1024x1024

/*

class TObjects
{
  int xi,yi,zi;
  float xf,yf,zf;

  PolList Draw(void);
};

class TObjectsGrid : TObjects
{




};


class TObjectsList
{
 public:



 TObjects *Objs;

};
*/
typedef struct headermapa
{
	int PosicionX;
	int PosicionY;

	int MapaAncho;
	int MapaAlto;

	int AlturaMaxima;
	int AlturaMinima;
	int AlturaAgua;
} theadermapa;

class TMapa
{
  public:

//Aca empieza la estructura del archivo

  int Memoria_Utilizada;

  unsigned char NombreMapa[80];

  int PosicionX;
  int PosicionY;

  int MapaAncho;
  int MapaAlto;

  int AlturaMaxima;
  int AlturaMinima;
  int AlturaAgua;

  int DistanciaEntrePuntos;
  int SuperficieTotal;

  char NombreArchivoPaleta[12];
  char Paleta[768];
//aca termina la estructura del archivo//el header

  TBITMAP *BitMapa;
  //La imagen del mapa
  TBITMAP **texturas;//La imagen del mapa dividido en cuadraditos
//aca termina la estructura del archivo

//  TObjectsList **ListaObjetos;
  TBITMAP *BitNubes;//opcional por ahora

  int Tamanio;
  TIPOP *MapaDatos;
  //TIPO **Linea;

  unsigned char ContadorDeMapas;

  //Para un buen efecto esferico
  int SuperficieTotalMedio;
  int RadioCirculoEnMapa;
  int DimensionMatrizEnMapa;
  int RadioDelPlaneta;


  float radioc;
  float radiop;
  float factorradiopradioc;
  float segmentocubo;

  int PosicionAnteriorX;
  int PosicionAnteriorY;


  TMapa();
  ~TMapa();

  void CargarMapa(void);

  void CargarTexturaMapa(TPALETA *PAL);

  void CargarMapa(char *);

  void CalcularBitMapa(void);

  void DibujarMapa(TBITMAP *);

  void SalvarMapa(void);

  void MostrarDatos(void);

  void CalcularConstantes(void);

  void AdecuarMapa(TBITMAP **);
};



#endif
