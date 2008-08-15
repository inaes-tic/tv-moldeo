#include"supereg/tgrdutil.h"

 
//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
TGrid*
TGrid_Utility::Grilla(int N)
{
  Actual = Inicial;
  while(N!=Actual->Nesima && Actual->Hijo!=NULL)
  {
     Actual = Actual->Hijo;
  }
  return(Actual);
}

//*----------*----------*----------*----------*----------*----------*----------


void
TGrid_Utility::TiempoCorriendo(void)
{

 Actual = Inicial->Hijo;
 if(Actual->dt!=0.0)
{
  while(Actual!=NULL)
 {
  Actual->tiempo+=Actual->dt;
  if(Actual->tiempo>=Actual->tiempofin)
  {
   Actual->tiempo = 0; Actual->dt=0.0;
  }
  Actual = Actual->Hijo;

 }
}
}


//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
////////////////////////////////

void
TGrid_Utility::ImprimirTodaLaInfo(void)
{
//nada
}

//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------

int
TGrid_Utility::FactorDeAlturaMaximaDeEscala(int N)
{
//return(N*N*100);
	return(0);
}

//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

int
TGrid_Utility::FactorDeAlturaMinimaDeEscala(int N)
{
//return(-N*N*100);
return(0);
}

//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//-----------------------------------------------------------------------------------------
//grillas numeradas de 0 a Nescalas(de mas grande a mas chica)

//-----------------------------------------------------------------------------------------

TGrid_Utility::TGrid_Utility(int Nescalas, int dimension)
{
    int i;
    char contador;
		contador = 0;
		Luzx = 8.0 / sqrt(129.0);
    Luzy = - 8.0 / sqrt(129.0);
    Luzz = -1.0 / sqrt(129.0);
    Inhibir = 0;
    SafeMode = 0;
    MULTALT = 1.5;
    Memoria_Utilizada = 0;


    FactorDeAspereza = 0.9;

    Mapa = new TMapa;

    Inicial = new TGrid(Mapa->DimensionMatrizEnMapa,0,contador);

    Inicial->Padre = NULL;
    Inicial->radm2 = Mapa->DistanciaEntrePuntos;
    Inicial->rad = Inicial->radm2/2;
    Inicial->radd2 = Inicial->rad/2;
    Inicial->AsperezaDeEscala = 0.9;
    Inicial->MAXALT = Mapa->AlturaMaxima;
    Inicial->MINALT = Mapa->AlturaMinima;
    Inicial->RELAGUASUP = Inicial->MAXALT - Mapa->AlturaAgua;
    Inicial->RELAGUAINF = Mapa->AlturaAgua - Inicial->MINALT;
    Inicial->PosRealCentroX = Mapa->PosicionX*Mapa->DistanciaEntrePuntos;
    Inicial->PosRealCentroY = Mapa->PosicionY*Mapa->DistanciaEntrePuntos;


    Inicial->Luzx = Luzx;
    Inicial->Luzy = Luzy;
    Inicial->Luzz = Luzz;
	Inicial->ALPAL = CrearPaleta(Inicial->MAXALT);
    Actual = Inicial;

    Actual->Self = Actual;

    for(i = 0; i <(Nescalas-1); i++)
       {
        contador++;
        Actual->Hijo = new TGrid(dimension,0,contador);

        Actual->Hijo->Padre = Actual;

        Actual = Actual->Hijo;
        Actual->Self = Actual;

        Actual->MAXALT = Mapa->AlturaMaxima + FactorDeAlturaMaximaDeEscala(i+1);
        Actual->MINALT = Mapa->AlturaMinima + FactorDeAlturaMinimaDeEscala(i+1);
        //otros
        Actual->MULTALT = MULTALT;
        Actual->Luzx = Luzx;
        Actual->Luzy = Luzy;
        Actual->Luzz = Luzz;
		Actual->ALPAL = CrearPaleta(Actual->MAXALT);


        Actual->RELAGUASUP = Actual->MAXALT - Mapa->AlturaAgua;
        Actual->RELAGUAINF = Mapa->AlturaAgua - Actual->MINALT;

        Actual->radm2 = Actual->Padre->rad; //radm2 ->esta es la distancia entre puntos
        Actual->rad =(Actual->radm2)/2;      //rad
        Actual->radd2 = Actual->rad/2;      //radd2
        Actual->AsperezaDeEscala = Actual->Padre->AsperezaDeEscala * FactorDeAspereza;
        }

 Actual->Hijo = NULL; Final = Actual;
 Inicial->AsperezaDeEscala = 0.07;
 Inicial->Hijo->AsperezaDeEscala = 0.07;

}

