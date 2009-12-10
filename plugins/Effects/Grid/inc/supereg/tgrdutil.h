#include"supereg/tipos.h"
#include"supereg/tmapa.h"
#include"supereg/tgrid.h"


#ifndef __TGRDUTIL_H__
#define __TGRDUTIL_H__

//Manejador de grillas


class TGrid_Utility
{
   public:

         TGrid *Inicial, *Actual, *Final;
         TMapa *Mapa;

         float FactorDeAspereza;
         float DistanciaAlRelieve;

         int MinGridShowing;
         int MaxGridShowing;
         int LUCES;

         //otros
         int Inhibir;
         int SafeMode;
         float Luzx,Luzy,Luzz;
         float MULTALT;
         int Memoria_Utilizada;

//constructor: primer parametro corresponde a
//cantidad de escalas posibles( = cantidad de grillas)
//segundo parametro: ancho de las matrices cuadradas de las grillas

         TGrid_Utility(int,int);
         ~TGrid_Utility();


         TGrid* Grilla(int);

         int get_alt_sferical(int x, int y);
         int get_alt_planar(int x, int y);


         void TiempoCorriendo(void);
         void EfectoOla(float);
         int FactorDeAlturaMaximaDeEscala(int);
         int FactorDeAlturaMinimaDeEscala(int);
         void ActualizarInicial(int,int);
         void ActualizarGrillas(int,TCAMARA3D*);
         void ProyectaryDibujar(TBITMAP *,TCAMARA3D*);
		 void glProyectaryDibujar(TBITMAP *,TCAMARA3D*);
         void Posicionar(int,int,float,TCAMARA3D*);
         void ImprimirTodaLaInfo(void);
};

#endif
