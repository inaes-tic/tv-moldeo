#include "supereg/pcx.h"

PcxFile pcxGlobal;
//***********************************************************************

//***********************************************************************

//***********************************************************************

//***********************************************************************

//***********************************************************************

//***********************************************************************


/*
  
  char *Read_PCX(char *filename, char PcxPal[768],char EcoData)
  
  
  Carga un archivo pcx del disco                        
  Lo descomprime utilizando el algoritmo convencional de RLE
  Y devuelve el untero a pcxGlobal>bitma que contiene la imagen
    
    
  EXCLUSIVAMENTE ARA IMAGENES DE ALETA NDEXADA DE 256 COLORES!!
          
      
      arametros: char *filename  ; El nombre del archivo
                 char PcxPal[768]; Donde guarda la aleta
                 char EcoData; Muestra en antalla el contenido del header
                               0 > lo muestra
                               1 > no lo muestra

*/

char *Read_PCX(char *filename, char PcxPal[768],char EcoData)
{

  unsigned long i;
  char abyte;
  char *p;
  FILE *f;
  PcxFile *pcx;

  pcx = &pcxGlobal;

  f = fopen(filename,"rb");

  if(f==NULL)
  {
  printf("\n no se encontro el archivo!! %s\n",filename);
  getch();
  exit(0);
  }

  fread(&pcx->hdr,sizeof(PcxHeader),1,f);

  pcx->width = 1 + pcx->hdr.xmax - pcx->hdr.xmin;

  if( pcx->hdr.bytes_per_line > pcx->width ) pcx->width = pcx->hdr.bytes_per_line;

  pcx->height = 1 + pcx->hdr.ymax - pcx->hdr.ymin;
  pcx->imagebytes =(unsigned int)(pcx->width*pcx->height);
  pcx->bitmap = new char [pcx->imagebytes+4];

  if(pcx->bitmap == NULL)
  {
  printf("\n no hay suficiente memoria!! \n");
  getch();
  exit(0);
  }

  p = &pcx->bitmap[4];

  unsigned int  pcxcont = 0;
  char reps;

  while(pcxcont<pcx->imagebytes)
    {

        abyte = fgetc(f);
        if(abyte>192)
        {
            reps = abyte & 0x3f;
            pcxcont+=reps;
            abyte = fgetc(f);
            while(reps--)
                *p++=abyte;
        }
        else
        {
            *p++=abyte;
            pcxcont++;
        }
    }

  fseek(f,-768L,SEEK_END);
  fread(PcxPal,768,1,f);

  for( i = 0 ; i < 768 ; i++) PcxPal[i] = PcxPal[i] >> 2;

  p = pcx->bitmap;
 (*(unsigned short *)p) = pcx->width;
  p += sizeof(unsigned short);
 (*(unsigned short *)p) = pcx->height;

  fclose(f);

  if(EcoData==1)
  {
   printf("pcx->hdr.manufacturer:%i\n",pcx->hdr.manufacturer);
   printf("pcx->hdr.version:%i\n",pcx->hdr.version);
   printf("pcx->hdr.encoding:%i\n",pcx->hdr.encoding);
   printf("pcx->hdr.color_planes:%i\n",pcx->hdr.color_planes);
   printf("pcx->hdr.xmax:%i\n",pcx->hdr.xmax);
   printf("pcx->hdr.xmin:%i\n",pcx->hdr.xmin);
   printf("pcx->hdr.ymax:%i\n",pcx->hdr.ymax);
   printf("pcx->hdr.ymin:%i\n",pcx->hdr.ymin);
   printf("pcx->height:%i\n",pcx->height);
   printf("pcx->width:%i\n",pcx->width);
   printf("pcx->imagebytes:%i\n",pcx->imagebytes);
   printf("pcx->hdr.bytes_per_line:%i\n",pcx->hdr.bytes_per_line);
   printf("pcx->hdr.hres:%i\n",pcx->hdr.hres);
   printf("pcx->hdr.vres:%i\n",pcx->hdr.vres);
   printf("pcx->hdr.reserved:%i\n",pcx->hdr.reserved);
   printf("pcx->hdr.palette_type:%i\n",pcx->hdr.palette_type);
   printf("pcx->hdr.hscreensize:%i\n",pcx->hdr.hscreensize);
   printf("pcx->hdr.vscreensize:%i\n",pcx->hdr.vscreensize);

   getch();
  }

  return(pcx->bitmap);
}