//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

TGrid_Utility::~TGrid_Utility()
{}

//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------

void
TGrid_Utility::ActualizarGrillas(int rapidez,TCAMARA3D *CAM)
{
  int SUMX,SUMY;
  float SUMZ;

  SUMX =(int)(CAM->u.x * rapidez);
  SUMY =(int)(CAM->u.y * rapidez);
  SUMZ =(float)(CAM->u.z * rapidez);       //avance automatico;

  CAM->Oz = CAM->Oz + SUMZ;
  if(CAM->Oz<0.0) CAM->Oz = 100000.0;

  Actual=Inicial;

  while(Actual!=NULL)
  {
   Actual->Actualizado = 0;
   Actual->x+= SUMX;
   Actual->y+= SUMY;
   Actual = Actual->Hijo;
  }

//************************************************************************//
/*                                                                        */
/*                                                                        */
  if(Inicial->x >(Inicial->rad)) ActualizarInicial(SUMX,SUMY);
  else
  if(Inicial->x < -(Inicial->rad)) ActualizarInicial(SUMX,SUMY);
  else
  if(Inicial->y >(Inicial->rad)) ActualizarInicial(SUMX,SUMY);
  else
  if(Inicial->y < -(Inicial->rad)) ActualizarInicial(SUMX,SUMY);



  Actual=Inicial->Hijo;
  while( Actual != NULL )
  {
   if(Actual->x >(Actual->rad)) Actual->Actualizar(SUMX,SUMY);
   if(Actual->x < -(Actual->rad)) Actual->Actualizar(SUMX,SUMY);
   if(Actual->y >(Actual->rad)) Actual->Actualizar(SUMX,SUMY);
   if(Actual->y < -(Actual->rad)) Actual->Actualizar(SUMX,SUMY);
   Actual = Actual->Hijo;
  }


//Arregla los difx, y los dify
  Actual = Inicial;
  while(Actual != Final)
  {
   if(Actual->Actualizado==1)
   {
   Actual->Hijo->dify =(int)(Actual->Hijo->y - Actual->y)/(Actual->rad);
   Actual->Hijo->difx =(int)(Actual->x - Actual->Hijo->x)/(Actual->rad);
   }
   Actual = Actual->Hijo;

  }

/*calcula la posicion relativa al mapa completo del centro de cada grilla                                                                        */
/**************************************************************************/

  Actual = Inicial->Hijo;
  while( Actual != NULL )
  {
   Actual->PosRealCentroX =((Actual->difx) *(Actual->radm2)) +(Actual->Padre->PosRealCentroX);
   Actual->PosRealCentroY =((Actual->dify) *(Actual->radm2)) +(Actual->Padre->PosRealCentroY);
   Actual->PosRealCentroY = ldiv(Actual->PosRealCentroY,(Mapa->MapaAlto*Mapa->DistanciaEntrePuntos)).rem;
   Actual->PosRealCentroX = ldiv(Actual->PosRealCentroX,(Mapa->MapaAncho*Mapa->DistanciaEntrePuntos)).rem;
   if(Actual->PosRealCentroY<0) Actual->PosRealCentroY+=Mapa->MapaAlto*Mapa->DistanciaEntrePuntos;
   if(Actual->PosRealCentroX<0) Actual->PosRealCentroX+=Mapa->MapaAncho*Mapa->DistanciaEntrePuntos;
   Actual = Actual->Hijo;
  }

  Actual=Inicial;
  while(Actual!=NULL)
  {
  Actual->Animacion(Inicial,Mapa);
  Actual=Actual->Hijo;
  }

  DistanciaAlRelieve = CAM->Oz -(float) Final->GP[Final->Tamanio/2].alt;
if(DistanciaAlRelieve<500.0) {CAM->Oz =(float) Final->GP[Final->Tamanio/2].alt + 500.0;}
  DistanciaAlRelieve = CAM->Oz - Final->GP[Final->Tamanio/2].alt;
}

