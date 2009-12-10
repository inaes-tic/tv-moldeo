#include "supereg/tmapa.h"

TMapa::TMapa()
{
Memoria_Utilizada = 0;
CargarMapa();
}


TMapa::~TMapa()
{
 delete[] MapaDatos[0];
 delete[] MapaDatos;
}


void
TMapa::CalcularBitMapa(void)
{
/*
  int i,j,coefcolor;
  unsigned char *colorcito;
  TIPO *Eldato;

  coefcolor =(AlturaMaxima - AlturaMinima ) / 255;

  for(j = 0 ; j < BitMapa->h ; j++)
    {
     Eldato = MapaDatos[j];
     colorcito = BitMapa->line[j];
     for( i = 0 ; i < BitMapa->w; i++,Eldato++,colorcito++)
      {
     (*colorcito) =(Eldato->alt / coefcolor) + 55;
      }
    }
	*/
}

void
TMapa::CargarMapa(void)
{
    FILE *miarchivo;
    int i;

    //miarchivo = fopen("mapa\\mapa.mp", "rb");
    miarchivo = fopen("data\\supereg\\mapa\\mapax.mp", "rb");
    //miarchivo = fopen("mapa\\mapa3.mp", "rb");
    //miarchivo = fopen("mapa\\mapa2.mp", "rb");

    fread(NombreMapa,sizeof(char),80,miarchivo);

    ContadorDeMapas = NombreMapa[79];

    fread(&MapaAncho,sizeof(int),1,miarchivo);
    fread(&MapaAlto,sizeof(int),1,miarchivo);

    Tamanio = MapaAncho*MapaAlto;
    MapaDatos = new TIPOP [MapaAlto]; 
    MapaDatos[0] = new TIPO [Tamanio];
    for(i=0;i<MapaAlto;i++) MapaDatos[i] = &MapaDatos[0][i*MapaAncho];

    Memoria_Utilizada+= Tamanio*sizeof(TIPO);
    Memoria_Utilizada+= MapaAlto*sizeof(TIPO*);


    fread(&AlturaMaxima,sizeof(int),1,miarchivo);
    fread(&AlturaMinima,sizeof(int),1,miarchivo);
    fread(&AlturaAgua,sizeof(int),1,miarchivo);
    fread(NombreArchivoPaleta,sizeof(char),12,miarchivo);
    fread(Paleta,sizeof(char),768,miarchivo);
    fread(MapaDatos[0],sizeof(TIPO),Tamanio,miarchivo);//lee el paquete entero!!!




    PosicionX = MapaAncho / 2;
    PosicionY = MapaAlto / 2;
    PosicionAnteriorX = PosicionX;
    PosicionAnteriorY = PosicionY;

    fclose(miarchivo);

    //Correccion
    MapaAncho = MapaAlto;
    CalcularConstantes();
}

void
TMapa::CargarMapa(char *elnombre)
{
    FILE *miarchivo;
    int i;

    miarchivo = fopen(elnombre, "rb");
         
    fread(NombreMapa,sizeof(char),80,miarchivo);

    ContadorDeMapas = NombreMapa[79];

    fread(&MapaAncho,sizeof(int),1,miarchivo);
    fread(&MapaAlto,sizeof(int),1,miarchivo);

    Tamanio = MapaAncho*MapaAlto;
    MapaDatos = new TIPOP [MapaAlto];
    MapaDatos[0] = new TIPO [Tamanio];
    for(i=0;i<MapaAlto;i++) MapaDatos[i] = &MapaDatos[0][i*MapaAncho];

    Memoria_Utilizada+= Tamanio*sizeof(TIPO);
    Memoria_Utilizada+= MapaAlto*sizeof(TIPO*);


    fread(&AlturaMaxima,sizeof(int),1,miarchivo);
    fread(&AlturaMinima,sizeof(int),1,miarchivo);
    fread(&AlturaAgua,sizeof(int),1,miarchivo);
    fread(NombreArchivoPaleta,sizeof(char),12,miarchivo);
    fread(Paleta,sizeof(char),768,miarchivo);
    fread(MapaDatos[0],sizeof(TIPO),Tamanio,miarchivo);

    PosicionX = MapaAncho / 2;
    PosicionY = MapaAlto / 2;
    PosicionAnteriorX = PosicionX;
    PosicionAnteriorY = PosicionY;

    fclose(miarchivo);

    CalcularConstantes();

    //set_color_depth(FORMATOCOLOR);
    //BitMapa = create_bitmap(MapaAncho,MapaAlto);
  //  Memoria_Utilizada+= Tamanio;
//    CalcularBitMapa();
}