//***********************************************************************

//***********************************************************************

//***********************************************************************

//***********************************************************************

//***********************************************************************

//***********************************************************************

/*  
  
  void Save_PCX(char *buffer, short width, short height,char palette[768],
                char mode_color, char *filename)
  
  Comprime utilizando el algoritmo convencional de RLE
  Y crea el archivo pcx

  EXCLUSIVAMENTE PARA IMAGENES DE PALETA INDEXADA DE 256 COLORES!!
          
      
     parametros: char *buffer; el buffer del cual se van a tomar los datos 
                 short width; ancho de la imagen 
                 short height; alto de la imagen
                 char palette[768]; aleta de colores
                 char mode_color; si es 1 el modo es 64*64*64(y lo va a pasar a 256*256*256)
                                  si es 0 el modo es 256*256*256
                 char *filename  ; El nombre del archivo que se va a grabar
*/

void Grabar_PCX(char *buffer, short width, short height,char palette[768],char mode_color, char *filename)
{

unsigned long i;
char color_byte;
char counter_byte;
long counter_line;
//char *p;
FILE *f;
PcxFile *pcx;

pcx = &pcxGlobal;

pcx->hdr.manufacturer = 10;  //Paintbrush
pcx->hdr.version = 5;        //Comun
pcx->hdr.encoding = 1;    //RLE comun
pcx->hdr.bits_per_pixel = 8;   //colores indexados(paleta de 256 colores)
pcx->hdr.xmin = 0;
pcx->hdr.ymin = 0;
pcx->hdr.xmax = width - 1;
pcx->hdr.ymax = height - 1;
pcx->hdr.hres = 75;     //esto generalmente para photoshop son(dpi) o sea dots per inch
pcx->hdr.vres = 75;
for(i=0;i<48;i++) pcx->hdr.palette16[i] = 0;
pcx->hdr.reserved = 0;  //no se usa
pcx->hdr.color_planes = 1;  //1 solo plano
pcx->hdr.bytes_per_line = width;
pcx->hdr.palette_type = 1;   //paleta de 256 colores
pcx->hdr.hscreensize = 640;
pcx->hdr.vscreensize = 480;

for(i=0;i<54;i++) pcx->hdr.filler[i] = 0;   //no se usa

f = fopen(filename,"wb");

fwrite(&pcx->hdr,sizeof(PcxHeader),1,f);

pcx->imagebytes = width * height ;
counter_line = 0;
i = 0;
char reps;


while(i<pcx->imagebytes)
{
   reps = 0;
   //tomamos el color de referencia
   color_byte = buffer[i];

   //vemos cuantos bytes sucesivos hay del mismo
   while(buffer[i]==color_byte && i<pcx->imagebytes)
   { reps++;
     if(reps==64) {reps--;break;} // se paso del maximo que es 63,lo vuelvo atras
     else {i++;counter_line++;} //sigo

     if(counter_line==pcx->hdr.bytes_per_line) {counter_line=0; break;}
   }
   //el indice i queda apuntando al proximo color para la proxima pasada
   //ya no necesitamos incrementarlo

   if(reps==1)   //no hay repetidos
   { if(color_byte<192) fputc(color_byte,f);
     else { fputc(192+1,f);
            fputc(color_byte,f);
           }
    }

    if(reps>1)
    {
      counter_byte = 0xC0 | reps ; //192 + reps
      fputc(counter_byte,f);
      fputc(color_byte,f);
    }
}

fputc(0x0C,f);

//salvamos la paleta

if(mode_color == 0) //modo 0 ->256*256*256
fwrite(&palette,768,1,f);
else        //modo 1 ->64*64*64 se pasa a 256*256*256
for(i=0;i<768;i++)
{
 fputc((char)(palette[i] << 2),f);
}

fclose(f);
}