//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//-----------------------------------------------------------------------------------------

/*
Esta funcion  actualiza la grilla inicial(la mas grande) con
los datos del Mapa
*/


void
TGrid_Utility::ActualizarInicial(int SUMX,int SUMY)
{
  int repx,repy;
  int i,j,q,k,qmod,kmod;

  Inicial->Actualizado = 1;

  repx = SUMX / Inicial->radm2;
  repy = SUMY / Inicial->radm2;
  Inicial->x = Inicial->x - repx * Inicial->radm2;
  Inicial->y = Inicial->y - repy * Inicial->radm2;

  if(Inicial->x > Inicial->rad) {Inicial->x-=Inicial->radm2; repx++;}
  if(Inicial->x < -Inicial->rad) {Inicial->x+=Inicial->radm2; repx--;}
  if(Inicial->y > Inicial->rad) {Inicial->y-=Inicial->radm2; repy++;}
  if(Inicial->y < -Inicial->rad) {Inicial->y+=Inicial->radm2; repy--;}

  Mapa->PosicionAnteriorX = Mapa->PosicionX;
  Mapa->PosicionAnteriorY = Mapa->PosicionY;

  Mapa->PosicionX =   Mapa->PosicionX+repx;
  if(Mapa->PosicionX<0) Mapa->PosicionX+=Mapa->MapaAncho;
  if(Mapa->PosicionX>=Mapa->MapaAncho) Mapa->PosicionX-=Mapa->MapaAncho;

  Mapa->PosicionY = Mapa->PosicionY-repy;
  if(Mapa->PosicionY<0) Mapa->PosicionY+=Mapa->MapaAlto;
  if(Mapa->PosicionY>=Mapa->MapaAlto) Mapa->PosicionY-=Mapa->MapaAlto;

  Inicial->PosRealCentroX = Mapa->PosicionX*Mapa->DistanciaEntrePuntos;
  Inicial->PosRealCentroY = Mapa->PosicionY*Mapa->DistanciaEntrePuntos;

  for(j=0,q=Mapa->PosicionY-Inicial->dvs/2; j<Inicial->dvspp; j++,q++)
  for(i=0,k=Mapa->PosicionX-Inicial->dvs/2; i<Inicial->dvspp; i++,k++)
      {
      qmod = q;
      if(qmod<0) qmod = qmod + Mapa->MapaAlto;
      if(qmod>=Mapa->MapaAlto) qmod = qmod - Mapa->MapaAlto;

      kmod = k;
      if(kmod<0) kmod = kmod + Mapa->MapaAncho;
      if(kmod>=Mapa->MapaAncho) kmod = kmod - Mapa->MapaAncho;

      Inicial->G[j][i] = Mapa->MapaDatos[qmod][kmod];
	  //asigno coordenadas de textura....
	  tdato2 tgp;
	  tgp.txa =(float) kmod /(float)(Mapa->MapaAncho-1); 
	  tgp.tya =(float) qmod /(float)(Mapa->MapaAlto-1);
	  tgp.txb =((float)(kmod+1) /(float)(Mapa->MapaAncho-1));
	  tgp.tyb =((float)(qmod+1) /(float)(Mapa->MapaAlto-1));
	  Inicial->L_GP[j][i] = tgp;
      //Inicial->GT[i+j*Inicial->dvspp] = Inicial->texturas[kmod+qmod*Mapa->MapaAncho];
      }

}

//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//-----------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

