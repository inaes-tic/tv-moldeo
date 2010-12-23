#include "../../inc/supereg/trdrmap.h"

TRadarMapa::TRadarMapa(TMapa *M,int x,int y, int ancho, int alto)
{
/*
set_color_depth(FORMATOCOLOR);

 RadarBitMapa = create_bitmap(ancho,alto);
 RadarBitMapaPantallaCompleta = create_bitmap(SCREEN_W,SCREEN_H);

 Memoria_Utilizada+= ancho*alto;
 Memoria_Utilizada+= SCREEN_W*SCREEN_H;
//cursorpal = new RGB [256];
//CursorObs = load_bmp('bmp\\elcursor.bmp',cursorpal);
 Activado = 0;
 Posx = x; Posy = y; Zoom = 1;
 contador_de_ciclos = 0;
 Inicializar(M);
 Activar(1);
 */
}

TRadarMapa::~TRadarMapa()
{
/*
destroy_bitmap(RadarBitMapa);
 destroy_bitmap(RadarBitMapaPantallaCompleta);
 */
// delete(cursorpal);
}


void
TRadarMapa::Activar(int state)
{
 if(state==0) Activado = 0;
 if(state==1) Activado = 1;
 if(state==2) Activado = 2;
}

void
TRadarMapa::Inicializar(TMapa *M)
{
 Actualizado(M);
}


void
TRadarMapa::Actualizado(TMapa *M)
{
 M->CalcularBitMapa();
/*
 stretch_blit(M->BitMapa,RadarBitMapaPantallaCompleta,
              0,0,M->BitMapa->w,M->BitMapa->h,
              0,0,RadarBitMapaPantallaCompleta->w,RadarBitMapaPantallaCompleta->h);

 stretch_blit(M->BitMapa,RadarBitMapa,
              0,0,M->BitMapa->w,M->BitMapa->h,
              0,0,RadarBitMapa->w,RadarBitMapa->h);
              */
}



void
TRadarMapa::Zoomeado(TMapa *M,char mas)
{
/*int X_Start,Y_Start,ancho,alto;
 int w = M->MapaAncho;
 int h = M->MapaAlto;

 Zoom+= mas;

 if(Zoom<1) Zoom = 1;

 if(Zoom>10) Zoom = 10;

 ancho = w / Zoom;

 alto = h / Zoom;

 X_Start =((w - ancho) / 2) +(M->PosicionX/Zoom);

 Y_Start =((h - alto) / 2) +(M->PosicionY/Zoom);

 stretch_blit(M->BitMapa,RadarBitMapa,
              X_Start,Y_Start,ancho,alto,
              0,0,RadarBitMapa->w,RadarBitMapa->h);*/
}
/*

void
TRadarMapa::MostrarRadarBitMapa(BITMAP *pagina)
{

 blit(RadarBitMapa,pagina,0,0,Posx,Posy,RadarBitMapa->w,RadarBitMapa->h);

}

void
TRadarMapa::MostrarRadarBitMapaPantallaCompleta(BITMAP *pagina)
{
 blit(RadarBitMapaPantallaCompleta,pagina,0,0,0,0,RadarBitMapaPantallaCompleta->w,RadarBitMapaPantallaCompleta->h);

}

void
TRadarMapa::Activo(TMapa *M,BITMAP *pagina)
{

 contador_de_ciclos++;

 MostrarContador(pagina);

 if(Activado==0)  return;

 if(Activado==1)
 {
 if(Zoom>1)
 {
  if(M->PosicionAnteriorX != M->PosicionX || M->PosicionAnteriorY != M->PosicionY)
  Zoomeado(M,0);

  MostrarRadarBitMapa(pagina);
 }
 else {MostrarRadarBitMapa(pagina);
       MostrarObservador(M,pagina);}
 }

 if(Activado==2)
 {
  MostrarRadarBitMapaPantallaCompleta(pagina);
  MostrarObservadorPantallaCompleta(M,pagina);

 }

}


void
TRadarMapa::MostrarObservador(TMapa *M,BITMAP *pagina)
{

 float coefx,coefy;
 int x,y,xGo,yGo;
 coefx =(float) RadarBitMapa->w / M->MapaAncho;
 coefy =(float) RadarBitMapa->h / M->MapaAlto;
 x =(int)(Posx +(float) M->PosicionX * coefx);
 y =(int)(Posy +(float) M->PosicionY * coefy);
 xGo =(int)(x +(float)(20*u.x));
 yGo =(int)(y -(float)(20*u.y));
 circle(pagina,x,y,20,(char)(contador_de_ciclos % 256));
 line(pagina,x,y,xGo,yGo,(char)(contador_de_ciclos % 256));

}


void
TRadarMapa::MostrarContador(BITMAP *pagina)
{

 textprintf(pagina,font,0,0,200,"frames: %i",contador_de_ciclos);
 textprintf(pagina,font,0,10,200,"Poligonos: %i",Poligonos);
 Poligonos = 0;


}


void
TRadarMapa::MostrarObservadorPantallaCompleta(TMapa *M,BITMAP *pagina)
{
 float coefx,coefy;
 int x,y,xGo,yGo;
 coefx = RadarBitMapaPantallaCompleta->w / M->MapaAncho;
 coefy = RadarBitMapaPantallaCompleta->h / M->MapaAlto;
 x =(int)(M->PosicionX * coefx);
 y =(int)(M->PosicionY * coefy);
 xGo =(int)(x +(float)(50*u.x));
 yGo =(int)(y -(float)(50*u.y));
 circle(pagina,x,y,50,(char)(rand()));
 line(pagina,x,y,xGo,yGo,(char)(rand()));
}
*/