/*

typedef struct
{ unsigned short ancho,alto;
  char *bitmap_char;
} font;

*/

///////////////////////////////////////////////////////////////
//
//   De aca en adelante hay que rehacer algunas cositas
//   para la proxima va estar listo....
//   Consiste en un par de funciones para manejar tipografias
//   graficas creadas en base a un archivo pcx......
//
///////////////////////////////////////////////////////////////

/*

font tipografia[256];

void LoadFont(char *filename)
{
 char *image;
 int i,j;
 image = Read_PCX(filename);
//arreglamos el bitmap pcx

for(i = 0; i<pcxGlobal.imagebytes; i++)
{
if(image[4+i] != 0xff) image[4+i] = 0;
}

/////seteamos los tamanios de los caracteres///////////

 for(i = 0;i<256;i++) tipografia[i].alto = tipografia[i].ancho = 4;

 //los numeros
 tipografia['0'].ancho = 4;
 tipografia['1'].ancho = 3;
 for(i='2'; i<='9'; i++) tipografia[i].ancho = 4;
 //las letras
 for(i='a'; i<='e'; i++) tipografia[i].ancho = 4;
 tipografia['f'].ancho = 3;
 tipografia['g'].ancho = 4;
 tipografia['h'].ancho = 4;
 tipografia['i'].ancho = 2;
 tipografia['j'].ancho = 4;
 tipografia['k'].ancho = 4;
 tipografia['l'].ancho = 4;
 tipografia['m'].ancho = 6;
 tipografia['n'].ancho = 4;
 tipografia['o'].ancho = 4;
 tipografia['p'].ancho = 4;
 tipografia['q'].ancho = 5;
 for(i='r'; i<='v'; i++) tipografia[i].ancho = 4;
 tipografia['w'].ancho = 6;
 tipografia['x'].ancho = 4;
 tipografia['y'].ancho = 4;
 tipografia['z'].ancho = 4;

 for(i=0; i<256; i++) tipografia[i].bitmap_char = new char [tipografia[i].ancho*tipografia[i].alto];
 for(i=0;i<16;i++) tipografia[32].bitmap_char[i] = 0 ;

 int ind_pcx = 0;
 char ind_tipo = '0';
 char *buf;

for(ind_tipo='0';ind_tipo<='9';ind_tipo++)
{
 buf = &tipografia[ind_tipo].bitmap_char[0];
 for(j = 0; j<tipografia[ind_tipo].alto; j++)
 for(i = ind_pcx ; i < ind_pcx + tipografia[ind_tipo].ancho; i++)
 *buf++ = image[4+i+j*pcxGlobal.width];

 ind_pcx+=tipografia[ind_tipo].ancho;
}


ind_pcx = 39;
for(ind_tipo='a';ind_tipo<='z';ind_tipo++)
{
 buf = tipografia[ind_tipo].bitmap_char;
 for(j = 0; j<tipografia[ind_tipo].alto; j++)
 for(i = ind_pcx ; i < ind_pcx + tipografia[ind_tipo].ancho; i++)
 *buf++ = image[4+i+j*pcxGlobal.width];

 ind_pcx+=tipografia[ind_tipo].ancho;
}

}

void Text(unsigned short X,unsigned short Y,char *texto, char Color ,font tipografia[256],char *buffer)
{
 char *txt;
 unsigned short i,j;
 char ind;
 txt = &texto[0];
 i=X;
 j=Y;
 while( *txt != '\0')
 {
  ind =(char) *txt;
  PutImageColor(i,j,tipografia[ind].ancho,tipografia[ind].alto,&tipografia[ind].bitmap_char[0],Color,buffer);
  i+= tipografia[ind].ancho;
  if( i >(319-tipografia[ind].ancho) )
  {
    i = X;
    j+= tipografia[ind].alto + 3;
  }
  txt++;
 }
}

*/