void
TGrid_Utility::ProyectaryDibujar(TBITMAP *paginaalturas,TCAMARA3D *CAM)
{ 

int ar;
int contador;


ar = 0;

struct vecteurtype uaux,vaux,waux;

       uaux = CAM->u;
       vaux = CAM->v;
       waux = CAM->w;
/*
       if(key[KEY_F1]) { CAM->u.x = -vaux.x; CAM->u.y = -vaux.y; CAM->u.z = -vaux.z;
                         CAM->v = uaux;}
       if(key[KEY_F2]) { CAM->v.x = -uaux.x; CAM->v.y = -uaux.y; CAM->v.z = -uaux.z;
                         CAM->u = vaux;}
       if(key[KEY_F3]) { CAM->v.x = -vaux.x; CAM->v.y = -vaux.y; CAM->v.z = -vaux.z;
                          CAM->u.x = -uaux.x; CAM->u.y = -uaux.y; CAM->u.z = -uaux.z;}
       if(key[KEY_F4]) { CAM->u = waux;
                          CAM->w.x = -uaux.x; CAM->w.y = -uaux.y; CAM->w.z = -uaux.z;}
       if(key[KEY_F5]) { CAM->w = uaux;
                          CAM->u.x = -waux.x; CAM->u.y = -waux.y; CAM->u.z = -waux.z;}
*/
/*
Determina el area del frustrum a la que corresponde, asi decido
el orden en que dibujo los triangulos
*/
if(CAM->u.x>=0 && CAM->u.y>=0)
     {
      if(CAM->u.y<=CAM->u.x) ar=1;
      if(CAM->u.y>CAM->u.x) ar=2;
     }
else
if(CAM->u.x<=0 && CAM->u.y>=0)
     {
      if(-CAM->u.x<=CAM->u.y) ar=3;
      if(-CAM->u.x>CAM->u.y) ar=4;
     }
else
if(CAM->u.x<=0 && CAM->u.y<=0)
     {
      if(-CAM->u.y<=-CAM->u.x) ar=5;
      if(-CAM->u.y>-CAM->u.x) ar=6;
     }
else
if(CAM->u.x>=0 && CAM->u.y<=0)
     {
      if(CAM->u.x<=-CAM->u.y) ar=7;
      if(CAM->u.x>-CAM->u.y) ar=8;
     }

Actual = Inicial;
for(contador = 0; contador<Inhibir; contador++)
{
 if(Actual->Hijo!=NULL) Actual = Actual->Hijo;
}
while(Actual!=Final)
 {
     if(DistanciaAlRelieve <(Actual->rad * 400))
     {

       if(DistanciaAlRelieve <(Actual->Hijo->rad * 400))
       {
           Actual->EnRango = 1;
           if(!SafeMode) Actual->ProyectarBorde(Inicial,Mapa,CAM->Oz,CAM);
           else Actual->ProyectarBorde_SafeMode(Inicial,Mapa,CAM->Oz,CAM);

           Actual->CalcularNormalesBorde();
           Actual->DibujarAlturaBorde(ar,paginaalturas);
       }
       else {



           if(!SafeMode) Actual->ProyectarTodo(Inicial,Mapa,CAM->Oz,CAM);
           else Actual->ProyectarTodo_SafeMode(Inicial,Mapa,CAM->Oz,CAM);

           Actual->CalcularNormalesTodo();
           Actual->DibujarAlturaTodo(ar,paginaalturas);

           Actual->EnRango = 1;
           Actual->Hijo->EnRango = 0;
           CAM->u = uaux;
           CAM->v = vaux;
           CAM->w = waux;
           return;
           }
     }

      Actual = Actual->Hijo;
  }

if(DistanciaAlRelieve <(Final->rad * 400))
{
if(!SafeMode) Actual->ProyectarTodo(Inicial,Mapa,CAM->Oz,CAM);
else Actual->ProyectarTodo_SafeMode(Inicial,Mapa,CAM->Oz,CAM);
Final->CalcularNormalesTodo();
Final->DibujarAlturaTodo(ar,paginaalturas);
Final->EnRango = 1;
}

CAM->u = uaux;
CAM->v = vaux;
CAM->w = waux;
}

//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//-----------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

