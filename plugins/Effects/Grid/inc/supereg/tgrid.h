#include"supereg/tipos.h"
#include"supereg/suppal.h"
#include"supereg/suprand.h"
#include"supereg/tmapa.h"

#ifndef __TGRID_H__
#define __TGRID_H__

typedef struct SProy
{
  Tpunto3df P3d; //el punto 3d
  Tpuntof P;     //el mismo proyectado en 2d
  unsigned char Intensidad;
  float If;
  TVector3df N;
  float Distancia;
  char Visible_Esfera_Rel;
  char SeDibuja; //esta dentro o fuera del cono de visibilidad
} TProy;

typedef struct SV3D_f {
float x,y,z;
float u,v;
int c;
float cf;
TRGB C;
TVector3df N;
} V3D_f;

class TGrid

{  public:

	//PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB;
	//PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;

     TGrid *Padre,*Hijo;
     TGrid *Self;
     TIPO **G;    //solo datos de altura
     //TIPO **L_G;
     TProy *PROY;
     TProy **L_PROY;
     TBITMAP *textura;
     TBITMAP **texturas;
     TIPO2P GP;  //datos extras
	 TIPO2P * L_GP;   //Vectores de puntos
     TBITMAP **GT;        //Vector de texturas
     TVector3df *Normales;
     char TEXTDEF;//definicion de las texturas por defecto NTEX
     char Nesima;
     char EnRango;

     int dvs;
     int dvspp;//optimizacion, dvs+1
     int Tamanio;

     int rad;
     int radm2;//optimizacion, rad*2
     int radd2;//optimizacion, rad/2
     int bordea;

     float AsperezaDeEscala;

     int x,y;
     int dify,difx;

     int PosRealCentroX;
     int PosRealCentroY;

     float z; //al pepe

     //efecto ola
     float tiempo,tiempofin,dt;
     float d1t,d2t,Ampl,Amplini,Amplfin,ohm,fresp,velonda;
     float d;
     //fin efecto ola

     int MAXALT;
     int MINALT;
     float MULTALT;
     int RELAGUASUP;
     int RELAGUAINF;

	 TPaletaAltura *ALPAL;

//Memoria
int Memoria_Utilizada;
int Poligonos;

//Luz
float Luzx,Luzy,Luzz;
float contraste;

//Render y precalculo
 int cpx;
 int cpy;
 int cpx_in_x;
 int cpy_in_y;
 float constante;
 float alfaang;
 float dd;
 float MRSQ;//MaparadiocSQ (al cuadrado)
 float HMmax;
 float COSANGMAXIMODEF;
 float COSANGMAXIMO;
 float d1;
 float cosfoco;
 float distmaxima;

 char Actualizado;

 TGrid(int,int,char,int=0);
 ~TGrid();

//FUNCIONES y PROCEDIMIENTOS

//   int ActualizacionInminente(int,int);//0 si no 1 si si
     void Actualizar(int,int);
     TIPO Promediar(TIPO, TIPO, TIPO, TIPO);
     TIPO Promediar(TIPO, TIPO);
     void Ruido(TIPO *);

     void Animacion(TGrid*,TMapa*);

     void Precalculus(TGrid *,TMapa *,TCAMARA3D *);
     void Precalculus(TGrid *,TMapa *,float, TCAMARA3D *);

     void ProyectarBorde(TGrid*,TMapa*,float,TCAMARA3D *);
     void ProyectarTodo(TGrid*,TMapa*,float,TCAMARA3D *);
     void glProyectarBorde(TGrid*,TMapa*,float,TCAMARA3D *);
     void glProyectarTodo(TGrid*,TMapa*,float,TCAMARA3D *);
     void ProyectarBorde_SafeMode(TGrid*,TMapa*,float,TCAMARA3D *);
     void ProyectarTodo_SafeMode(TGrid*,TMapa*,float,TCAMARA3D *);

     void CalcularNormalesBorde(void);
     void CalcularNormalesTodo(void);

     void DibujarAlturaBorde(int,TBITMAP *);
     void DibujarAlturaTodo(int,TBITMAP *);
     void glDibujarAlturaBorde(int,TBITMAP *,TCAMARA3D*);
     void glDibujarAlturaTodo(int,TBITMAP *,TCAMARA3D*);

     void ImprimirTodaLaInfo(void);

};

#endif




