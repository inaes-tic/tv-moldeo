#include "supereg/tipos.h"
#include "supereg/sup3d.h"
#include "supereg/suppal.h"
#include "supereg/tgrdutil.h"

#if !defined(__TMOTOR_H__)
#define __TMOTOR_H__

#define PALETAS 16

typedef struct Snave
{
 float AlturaRelativa;
 float Separacion;
 float z;
 TTerna3df Tdireccion;
 float grav;
 float T;
} Tnave;

class TEngine_Utility
{
public:

 float MULTALT;
 int SafeMode;
 TVector3df Luz;
 float Luzx,Luzy,Luzz;

 int Inhibir;
 int Poligonos;
 int Memoria_Utilizada;//en bytes

 TBITMAP *pagina_virtual;

 TGrid_Utility *TG_RELIEVE;
 TGrid_Utility *TG_NUBES;


 TCAMARA3D CAM3D;

 int Rapidez;
 int RapidezMaxima;

 TEngine_Utility();
 ~TEngine_Utility();


 void Start_Engine(void);
 void Loop_Engine(void);
 void Stop_Engine(void);
 void ImprimirTodaLaInfo(void);
 int Check_Keyb(int);

 void Trim(int val);
 void Pitch(int val);
 };

#endif