void
TGrid_Utility::glProyectaryDibujar(TBITMAP *paginaalturas,TCAMARA3D *CAM)
{ 

int ar;
int contador;
float ratio = 1.0*(1024.0f/768.0f);

ar = 0;

struct vecteurtype uaux,vaux,waux;

       uaux = CAM->u;
       vaux = CAM->v;
       waux = CAM->w;
/*
       if(key[KEY_F1]) { CAM->u.x = -vaux.x; CAM->u.y = -vaux.y; CAM->u.z = -vaux.z;
                         CAM->v = uaux;}
       if(key[KEY_F2]) { CAM->v.x = -uaux.x; CAM->v.y = -uaux.y; CAM->v.z = -uaux.z;
                         CAM->u = vaux;}
       if(key[KEY_F3]) { CAM->v.x = -vaux.x; CAM->v.y = -vaux.y; CAM->v.z = -vaux.z;
                          CAM->u.x = -uaux.x; CAM->u.y = -uaux.y; CAM->u.z = -uaux.z;}
       if(key[KEY_F4]) { CAM->u = waux;
                          CAM->w.x = -uaux.x; CAM->w.y = -uaux.y; CAM->w.z = -uaux.z;}
       if(key[KEY_F5]) { CAM->w = uaux;
                          CAM->u.x = -waux.x; CAM->u.y = -waux.y; CAM->u.z = -waux.z;}
*/
/*
Determina el area del frustrum a la que corresponde, asi decido
el orden en que dibujo los triangulos
*/
if(CAM->u.x>=0 && CAM->u.y>=0)
     {
      if(CAM->u.y<=CAM->u.x) ar=1;
      if(CAM->u.y>CAM->u.x) ar=2;
     }
else
if(CAM->u.x<=0 && CAM->u.y>=0)
     {
      if(-CAM->u.x<=CAM->u.y) ar=3;
      if(-CAM->u.x>CAM->u.y) ar=4;
     }
else
if(CAM->u.x<=0 && CAM->u.y<=0)
     {
      if(-CAM->u.y<=-CAM->u.x) ar=5;
      if(-CAM->u.y>-CAM->u.x) ar=6;
     }
else
if(CAM->u.x>=0 && CAM->u.y<=0)
     {
      if(CAM->u.x<=-CAM->u.y) ar=7;
      if(CAM->u.x>-CAM->u.y) ar=8;
     }

Actual = Inicial;
for(contador = 0; contador<Inhibir; contador++)
{
 if(Actual->Hijo!=NULL) Actual = Actual->Hijo;
}

while(Actual!=Final)
 {
     if(DistanciaAlRelieve <(Actual->rad * 400))
     {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45,ratio,Actual->rad * 1.0f,Actual->rad * 1000000.0f);
		glMatrixMode(GL_MODELVIEW);

       if(DistanciaAlRelieve <(Actual->Hijo->rad * 400))
       {
           Actual->EnRango = 1;
		   Actual->glProyectarBorde(Inicial,Mapa,CAM->Oz,CAM);
           Actual->CalcularNormalesBorde();
           Actual->glDibujarAlturaBorde(ar,paginaalturas,CAM);
       }
       else {
		   Actual->glProyectarTodo(Inicial,Mapa,CAM->Oz,CAM);
		   Actual->CalcularNormalesTodo();
           Actual->glDibujarAlturaTodo(ar,paginaalturas,CAM);

           Actual->EnRango = 1;
           Actual->Hijo->EnRango = 0;
           CAM->u = uaux;
           CAM->v = vaux;
           CAM->w = waux;
           return;
           }
     }

      Actual = Actual->Hijo;
  }

if(DistanciaAlRelieve <(Final->rad * 400))
{
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45,ratio,Final->rad * 1.0f,Final->rad * 1000000.0f);
glMatrixMode(GL_MODELVIEW);
Final->glProyectarTodo(Inicial,Mapa,CAM->Oz,CAM);
Final->CalcularNormalesTodo();
Final->glDibujarAlturaTodo(ar,paginaalturas,CAM);
Final->EnRango = 1;
}

CAM->u = uaux;
CAM->v = vaux;
CAM->w = waux;
}


//-----------------------------------------------------------------------------------------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//*----------*----------*----------*----------*----------*----------*----------
//-----------------------------------------------------------------------------------------
//---------------------------------------------------------------------------


void
TGrid_Utility::Posicionar(int map_x,int map_y, float altitud, TCAMARA3D *CAM)
{
 Mapa->PosicionX = map_x - Inicial->dvspp;
 Mapa->PosicionY = map_y;
 Inicial->PosRealCentroX = Mapa->PosicionX*Mapa->DistanciaEntrePuntos;
 Inicial->PosRealCentroY = Mapa->PosicionY*Mapa->DistanciaEntrePuntos;

 inivecteurs(0.0,0.0,altitud,CAM);

 ActualizarGrillas(Inicial->dvspp*Inicial->radm2,CAM);

 CAM->u.x = -sqrt(2.0)/2;
 CAM->u.y = sqrt(2.0)/2;
 CAM->v.x = CAM->u.x;
 CAM->v.y = - CAM->u.y;
}


