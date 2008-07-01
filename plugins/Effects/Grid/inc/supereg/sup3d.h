#include <math.h>

#if !defined(PI)
#define PI 3.1416
#endif

#if !defined(__SUP3D_H__)
#define __SUP3D_H__

struct punto {
        short x,y;
        };

struct point3d {           /*Sirve para definir puntos en un espacio real*/
    float x,y,z;       /*cartesiano, tres ejes ,tres coordenadas*/
          };

typedef struct vecteurtype {           /*Define la orientacion de un vector*/
                   /*con valores entre 0 y 1 para productos*/
                   /*vectoriales }{En las procedures de giros y etc*/
                   /*se utilizan productos vectoriales, cuyos vectores*/
                   /*son de norme 1*/
         float x,y,z;
         float norme;
} tvecteurtype;

typedef struct SVector3df
{
 float x,y,z;
 float norma;
} TVector3df;

typedef struct SVector3di
{
 int x,y,z;
} TVector3di;

typedef struct STerna3df
{
 TVector3df v1,v2,v3;
} TTerna3df;

typedef struct Spunto3df
{
  float x,y,z;
} Tpunto3df;

typedef struct Spuntoi
{
  int x,y;
} Tpuntoi;

typedef struct Spuntof
{
  float x,y;
} Tpuntof;


typedef struct STriangulo3d
{
  Tpunto3df  p3d1,p3d2,p3d3;
  Tpuntoi p1,p2,p3;
  char c1,c2,c3;
  TVector3df Normal;
  char SeDibuja;
} TTriangulo3d;


typedef struct SCAMARA3D {

  float Ox,Oy,Oz,constante;
  struct vecteurtype u,v,w,vecteur1,vecteur2;
  struct punto get3d;
  float angle,ang;
  int foco;
  int wireframe;
  int lineas;
} TCAMARA3D;

/*transforma un punto 3d en un punto2d
                   correspondiente a su proyeccion en la pantalla
                   de la camara
                   este punto 2d es representado por
                   la variable "get3d".x .y*/

void get3dpoint640x350(float x1,float y1,float z1, struct vecteurtype u, struct vecteurtype v, struct vecteurtype w, TCAMARA3D *CAM);
void get3dpoint640x480(float x1,float y1,float z1, struct vecteurtype u, struct vecteurtype v, struct vecteurtype w, TCAMARA3D *CAM);
void get3dpoint640x480_fast_0(float x1,float y1,float z1, struct vecteurtype u, struct vecteurtype v, struct vecteurtype w, TCAMARA3D *CAM);
void get3dpoint320x200(float x1,float y1,float z1, struct vecteurtype u, struct vecteurtype v, struct vecteurtype w, TCAMARA3D *CAM);
void get3dpoint320x240(float x1,float y1,float z1, struct vecteurtype u, struct vecteurtype v, struct vecteurtype w, TCAMARA3D *CAM);
void inivecteurs(float xo, float yo, float zo, TCAMARA3D *CAM);
void movevector(struct vecteurtype v1,struct vecteurtype v2, struct vecteurtype v3, float angulo, TCAMARA3D *CAM);
void viewfrom(float ax, float ay, float az, TCAMARA3D *CAM);
void viewfromto(struct point3d A, struct point3d M, TCAMARA3D *CAM);
void turnview(float angleuv, float anglevw, float anglewu, TCAMARA3D *CAM);
void girar(float ang, struct point3d M, TCAMARA3D *CAM);
void movepointview(float valx, float valy, float valz, TCAMARA3D *CAM);

#endif