void
TMapa::SalvarMapa(void)
{
    FILE *miarchivo;
    char *cont;
    char nombre[80] = "mapa\\m";

	cont = NULL;
    ContadorDeMapas++;
    NombreMapa[79]=ContadorDeMapas;

    itoa(ContadorDeMapas,cont,10);
    strcat(nombre,cont);
    strcat(nombre,".mp");
    printf("%s",nombre);

    miarchivo = fopen(nombre, "wb");

    fwrite(NombreMapa,sizeof(char),80,miarchivo);
    fwrite(&MapaAncho,sizeof(int),1,miarchivo);
    fwrite(&MapaAlto,sizeof(int),1,miarchivo);
    fwrite(&AlturaMaxima,sizeof(int),1,miarchivo);
    fwrite(&AlturaMinima,sizeof(int),1,miarchivo);
    fwrite(&AlturaAgua,sizeof(int),1,miarchivo);

    fwrite(NombreArchivoPaleta,sizeof(char),12,miarchivo);
    fwrite(Paleta,sizeof(char),768,miarchivo);
    fwrite(MapaDatos[0],sizeof(TIPO),Tamanio,miarchivo);

    fclose(miarchivo);

}


void
TMapa::AdecuarMapa(TBITMAP **T)
{
 int i,j;

 for(i=0; i<MapaAncho; i++)
 for(j=0; j<MapaAlto; j++)
 {
 //MapaDatos[j][i].aspereza =(char) T[i+j*MapaAncho]->line[0][0];
 MapaDatos[j][i].alt =(int)(MapaDatos[j][i].aspereza * 4096);
 }
}


void
TMapa::CargarTexturaMapa(TPALETA *PAL)
{
/*
 V3D_f v1,v2,v3,v4;
 BITMAP *TEXTMAP;

 TEXTMAP = load_bitmap("mapa\\mitult7.pcx",PAL->RGBPaletaGlobal); //este de 16

 if(TEXTMAP==NULL)  exit(0);

 texturas = new(BITMAP *) [4096];

 if(texturas==NULL) exit(0);

 int i,j;
 for(j=0;j<MapaAlto;j++)
 for(i=0;i<MapaAncho;i++)
 {
  texturas[i+j*MapaAncho] = create_bitmap(NTEX,NTEX);
  if(texturas[i+j*MapaAncho]==NULL) exit(0);
  Memoria_Utilizada+= NTEX * NTEX ;
  Memoria_Utilizada+= sizeof(BITMAP);
  stretch_blit(TEXTMAP,texturas[i+j*MapaAncho],i*NTEX,j*NTEX,NTEX,NTEX,0,0,NTEX,NTEX);
  //blit(texturas[i+j*64],screen,0,0,i*8,j*8,screen->w,screen->h);

v1.x=i*NTEX;
v1.y=j*NTEX;
v1.u = 0.0;
v1.v = 0.0;

v2.x =(i+1)*NTEX;
v2.y = j*NTEX;
v2.u = 7.0;
v2.v = 0.0;

v3.x = i*NTEX;
v3.y =(j+1)*NTEX;
v3.u = 0.0;
v3.v = 7.0;

v4.x =(i+1)*NTEX;
v4.y =(j+1)*NTEX;
v4.u = 7.0;
v4.v = 7.0;
                                    
triangle3d_f(screen,POLYTYPE_ATEX,texturas[i+j*MapaAncho],&v1,&v2,&v3);
triangle3d_f(screen,POLYTYPE_ATEX,texturas[i+j*MapaAncho],&v2,&v3,&v4);
}

blit(TEXTMAP,screen,0,0,0,0,screen->w,screen->h);
rest(1000);
AdecuarMapa(texturas);

destroy_bitmap(TEXTMAP);
rest(500);
*/
}



void
TMapa::DibujarMapa(TBITMAP *pagina)
{
 //blit(BitMapa,pagina,0,0,0,0,BitMapa->w,BitMapa->h);
}

void
TMapa::MostrarDatos(void)
{
	 if(MapaDatos==NULL) return;
	 printf("El Nombre del mapa: %s\n",NombreMapa);
	 printf(" Ancho del Mapa: %i\n",MapaAncho);
	 printf(" Alto del Mapa: %i\n",MapaAlto);
	 printf(" Posicion del Observador X,Y: %i,%i\n",PosicionX,PosicionY);
	 printf(" Altura Maxima del Relieve: %i\n",AlturaMaxima);
	 printf(" Altura Minima del Relieve: %i\n",AlturaMinima);
	 printf(" Altura del Mar Absoluta: %i\n",AlturaAgua);
	 printf(" Nombre del Archivo de la Paleta a Usar: %s\n",NombreArchivoPaleta);
}


void
TMapa::CalcularConstantes(void)
{
 DistanciaEntrePuntos =(int) pow(2.0,17)*METRO;

 SuperficieTotal = MapaAncho*MapaAlto; //S = h * w

 SuperficieTotalMedio = SuperficieTotal/2; // S' = S / 2

 RadioCirculoEnMapa =(int) sqrt(SuperficieTotalMedio/PI);// PI * r*r = S'

 DimensionMatrizEnMapa = RadioCirculoEnMapa * 2 + 1; //

 RadioDelPlaneta =(int)((float) pow(SuperficieTotal/(4*PI),0.33));

 radiop =(float) RadioDelPlaneta *(float) DistanciaEntrePuntos;

 radioc =(float) RadioCirculoEnMapa *(float) DistanciaEntrePuntos;

 factorradiopradioc = 1.0; //(float)(radiop/radioc);
}

