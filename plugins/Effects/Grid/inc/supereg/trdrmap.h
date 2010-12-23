#ifndef __TRDRMAP_H__
#define __TRDRMAP_H__

#include "supereg/tipos.h"
#include "supereg/tmapa.h"

class TRadarMapa
{
  public:

  TBITMAP *RadarBitMapa;
  TBITMAP *RadarBitMapaPantallaCompleta;
//  BITMAP *CursorObs;
//  RGB *cursorpal;
  char Activado;//si o no
  int Posx;
  int Posy;
  int Zoom;

  int contador_de_ciclos;

  TRadarMapa(TMapa*,int,int,int,int);
  ~TRadarMapa();

  void Inicializar(TMapa *);

  void Actualizado(TMapa *);

  void Zoomeado(TMapa *,char);

 /* void MostrarContador( BITMAP *);

  void MostrarObservador(TMapa *,BITMAP *);

  void MostrarObservadorPantallaCompleta(TMapa *,BITMAP *);

  void MostrarRadarBitMapa(BITMAP *);

  void MostrarRadarBitMapaPantallaCompleta(BITMAP *);

  void Activo(TMapa *,BITMAP *);
*/
  void Activar(int);
};
#endif





