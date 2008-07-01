#include"supereg/tgrid.h"

void 
TGrid::ImprimirTodaLaInfo(void)
{
//nada
}


//===========
//  x 0 0
//  0 0 0
//  0 0 0 
//===========
#define ACTUALIZACION_1 {adr=indix+(i/2)+(indiy+(j/2))*(Padre->dvs+1);G[j][i]= Padre->G[0][adr];G[j][i].anim=0;\
                         GP[i+j*dvspp].cuarto = 1;\
						 GP[i+j*dvspp].txa = Padre->GP[adr].txa;\
						 GP[i+j*dvspp].tya = Padre->GP[adr].tya;\
						 GP[i+j*dvspp].txb = GP[i+j*dvspp].txa +(Padre->GP[adr].txb - Padre->GP[adr].txa) / 2;\
						 GP[i+j*dvspp].tyb = GP[i+j*dvspp].tya +(Padre->GP[adr].tyb - Padre->GP[adr].tya) / 2;\
                         }
//===========
//  P 0 0
//  x 0 0
//  P 0 0
//===========
#define ACTUALIZACION_2 {adr=indix+(i/2)+(indiy+(j/2))*(Padre->dvs+1); G[j][i]=Promediar(Padre->G[0][adr],Padre->G[0][adr+(Padre->dvs+1)]);\
                         GP[i+j*dvspp].cuarto = 3;\
						 GP[i+j*dvspp].txa = Padre->GP[adr].txa;\
						 GP[i+j*dvspp].tya = Padre->GP[adr].tya +(Padre->GP[adr].tyb - Padre->GP[adr].tya) / 2.0;\
						 GP[i+j*dvspp].txb = GP[i+j*dvspp].txa +(Padre->GP[adr].txb - Padre->GP[adr].txa) / 2;\
						 GP[i+j*dvspp].tyb = GP[i+j*dvspp].tya +(Padre->GP[adr].tyb - Padre->GP[adr].tya) / 2;\
                         }
//===========
//  P x P
//  0 0 0
//  0 0 0
//===========
#define ACTUALIZACION_3 {adr=indix+(i/2)+(indiy+(j/2))*(Padre->dvs+1); G[j][i]=Promediar(Padre->G[0][adr],Padre->G[0][adr+1]);\
                         GP[i+j*dvspp].cuarto = 2;\
						 GP[i+j*dvspp].txa = Padre->GP[adr].txa +(Padre->GP[adr].txb - Padre->GP[adr].txa) / 2;\
						 GP[i+j*dvspp].tya = Padre->GP[adr].tya;\
						 GP[i+j*dvspp].txb = GP[i+j*dvspp].txa +(Padre->GP[adr].txb - Padre->GP[adr].txa) / 2;\
						 GP[i+j*dvspp].tyb = GP[i+j*dvspp].tya +(Padre->GP[adr].tyb - Padre->GP[adr].tya) / 2;\
                         }

//===========
//  P 0 P
//  0 x 0
//  P 0 P
//===========
#define ACTUALIZACION_4 {adr=indix+(i/2)+(indiy+(j/2))*(Padre->dvs+1); G[j][i]=Promediar(Padre->G[0][adr],Padre->G[0][adr+1+(Padre->dvs+1)],Padre->G[0][adr+1],Padre->G[0][adr+(Padre->dvs+1)]);\
                         GP[i+j*dvspp].cuarto = 4;\
						 GP[i+j*dvspp].txa = Padre->GP[adr].txa +(Padre->GP[adr].txb - Padre->GP[adr].txa) / 2;\
						 GP[i+j*dvspp].tya = Padre->GP[adr].tya +(Padre->GP[adr].tyb - Padre->GP[adr].tya) / 2;\
						 GP[i+j*dvspp].txb = GP[i+j*dvspp].txa +(Padre->GP[adr].txb - Padre->GP[adr].txa) / 2;\
						 GP[i+j*dvspp].tyb = GP[i+j*dvspp].tya +(Padre->GP[adr].tyb - Padre->GP[adr].tya) / 2;\
                         }
/*
G : los datos de la grilla

GP : los datos de la grilla(P->PROXIMO) usado para animar ese vertice de la grilla

PROY : los puntos proyectados

Los tres tienen igual tama¤o y autosimilares

*/
TGrid::TGrid(int dim, int valor,char N,  int altitud)
{
   int j;

   //multiplicador de altura
   MULTALT = 1.0;

   Memoria_Utilizada = 0;
   Poligonos = 0;

   Luzx =(float)(8.0) / sqrt(129.0);
   Luzy =(float)(-8.0) / sqrt(129.0);
   Luzz =(float)(-1.0) / sqrt(129.0);

   Nesima = N;

   Tamanio = dim*dim;

   dvspp = dim;

   dvs = dvspp-1;

   //====================================
   // nodos dela grilla: datos basicos
   //====================================
   G = new TIPO * [dvspp];
   G[0] = new TIPO [Tamanio];
   for(j=1;j<dvspp;j++)  G[j] = &G[0][j*dvspp];
   Memoria_Utilizada+= Tamanio*sizeof(TIPO);
   Memoria_Utilizada+= dvspp*sizeof(TIPO*);

   //====================================
   // nodos dela grilla: datos basicos
   //====================================
   GP = new TIPO2 [Tamanio];

   L_GP = new TIPO2P [dvspp];

   for(j=0;j<dvspp; j++)  L_GP[j] = &GP[j*dvspp];
   Memoria_Utilizada+= Tamanio*sizeof(TIPO2);
   Memoria_Utilizada+= dvspp*sizeof(TIPO*);


   //====================================
   // nodos dela grilla: puntos 3d proyectados
   //====================================   
   PROY = new TProy [Tamanio];
   L_PROY = new TProy * [dvspp];
   for(j=0;j<dvspp; j++) L_PROY[j] = &PROY[j*dvspp];

   Memoria_Utilizada+= Tamanio*sizeof(TProy);
   Memoria_Utilizada+= dvspp*sizeof(TProy*);


   //si la grilla no es la grilla mas grande o sea la 0
   if(N!=0)
   {
   Normales = new TVector3df [dvs*dvs*2];
   Memoria_Utilizada+= dvs*dvs*2*sizeof(TVector3df);
   }

   x = 0;

   y = 0;

   z = altitud;

   difx = 0;

   dify = 0;

   bordea = dvs / 4; //esto es si todas las grillas tienen las mismas divisiones

   AsperezaDeEscala = 0.01;

   textura = NULL;

   contraste = 5.0;
	glActiveTextureARB = NULL;
	glMultiTexCoord2fARB = NULL;

	#ifdef _WIN32
	glActiveTextureARB		=(PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	=(PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
	#else
	glActiveTextureARB		=(PFNGLACTIVETEXTUREARBPROC) glutGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	=(PFNGLMULTITEXCOORD2FARBPROC) glutGetProcAddress("glMultiTexCoord2fARB");
	#endif

}


//-----------------------------------------------------------------------------------------

TGrid::~TGrid()
{
 delete[] PROY;
 delete[] L_PROY;
 delete[] G[0];
 delete[] G;
 delete[] L_GP;
 delete[] GP;
 DestruirPaleta(ALPAL);
}

//-----------------------------------------------------------------------------------------


TIPO
TGrid::Promediar(TIPO alt1,TIPO alt2,TIPO alt3,TIPO alt4)
{
       TIPO value;
       value.alt=(alt1.alt+alt2.alt+alt3.alt+alt4.alt)/4;
       value.aspereza =(alt1.aspereza+alt2.aspereza+alt3.aspereza+alt4.aspereza)/4;
       value.anim = 0;
       value.origen = Nesima;
return(value);
}


//-----------------------------------------------------------------------------------------


TIPO
TGrid::Promediar(TIPO alt1,TIPO alt2)
{
    TIPO value;
    value.alt =(alt1.alt+alt2.alt)/2;
    value.aspereza =(alt1.aspereza+alt2.aspereza)/2;
    value.anim = 0;
    value.origen = Nesima;
return(value);}


//-----------------------------------------------------------------------------------------


void
TGrid::Ruido(TIPO *altura_semilla)
{
int r ;
float as,t;

t =(float) altura_semilla->aspereza;
//as =(float)((float) altura_semilla->aspereza * altura_semilla->aspereza * altura_semilla->aspereza)/(255.0*255.0*255.0);//*altura_semilla.aspereza;
as =((sin(1.5*t)*sin(2.2*t)) * t * t) /( 255.0 * 255.0);
if(as>0.7) as = 0.7;
r =(int)(gauss(altura_semilla->alt)*radm2*AsperezaDeEscala*as);
if(altura_semilla->alt != 0) altura_semilla->alt = altura_semilla->alt + r;

if(altura_semilla->alt<MINALT) altura_semilla->alt = MINALT;
if(altura_semilla->alt>MAXALT) altura_semilla->alt = MAXALT;

altura_semilla->anim = 30;

}

//-----------------------------------------------------------------------------------------


void
TGrid::Actualizar(int SUMX, int SUMY)
{
 int i,j,istart,jstart;
// int adri,adrj;
 int adr;
 int act_ax,act_bx,act_ay,act_by;
 int rand_act_ax,rand_act_bx,rand_act_ay,rand_act_by;    
 int repx,repy;
 int indix,indiy,brda;

  Actualizado = 1;

  repx = SUMX /(rad * 2); repy = SUMY /(rad * 2);
  x = x -(repx *(rad * 2));  y = y -(repy *(rad * 2));

  if(x > rad) {x=x-(rad*2); repx++;}
  if(x < -rad) {x=x+(rad*2); repx--;}
  if(y > rad) {y=y-(rad*2); repy++;}
  if(y < -rad) {y=y+(rad*2); repy--;}

  dify =(int)(y - Padre->y)/(Padre->rad);
  difx =(int)(Padre->x - x)/(Padre->rad);

  if(repx == 0) {act_ax = 0; act_bx = 0;
                  rand_act_ax = 4; rand_act_bx = 4;}
  else      
  if(repx < 0) {act_ax = 0; act_bx = - repx - 1;
                 rand_act_ax = 4; rand_act_bx = - repx - 1 + 4;}
  else          {act_ax = dvs - repx + 1; act_bx = dvs;
                 rand_act_ax = dvs - 4 - repx + 1; rand_act_bx = dvs - 4;}

  if(repy == 0) {act_ay = 0; act_by = 0;
                  rand_act_ay = 4; rand_act_by = 4;}
  else      
  if(repy > 0) {act_ay = 0; act_by = repy - 1;
                 rand_act_ay = 4; rand_act_by = repy - 1 + 4;}
  else          {act_ay = dvs + repy + 1; act_by = dvs;
                 rand_act_ay = dvs - 4 + repy + 1; rand_act_by = dvs - 4;}

     
  if(repy< -(dvs+1) || repy >dvs+1) { act_ay = 0; act_by = dvs; repx = 0;}
  if(repx< -(dvs+1) || repx >dvs+1) { act_ay = 0; act_by = dvs; repx = 0; repy = dvs+1; }


//Ahora que fijamos los margenes del despalzamiento de la grilla
//corremos los datos de la grilla

  if(repx!=0 && repx<=dvs && repx>=-dvs)
        {if(repx<0) for(j=0; j<=dvs; j++)
                      for(i=dvs; i>=act_bx+1; i--)
                        {
                         G[j][i]=G[j][i+repx];
                         GP[i+j*dvspp]=GP[i+repx+j*dvspp];
//                         if(Nesima==4) GT[i+j*dvspp] = GT[i+repx+j*dvspp]; else
/*texturas*/             //blit(GT[i+repx+j*dvspp],GT[i+j*dvspp],0,0,0,0,TEXTDEF,TEXTDEF);
                        }
           else for(j=0; j<=dvs; j++)
                  for(i=0; i<=act_ax-1; i++)
                    {
                     G[j][i]=G[j][i+repx];
                     GP[i+j*dvspp]=GP[i+repx+j*dvspp];
//                     if(Nesima==4) GT[i+j*dvspp] = GT[i+repx+j*dvspp]; else
/*texturas*/     //blit(GT[i+repx+j*dvspp],GT[i+j*dvspp],0,0,0,0,TEXTDEF,TEXTDEF);
                    }
        }

  if(repy!=0 && repy<=dvs && repy>=-dvs)
        {if(repy<0) for(i=0; i<=dvs; i++)
                for(j=0; j<=act_ay-1; j++)
                {
                 G[j][i]=G[j-repy][i];
                 GP[i+j*dvspp]=GP[i+(j-repy)*dvspp];
//                 if(Nesima==4) GT[i+j*dvspp] = GT[i+(j-repy)*dvspp]; else
/*texturas*/     //blit(GT[i+(j-repy)*dvspp],GT[i+j*dvspp],0,0,0,0,TEXTDEF,TEXTDEF);
                }
        else        for(i=0; i<=dvs; i++)
                for(j=dvs; j>=act_by+1; j--)
                {
                 G[j][i]=G[j-repy][i];
                 GP[i+j*dvspp]=GP[i+(j-repy)*dvspp];
//                 if(Nesima==4) GT[i+j*dvspp] = GT[i+(j-repy)*dvspp]; else
/*texturas*/     //blit(GT[i+(j-repy)*dvspp],GT[i+j*dvspp],0,0,0,0,TEXTDEF,TEXTDEF);
                }
        }

  brda =( Padre->dvs / 2 ) -( dvs / 4 );

//aqui se calculan los puntos medios de la grilla con respecto a la grilla padre

if(repy!=0)
{
  //indix es con respecto al padre a partir de donde se
  //tomaran los puntos del Padre que coinciden con el Hijo
  //depende de difx(posicion relativa, una de tres)
  //analogo indiy      
  indix=brda+((difx+1)/2); indiy=brda+((dify+1)/2);
  jstart=abs(dify); istart=abs(difx);
  if(repy<0) for(j=dvs-jstart; j>=act_ay; j-=2)
              for(i=istart; i<=dvs; i+=2) ACTUALIZACION_1
    else for(j=jstart; j<=act_by; j+=2)
          for(i=istart; i<=dvs; i+=2) ACTUALIZACION_1

  indix=brda+((difx+1)/2); indiy=brda-((1-dify)/2);
  if(abs(dify)==abs(difx) && difx!=0) {istart=1;jstart=0;}
  else {istart=abs(difx);jstart=1-abs(dify);}
  if(repy<0) {for(j=dvs-jstart; j>=act_ay; j-=2)
               for(i=istart; i<=dvs; i+=2) ACTUALIZACION_2
               for(j=3-(1-jstart); j>=(act_ay-(dvs-3)) && j>=0; j-=2) 
               for(i=istart; i<=dvs; i+=2) ACTUALIZACION_2
               }  
    else  {for(j=jstart; j<=act_by; j+=2)
           for(i=istart; i<=dvs; i+=2) ACTUALIZACION_2
           for(j=(1-jstart) + dvs - 3; j<=(act_by + dvs - 3)  && j<=dvs; j+=2) 
           for(i=istart; i<=dvs; i+=2) ACTUALIZACION_2     
          }  

  indix=brda-((1-difx)/2); indiy=brda+((dify+1)/2);
  if(abs(dify)==abs(difx) && difx!=0) {istart=0;jstart=1;}
  else {istart=1-abs(difx);jstart=abs(dify);}
  if(repy<0) {for(j=dvs-jstart; j>=act_ay; j-=2)
               for(i=istart; i<=dvs; i+=2) ACTUALIZACION_3
               for(j=3-(1-jstart); j>=(act_ay-(dvs-3)) && j>=0; j-=2) 
               for(i=istart; i<=dvs; i+=2) ACTUALIZACION_3
               }  
    else  {for(j=jstart; j<=act_by; j+=2)
           for(i=istart; i<=dvs; i+=2) ACTUALIZACION_3
           for(j=(1-jstart) + dvs - 3; j<=(act_by + dvs - 3)  && j<=dvs; j+=2) 
           for(i=istart; i<=dvs; i+=2) ACTUALIZACION_3     
          }  

  indix=brda-((1-difx)/2); indiy=brda-((1-dify)/2);
  if(abs(dify)==abs(difx) && difx!=0) {istart=0;jstart=0;}
  else {istart=1-abs(difx);jstart=1-abs(dify);}
  if(repy<0) {for(j=dvs-jstart; j>=act_ay; j-=2)
               for(i=istart; i<=dvs; i+=2) ACTUALIZACION_4
               for(j=3-(1-jstart); j>=(act_ay-(dvs-3)) && j>=0; j-=2) 
               for(i=istart; i<=dvs; i+=2) ACTUALIZACION_4
               }  
    else  {for(j=jstart; j<=act_by; j+=2)
           for(i=istart; i<=dvs; i+=2) ACTUALIZACION_4
           for(j=(1-jstart) + dvs - 3; j<=(act_by + dvs - 3)  && j<=dvs; j+=2) 
           for(i=istart; i<=dvs; i+=2) ACTUALIZACION_4      
          }  

}


if(repx!=0)
{
  int a_Y,b_Y;

  indix = brda+((difx+1)/2);indiy=brda+((dify+1)/2);
  jstart = abs(dify);istart = abs(difx);
  if(act_ay != 0) {a_Y = jstart;b_Y = act_ay;}
  else {a_Y =jstart+((act_by-jstart)/2)*2;b_Y = dvs;}
  if(repx<0) for(i = istart ;  i <= act_bx ; i += 2)
              for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_1
         else for(i=dvs-istart; i>=act_ax; i-=2)
              for(j = a_Y ; j <= b_Y ; j+=2 ) ACTUALIZACION_1
          

          indix=brda+((difx+1)/2); indiy=brda-((1-dify)/2);
  if(abs(dify)==abs(difx) && difx!=0) {istart=1;jstart=0;}
  else {istart=abs(difx); jstart=1-abs(dify);}
  if(act_ay != 0) { a_Y = jstart; b_Y = act_ay;}
  else { a_Y = jstart +((act_by-jstart)/2)*2; b_Y = dvs;}
  if(repx<0) {for(i = istart ;  i <= act_bx ; i += 2)
               for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_2
               for(i =(1-istart) + dvs - 3 ; i <=(act_bx + dvs - 3) && i<=dvs ; i+= 2)
               for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_2
                }
     else {for(i=dvs-istart; i>=act_ax; i-=2)
            for(j = a_Y ; j <= b_Y ; j+=2 ) ACTUALIZACION_2
            for(i = 3 -(1-istart) ; i >=( act_ax -(dvs-3) ) && i>=0; i-=2)
            for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_2
           }

  indix=brda-((1-difx)/2); indiy=brda+((dify+1)/2);
  if(abs(dify)==abs(difx) && difx!=0) {istart=0;jstart=1;}
  else {istart=1-abs(difx);jstart=abs(dify);}
  if(act_ay != 0) { a_Y = jstart; b_Y = act_ay;}
  else { a_Y = jstart +((act_by-jstart)/2)*2; b_Y = dvs;}     
  if(repx<0) {for(i = istart ;  i <= act_bx ; i += 2)
               for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_3
               for(i =(1-istart) + dvs - 3 ; i <=(act_bx + dvs - 3) && i<=dvs ; i+= 2)
               for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_3
                }
     else {for(i=dvs-istart; i>=act_ax; i-=2)
            for(j = a_Y ; j <= b_Y ; j+=2 ) ACTUALIZACION_3
            for(i = 3 -(1-istart) ; i >=( act_ax -(dvs-3) ) && i>=0; i-=2)
            for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_3
           }

  indix=brda-((1-difx)/2); indiy=brda-((1-dify)/2);
  if(abs(dify)==abs(difx) && difx!=0) {istart=0;jstart=0;}
  else {istart=1-abs(difx);jstart=1-abs(dify);}
  if(act_ay != 0) { a_Y = jstart; b_Y = act_ay;}
  else { a_Y = jstart +((act_by-jstart)/2)*2; b_Y = dvs;}
  if(repx<0) {for(i = istart ;  i <= act_bx ; i += 2)
               for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_4
               for(i =(1-istart) + dvs - 3 ; i <=(act_bx + dvs - 3) && i<=dvs ; i+= 2)
               for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_4
                }
     else {for(i=dvs-istart; i>=act_ax; i-=2)
            for(j = a_Y ; j <= b_Y ; j+=2 ) ACTUALIZACION_4
            for(i = 3 -(1-istart) ; i >=( act_ax -(dvs-3) ) && i>=0; i-=2)
            for(j = a_Y ; j <=  b_Y ; j+=2 ) ACTUALIZACION_4
           }

}


//////aca se suma a las alturas medias calculadas anteriormente
/////una variable aleatoria cuya semilla sera la altura misma que modificara

if(repy< -(dvs+1-8) || repy >dvs+1-8) { rand_act_ay = 4; rand_act_by = dvs - 4; repx = 0;}
if(repx< -(dvs+1-8) || repx >dvs+1-8) { rand_act_ay = 4; rand_act_by = dvs - 4; repx = 0; repy = 1; }

if(repy!=0)
{

  if(abs(dify)==abs(difx) && difx!=0) {istart=1;jstart=0;}
  else {istart=abs(difx);jstart=1-abs(dify);}
  if(repy<0) for(j=dvs-(jstart+4); j>=rand_act_ay; j-=2)
          for(i=istart+4; i<=dvs-4; i+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}
    else  for(j=jstart+4; j<=rand_act_by; j+=2)
          for(i=istart+4; i<=dvs-4; i+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}

  if(abs(dify)==abs(difx) && difx!=0) {istart=0;jstart=1;}
  else {istart=1-abs(difx);jstart=abs(dify);}
  if(repy<0) for(j=dvs-(jstart+4); j>=rand_act_ay; j-=2)
          for(i=istart+4; i<=dvs-4; i+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}
    else  for(j=jstart+4; j<=rand_act_by; j+=2)
          for(i=istart+4; i<=dvs-4; i+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}

  if(abs(dify)==abs(difx) && difx!=0) {istart=0;jstart=0;}
  else {istart=1-abs(difx);jstart=1-abs(dify);}
  if(repy<0) for(j=dvs-(jstart+4); j>=rand_act_ay; j-=2)
          for(i=istart+4; i<=dvs-4; i+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}
    else  for(j=jstart+4; j<=rand_act_by; j+=2)
          for(i=istart+4; i<=dvs-4; i+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}
  
}


if(repx!=0)
{
  int a_Y,b_Y;

  if(abs(dify)==abs(difx) && difx!=0) {istart=1;jstart=0;}
  else {istart=abs(difx); jstart=1-abs(dify);}
  if(rand_act_ay != 4) { a_Y = jstart+4; b_Y = rand_act_ay;}
  else { a_Y = jstart + 4 +((act_by-jstart)/2)*2; b_Y = dvs - 4;}
  if(repx<0) for(i=istart+4; i<=rand_act_bx; i+=2)
          for( j = a_Y ; j <= b_Y ; j+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}
     else for(i=dvs-(istart+4); i>=rand_act_ax; i-=2)
          for( j = a_Y ; j <= b_Y ; j+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}

  if(abs(dify)==abs(difx) && difx!=0) {istart=0;jstart=1;}
  else {istart=1-abs(difx);jstart=abs(dify);}
    if(rand_act_ay != 4) { a_Y = jstart+4; b_Y = rand_act_ay;}
  else { a_Y = jstart + 4 +((act_by-jstart)/2)*2; b_Y = dvs - 4;}
  if(repx<0) for(i=istart+4; i<=rand_act_bx; i+=2)
          for( j = a_Y ; j <= b_Y ; j+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}
     else for(i=dvs-(istart+4); i>=rand_act_ax; i-=2)
          for( j = a_Y ; j <= b_Y ; j+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}

  if(abs(dify)==abs(difx) && difx!=0) {istart=0;jstart=0;}
  else {istart=1-abs(difx);jstart=1-abs(dify);}
  if(rand_act_ay != 4) { a_Y = jstart+4; b_Y = rand_act_ay;}
  else { a_Y = jstart + 4 +((act_by-jstart)/2)*2; b_Y = dvs - 4;}
  if(repx<0) for(i=istart+4; i<=rand_act_bx; i+=2)
          for( j = a_Y ; j <= b_Y ; j+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}
     else for(i=dvs-(istart+4); i>=rand_act_ax; i-=2)
          for( j = a_Y ; j <= b_Y ; j+=2) {GP[i+j*dvspp].altaux=G[j][i].alt; Ruido(&G[j][i]);}

}


return;
}

//-----------------------------------------------------------------------------------------
///******///******///******///******///******///******///******///******///******
/*
    RENDER ESFERICO

*/
///******///******///******///******///******///******///******///******///******

void
TGrid::Precalculus(TGrid *Inicial,TMapa *Mapa,TCAMARA3D *CAM)
{
 TGrid * Actual;
// int contador=0;

 Actual = Inicial;

 cpx = 0;
 cpy = 0;

 while( Actual != Self )
 {
  Actual = Actual->Hijo;
  cpx+=(Actual->difx) *(Actual->radm2);
  cpy-=(Actual->dify) *(Actual->radm2);
 }

 //CAM->Oz = zz;

 cpx_in_x = cpx - Inicial->x;

 cpy_in_y = cpy - Inicial->y;

 CAM->constante = - CAM->u.z * CAM->Oz;

 MRSQ = Mapa->radioc * Mapa->radioc;

 alfaang = acos( Mapa->radioc /(Mapa->radioc + CAM->Oz) );

 distmaxima = sqrt((CAM->Oz+10000000+Mapa->radioc)*(CAM->Oz+10000000+Mapa->radioc) - MRSQ);

 dd =(Mapa->radioc/(Mapa->radioc+CAM->Oz))*distmaxima;

 d1 = sqrt(float(CAM->foco*CAM->foco+320*320));

 HMmax = rad*2.5;

 COSANGMAXIMODEF = 0.9;

 COSANGMAXIMO = CAM->foco / d1;

 if(d1==0) cosfoco = COSANGMAXIMODEF;
        else cosfoco = COSANGMAXIMO;

}


void
TGrid::Precalculus(TGrid *Inicial,TMapa *Mapa,float zz, TCAMARA3D *CAM)
{
 TGrid * Actual;
// int contador=0;

 Actual = Inicial;

 cpx = 0;
 cpy = 0;

 while( Actual != Self )
 {
  Actual = Actual->Hijo;
  cpx+=(Actual->difx) *(Actual->radm2);
  cpy-=(Actual->dify) *(Actual->radm2);
 }

 CAM->Oz = zz;

 cpx_in_x = cpx - Inicial->x;

 cpy_in_y = cpy - Inicial->y;

 CAM->constante = - CAM->u.z * CAM->Oz;

 MRSQ = Mapa->radioc * Mapa->radioc;

 alfaang = acos( Mapa->radioc /(Mapa->radioc + CAM->Oz) );

 distmaxima = sqrt((CAM->Oz+10000000+Mapa->radioc)*(CAM->Oz+10000000+Mapa->radioc) - MRSQ);

 dd =(Mapa->radioc/(Mapa->radioc+CAM->Oz))*distmaxima;

 d1 = sqrt(float(CAM->foco*CAM->foco+320*320));

 HMmax = rad*2.5;

 COSANGMAXIMODEF = 0.9;

 COSANGMAXIMO = CAM->foco / d1;

 if(d1==0) cosfoco = COSANGMAXIMODEF;
        else cosfoco = COSANGMAXIMO;

}



void
TGrid::Animacion(TGrid *Inicial,TMapa *Mapa)
{
 int px_i,py_i;
 float px,py,pr,altitud;
 int i,j;
 int altsum=0;

 for(i=0;i<dvspp;i++)
 for(j=0;j<dvspp;j++)
 {

  if(G[j][i].anim!=0 && G[j][i].origen == Nesima)
  {
   altsum =(G[j][i].alt-L_GP[j][i].altaux)*(G[j][i].anim)/50;
   G[j][i].anim--;
  }
  else altsum=0;

  altitud =(G[j][i].alt-altsum) * MULTALT;

  if(altitud<0.0) altitud = 0.0;

  px_i =(rad*(2*i-dvs)) + cpx_in_x;
  py_i =(-rad*(2*j-dvs)) + cpy_in_y;
  px =(float) px_i;
  py =(float) py_i;

  pr = px*px+py*py;

  //la ola

  L_GP[j][i].alt =(int) altitud;

 }

}


void
TGrid::ProyectarTodo(TGrid *Inicial,TMapa *Mapa,float zz,TCAMARA3D *CAM)
{
 int i,j;
 int istart,jstart,iend,jend;
 float dist,cosOMu;
 char sedibuja;
 float HM,DIVI,Hx,Hy,Hz;
 float altitud;

 int px_i,py_i;
 float px,py,pz,pr;

 for(i=0;i<dvspp*dvspp;i++)
 {PROY[i].SeDibuja = 0;}

 Precalculus(Inicial,Mapa,CAM);

 if(Padre==NULL)
   {
   istart = dvs / 2 -(int)(dd /(pow(2.0,17)*METRO));
   jstart = istart;
   iend = dvspp-istart;
   jend = iend;
   }
   else
 {
   istart = 0;
   jstart = 0;
   iend = dvspp;
   jend = dvspp;
 }

 for(i=istart;i<iend;i++)
 for(j=jstart;j<jend;j++)
 {
  altitud =  L_GP[j][i].alt;

  px_i =(rad*(2*i-dvs)) + cpx_in_x;
  py_i =(-rad*(2*j-dvs)) + cpy_in_y;
  px =(float) px_i;
  py =(float) py_i;

  pr = px*px+py*py;

  if(pr<MRSQ)
  {
     pz = sqrt(MRSQ - pr);

     px = px*(Mapa->factorradiopradioc);

     py = py*(Mapa->factorradiopradioc);

     pz = pz*(Mapa->factorradiopradioc) - Mapa->radioc;

     pz = pz + altitud;

     L_PROY[j][i].P3d.x = px;

     L_PROY[j][i].P3d.y = py;

     L_PROY[j][i].P3d.z = pz;

     HM = CAM->u.x*px+CAM->u.y*py+CAM->u.z*pz+CAM->constante;

     dist = sqrt(px*px+py*py+(pz-CAM->Oz)*(pz-CAM->Oz));

     if(HM>HMmax) cosOMu=(CAM->u.x*px+CAM->u.y*py+CAM->u.z*(pz-CAM->Oz))/dist;
     else cosOMu = 0.0;

     if(cosOMu>cosfoco/1.3 && HM>HMmax && dist<distmaxima) sedibuja = 1;
     else sedibuja = 0;

     if(sedibuja==1) {
                    Hx = px - HM * CAM->u.x;
                    Hy = py - HM * CAM->u.y;
                    Hz =(pz - CAM->Oz) - HM * CAM->u.z;
                    DIVI= CAM->foco/HM;
                    L_PROY[j][i].P.x =(float)((CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
                    L_PROY[j][i].P.y =(float)((CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
 //                 if(L_PROY[j][i].P.x<0.0 || L_PROY[j][i].P.x>=640.0 || L_PROY[j][i].P.y<0.0 || L_PROY[j][i].P.y>=480.0) L_PROY[j][i].SeDibuja = 0;
                    //else
                    L_PROY[j][i].SeDibuja = 1;
                    L_PROY[j][i].Distancia = dist;

                   }
                   else
                   {
                    L_PROY[j][i].P.x = -2.0;
                    L_PROY[j][i].P.y = -2.0;
                    L_PROY[j][i].SeDibuja = 0;
                    L_PROY[j][i].Distancia = dist;
                   }
  }
  else L_PROY[j][i].SeDibuja = 0;

 }

}

///************************************************************************
void
TGrid::glProyectarTodo(TGrid *Inicial,TMapa *Mapa,float zz,TCAMARA3D *CAM)
{
 int i,j;
 int istart,jstart,iend,jend;
 float dist,cosOMu;
 char sedibuja;
 float HM;
 float altitud;

 int px_i,py_i;
 float px,py,pz,pr;

 for(i=0;i<dvspp*dvspp;i++)
 {PROY[i].SeDibuja = 0;}

 Precalculus(Inicial,Mapa,CAM);

 if(Padre==NULL)
   {
   istart = dvs / 2 -(int)(dd /(pow(2.0,17)*METRO));
   jstart = istart;
   iend = dvspp-istart;
   jend = iend;
   }
   else
 {
   istart = 0;
   jstart = 0;
   iend = dvspp;
   jend = dvspp;
 }

 for(i=istart;i<iend;i++)
 for(j=jstart;j<jend;j++)
 {
  altitud =  L_GP[j][i].alt;

  px_i =(rad*(2*i-dvs)) + cpx_in_x;
  py_i =(-rad*(2*j-dvs)) + cpy_in_y;
  px =(float) px_i;
  py =(float) py_i;

  pr = px*px+py*py;

  if(pr<MRSQ)
  {
     pz = sqrt(MRSQ - pr);

     px = px*(Mapa->factorradiopradioc);

     py = py*(Mapa->factorradiopradioc);

     pz = pz*(Mapa->factorradiopradioc) - Mapa->radioc;

     pz = pz + altitud;

     L_PROY[j][i].P3d.x = px;

     L_PROY[j][i].P3d.y = py;

     L_PROY[j][i].P3d.z = pz;

     HM = CAM->u.x*px+CAM->u.y*py+CAM->u.z*pz+CAM->constante;

     dist = sqrt(px*px+py*py+(pz-CAM->Oz)*(pz-CAM->Oz));

     if(HM>HMmax) cosOMu=(CAM->u.x*px+CAM->u.y*py+CAM->u.z*(pz-CAM->Oz))/dist;
     else cosOMu = 0.0;

     if(cosOMu>cosfoco/1.3 && HM>HMmax && dist<distmaxima) sedibuja = 1;
     else sedibuja = 0;

     if(sedibuja==1) {
                    //Hx = px - HM * CAM->u.x;
                    //Hy = py - HM * CAM->u.y;
                    //Hz =(pz - CAM->Oz) - HM * CAM->u.z;
                    //DIVI= CAM->foco/HM;
                    //L_PROY[j][i].P.x =(float)((CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
                    //L_PROY[j][i].P.y =(float)((CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
 //                 if(L_PROY[j][i].P.x<0.0 || L_PROY[j][i].P.x>=640.0 || L_PROY[j][i].P.y<0.0 || L_PROY[j][i].P.y>=480.0) L_PROY[j][i].SeDibuja = 0;
                    //else
                    L_PROY[j][i].SeDibuja = 1;
                    L_PROY[j][i].Distancia = dist;

                   }
                   else
                   {
                    //L_PROY[j][i].P.x = -2.0;
                    //L_PROY[j][i].P.y = -2.0;
                    L_PROY[j][i].SeDibuja = 0;
                    L_PROY[j][i].Distancia = dist;
                   }
  }
  else L_PROY[j][i].SeDibuja = 0;

 }

}


///************************************************************************

///************************************************************************
void
TGrid::ProyectarTodo_SafeMode(TGrid *Inicial,TMapa *Mapa,float zz,TCAMARA3D *CAM)
{
 int i,j;
 float dist,cosOMu;
 char sedibuja;
 float HM,DIVI,Hx,Hy,Hz;
 float altitud;

 int px_i,py_i;
 float px,py,pz;


 for( i = 0 ; i < Tamanio; i++) PROY[i].SeDibuja = 0;

 Precalculus(Inicial,Mapa,zz,CAM);

 for(i=0;i<dvspp;i++)
 for(j=0;j<dvspp;j++)
 {
  altitud =  L_GP[j][i].alt;

  if(altitud<0.0) altitud = 0.0;
  //posicion del punto relativa al mapa
  //y a la minima division de la minima grilla
  px_i = rad *(2 * i - dvs) - x;
  py_i = - rad *(2 * j - dvs) - y;
  px =(float) px_i;
  py =(float) py_i;

//  pr = px * px + py * py;

//  if(pr<radioc*radioc)
//  {
     pz = altitud;

     L_PROY[j][i].P3d.x = px;
     L_PROY[j][i].P3d.y = py;
     L_PROY[j][i].P3d.z = pz;

     HM = CAM->u.x * px + CAM->u.y * py + CAM->u.z * pz + CAM->constante;

     dist = sqrt(px*px+py*py+(pz-CAM->Oz)*(pz-CAM->Oz));

     if(HM>HMmax && dist!=0.0) cosOMu=(CAM->u.x*px+CAM->u.y*py+CAM->u.z*(pz-CAM->Oz))/dist;
     else cosOMu = 0.0;

     if(cosOMu>cosfoco/1.5 && HM>HMmax && dist<distmaxima) sedibuja = 1;
        else sedibuja = 0;

     if(sedibuja==1) {
                    Hx = px - HM * CAM->u.x;
                    Hy = py - HM * CAM->u.y;
                    Hz =(pz - CAM->Oz) - HM * CAM->u.z;
                    DIVI = CAM->foco/HM;
                    L_PROY[j][i].P.x =(float)(RESXD2-(CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
                    L_PROY[j][i].P.y =(float)(RESYD2-(CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
                    //if(L_PROY[j][i].P.x<0.0 || L_PROY[j][i].P.x>=640.0 || L_PROY[j][i].P.y<0.0 || L_PROY[j][i].P.y>=480.0) L_PROY[j][i].SeDibuja = 0;
                    //else
                    L_PROY[j][i].SeDibuja = 1;
                   }
                   else
                   {
                    L_PROY[j][i].P.x = -1.0;
                    L_PROY[j][i].P.y = -1.0;
                    L_PROY[j][i].SeDibuja = 0;
                   }
//  }
//  else L_PROY[j][i].SeDibuja = 0;

 }

}


///************************************************************************
/*
MEZCLA DE CULLING FACES + FRUSTRUM
Para los que no se deben dibujar, fija el valor de SeDibuja en 0.

PROYECCION
Esta funcion hace la proyeccion de los puntos 3d a 2d.



*/
///***************************

void
TGrid::ProyectarBorde(TGrid *Inicial,TMapa *Mapa,float zz,TCAMARA3D *CAM)
{
 int i,j;
 int istart,jstart,iend,jend;
 float dist,cosOMu;
 char sedibuja;
 float HM,DIVI,Hx,Hy,Hz;
 float altitud;

 int px_i,py_i;
 float px,py,pz,pr;
 int brd1,brd2;
 int brdax,brdbx,brday,brdby;
 int indax,indbx,inday,indby;


 for(i=0;i<dvspp*dvspp;i++) {PROY[i].SeDibuja = 0;}

 Precalculus(Inicial,Mapa,zz,CAM);

 brd1 =(dvs/2) -((Hijo->dvs)/4) + 1;
 brd2 = dvs - brd1 ;
 brdax = brd1 +((Hijo->difx+1)/2); brdbx=brd2-((1-Hijo->difx)/2);
 brday = brd1 +((Hijo->dify+1)/2); brdby=brd2-((1-Hijo->dify)/2);

 indax =(abs(difx) + 1) / 2 + 2 ;
 indbx = dvs - indax ;
 inday =(abs(dify) + 1) / 2 + 2;
 indby = dvs - inday ;

 if(Padre==NULL)
   {
   istart = dvs / 2 -(int)(dd /(pow(2.0,17)*METRO));
   jstart = istart;
   iend = dvspp-istart;
   jend = iend;
   }
   else
 {
   istart = indax;
   jstart = inday;
   iend = indbx+1;
   jend = indby+1;
 }

 for(i=istart;i<iend;i++)
 for(j=jstart;j<jend;j++)
 {

 if(i<=brdax+1 || i>=brdbx-1 || brdby-1<=j || j<=brday+1 || Padre==NULL)
 {
  altitud =  L_GP[j][i].alt;

  px_i =(rad*(2*i-dvs))+cpx_in_x;
  py_i =(-rad*(2*j-dvs))+cpy_in_y;
  px =(float) px_i;
  py =(float) py_i;

  pr = px*px+py*py;

  if(pr<MRSQ)
  {

     pz = sqrt(MRSQ-pr);
     px = px*(Mapa->factorradiopradioc);
     py = py*(Mapa->factorradiopradioc);
     pz = pz*(Mapa->factorradiopradioc) - Mapa->radioc;
     pz = pz + altitud;

     L_PROY[j][i].P3d.x = px;
     L_PROY[j][i].P3d.y = py;
     L_PROY[j][i].P3d.z = pz;

     HM = CAM->u.x*px+CAM->u.y*py+CAM->u.z*pz+CAM->constante;

     dist = sqrt(px*px+py*py+(pz-CAM->Oz)*(pz-CAM->Oz));

     if(HM>HMmax) cosOMu=(CAM->u.x*px+CAM->u.y*py+CAM->u.z*(pz-CAM->Oz))/dist;
     else cosOMu = 0.0;
     if(cosOMu>cosfoco/1.3 && HM>HMmax && dist<distmaxima) sedibuja = 1;
        else sedibuja = 0;

     if(sedibuja==1) {
                    Hx = px - HM * CAM->u.x;
                    Hy = py - HM * CAM->u.y;
                    Hz =(pz - CAM->Oz) - HM * CAM->u.z;
                    DIVI = CAM->foco/HM;
                    L_PROY[j][i].P.x =(float)((CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
                    L_PROY[j][i].P.y =(float)((CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
                    //if(L_PROY[j][i].P.x<0.0 || L_PROY[j][i].P.x>=640.0 || L_PROY[j][i].P.y<0.0 || L_PROY[j][i].P.y>=480.0) L_PROY[j][i].SeDibuja = 0;
                    //else
                    L_PROY[j][i].SeDibuja = 1;
                    L_PROY[j][i].Distancia = dist;
                   }
                   else
                   {
                    L_PROY[j][i].P.x = -2.0;
                    L_PROY[j][i].P.y = -2.0;
                    L_PROY[j][i].SeDibuja = 0;
                    L_PROY[j][i].Distancia = dist;
                   }
  }
  else L_PROY[j][i].SeDibuja = 0;

 }
 }

}

///************************************************************************
///***************************

void
TGrid::glProyectarBorde(TGrid *Inicial,TMapa *Mapa,float zz,TCAMARA3D *CAM)
{
 int i,j;
 int istart,jstart,iend,jend;
 float dist,cosOMu;
 char sedibuja;
 float HM;
 float altitud;

 int px_i,py_i;
 float px,py,pz,pr;
 int brd1,brd2;
 int brdax,brdbx,brday,brdby;
 int indax,indbx,inday,indby;


 for(i=0;i<dvspp*dvspp;i++) {PROY[i].SeDibuja = 0;}

 Precalculus(Inicial,Mapa,zz,CAM);

 brd1 =(dvs/2) -((Hijo->dvs)/4) + 1;
 brd2 = dvs - brd1 ;
 brdax = brd1 +((Hijo->difx+1)/2); brdbx=brd2-((1-Hijo->difx)/2);
 brday = brd1 +((Hijo->dify+1)/2); brdby=brd2-((1-Hijo->dify)/2);

 indax =(abs(difx) + 1) / 2 + 2 ;
 indbx = dvs - indax ;
 inday =(abs(dify) + 1) / 2 + 2;
 indby = dvs - inday ;

 if(Padre==NULL)
   {
   istart = dvs / 2 -(int)(dd /(pow(2.0,17)*METRO));
   jstart = istart;
   iend = dvspp-istart;
   jend = iend;
   }
   else
 {
   istart = indax;
   jstart = inday;
   iend = indbx+1;
   jend = indby+1;
 }

 for(i=istart;i<iend;i++)
 for(j=jstart;j<jend;j++)
 {

 if(i<=brdax+1 || i>=brdbx-1 || brdby-1<=j || j<=brday+1 || Padre==NULL)
 {
  altitud =  L_GP[j][i].alt;

  px_i =(rad*(2*i-dvs))+cpx_in_x;
  py_i =(-rad*(2*j-dvs))+cpy_in_y;
  px =(float) px_i;
  py =(float) py_i;

  pr = px*px+py*py;

  if(pr<MRSQ)
  {

     pz = sqrt(MRSQ-pr);
     px = px*(Mapa->factorradiopradioc);
     py = py*(Mapa->factorradiopradioc);
     pz = pz*(Mapa->factorradiopradioc) - Mapa->radioc;
     pz = pz + altitud;

     L_PROY[j][i].P3d.x = px;
     L_PROY[j][i].P3d.y = py;
     L_PROY[j][i].P3d.z = pz;

     HM = CAM->u.x*px+CAM->u.y*py+CAM->u.z*pz+CAM->constante;

     dist = sqrt(px*px+py*py+(pz-CAM->Oz)*(pz-CAM->Oz));

     if(HM>HMmax) cosOMu=(CAM->u.x*px+CAM->u.y*py+CAM->u.z*(pz-CAM->Oz))/dist;
     else cosOMu = 0.0;
     if(cosOMu>cosfoco/1.3 && HM>HMmax && dist<distmaxima) sedibuja = 1;
        else sedibuja = 0;

     if(sedibuja==1) {
                    //Hx = px - HM * CAM->u.x;
                    //Hy = py - HM * CAM->u.y;
                    //Hz =(pz - CAM->Oz) - HM * CAM->u.z;
                    //DIVI = CAM->foco/HM;
                    //L_PROY[j][i].P.x =(float)((CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
                    //L_PROY[j][i].P.y =(float)((CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
                    //if(L_PROY[j][i].P.x<0.0 || L_PROY[j][i].P.x>=640.0 || L_PROY[j][i].P.y<0.0 || L_PROY[j][i].P.y>=480.0) L_PROY[j][i].SeDibuja = 0;
                    //else
                    L_PROY[j][i].SeDibuja = 1;
                    L_PROY[j][i].Distancia = dist;
                   }
                   else
                   {
                    //L_PROY[j][i].P.x = -2.0;
                    //L_PROY[j][i].P.y = -2.0;
                    L_PROY[j][i].SeDibuja = 0;
                    L_PROY[j][i].Distancia = dist;
                   }
  }
  else L_PROY[j][i].SeDibuja = 0;

 }
 }

}
///************************************************************************

///************************************************************************

///************************************************************************

void
TGrid::ProyectarBorde_SafeMode(TGrid *Inicial,TMapa *Mapa, float zz,TCAMARA3D *CAM)
{
 int i,j;
 float dist,cosOMu;
 char sedibuja;
 float HM,DIVI,Hx,Hy,Hz;
 float altitud;

 int px_i,py_i;
 float px,py,pz;
 int brd1,brd2;
 int brdax,brdbx,brday,brdby;


 for( i = 0 ; i < Tamanio ; i++) PROY[i].SeDibuja = 0;

 Precalculus(Inicial,Mapa,zz,CAM);

 brd1 =(dvs/2) -((Hijo->dvs)/4) + 1;
 brd2 = dvs - brd1 ;
 brdax = brd1 +((Hijo->difx+1)/2); brdbx=brd2-((1-Hijo->difx)/2);
 brday = brd1 +((Hijo->dify+1)/2); brdby=brd2-((1-Hijo->dify)/2);

 for(i=0;i<dvspp;i++)
 for(j=0;j<dvspp;j++)
 {

 if(i<=brdax+1 || i>=brdbx-1 || brdby-1<=j || j<=brday+1 || Padre==NULL)
 {
  altitud =  L_GP[j][i].alt;

  if(altitud<0.0) altitud = 0.0;

  px_i = rad *(2 * i - dvs) - x;
  py_i = - rad *(2 * j - dvs) - y;
  px =(float) rad *(2 * i - dvs) - x;
  py =(float) - rad *(2 * j - dvs) - y;

//  pr = px * px + py * py;

//  if(pr<radioc*radioc)
//  {
     /*
     pz = sqrt(radioc*radioc-pr);
     px = px*factorradiopradioc;
     py = py*factorradiopradioc;
     pz = pz*factorradiopradioc - radioc;
     pz = pz + altitud;
     */
     pz = altitud;

     L_PROY[j][i].P3d.x = px;
     L_PROY[j][i].P3d.y = py;
     L_PROY[j][i].P3d.z = pz;

     HM = CAM->u.x*px+CAM->u.y*py+CAM->u.z*pz+constante;

     dist = sqrt(px*px+py*py+(pz-CAM->Oz)*(pz-CAM->Oz));

     if(HM>HMmax && dist!=0.0) cosOMu =(CAM->u.x*px+CAM->u.y*py+CAM->u.z*(pz-CAM->Oz))/dist;
     else cosOMu = 0.0;
     if(cosOMu>cosfoco/1.5 && HM>HMmax && dist<distmaxima) sedibuja = 1;
     else sedibuja = 0;

     if(sedibuja==1) {
                    Hx = px - HM * CAM->u.x;
                    Hy = py - HM * CAM->u.y;
                    Hz =(pz - CAM->Oz) - HM * CAM->u.z;
                    DIVI = CAM->foco/HM;
                    L_PROY[j][i].P.x =(float)((CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
                    L_PROY[j][i].P.y =(float)((CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
                    //if(L_PROY[j][i].P.x<0.0 || L_PROY[j][i].P.x>=640.0 || L_PROY[j][i].P.y<0.0 || L_PROY[j][i].P.y>=480.0) L_PROY[j][i].SeDibuja = 0;
                    //else
                    L_PROY[j][i].SeDibuja = 1;
                   }
                   else
                   {
                    L_PROY[j][i].P.x = -1000.0;
                    L_PROY[j][i].P.y = -1000.0;
                    L_PROY[j][i].SeDibuja = 0;
                   }
//  }
//  else L_PROY[j][i].SeDibuja = 0;

 }
 }

}


//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------

void
TGrid::CalcularNormalesBorde(void)
{
  int i,j;
  int brdax,brdbx,brday,brdby;
  int indax,indbx,inday,indby;
  int altp1,altp2,altp3,altp4;
  int brd1,brd2;
  double Nx,Ny,Nz,Nnorme,escal;
  int ax,bx,ay,by;

  if(Nesima==0) return;

  brd1 =(dvs/2) -(dvs/4) + 1;
  brd2 = dvs - brd1 ;
  brdax = brd1 +((Hijo->difx+1)/2); brdbx = brd2 -((1-Hijo->difx)/2);
  brday = brd1 +((Hijo->dify+1)/2); brdby = brd2 -((1-Hijo->dify)/2);

  indax =(abs(difx) + 1) / 2 + 2 ;
  indbx = dvs - indax ;
  inday =(abs(dify) + 1) / 2 + 2;
  indby = dvs - inday ;

// for( i = indax ; i < indbx-1 ; i++)
// for( j = inday ; j < indby-1 ; j++)
  for( i = 0 ; i <= dvs-1 ; i++)
  for( j = 0 ; j <= dvs-1 ; j++)
 {
  /*
  if(L_PROY[j][i].SeDibuja==1 &&
         L_PROY[j+1][i].SeDibuja==1 &&
         L_PROY[j][i+1].SeDibuja==1 &&
         L_PROY[j+1][i+1].SeDibuja==1)
         {
  */
  if(i<brdax+2 || i>=brdbx-2 || brdby-2<=j || j<brday+2)
    {

     /*
     altp1 = L_PROY[j][i].P3d.z;
     altp2 = L_PROY[j][i+1].P3d.z;
     altp3 = L_PROY[j+1][i].P3d.z;
     altp4 = L_PROY[j+1][i+1].P3d.z;
     */

     altp1 = L_GP[j][i].alt;
     altp2 = L_GP[j][i+1].alt;
     altp3 = L_GP[j+1][i].alt;
     altp4 = L_GP[j+1][i+1].alt;

     Nx =(double)(altp1 - altp2);
     Ny =(double)(altp3 - altp1);
     Nz =(double) rad * 2.0;

     Nnorme = sqrt( Nx * Nx + Ny * Ny + Nz * Nz);
     Normales[2*(i+j*dvs)+1].x = Nx = Nx / Nnorme;
     Normales[2*(i+j*dvs)+1].y = Ny = Ny / Nnorme;
     Normales[2*(i+j*dvs)+1].z = Nz = Nz / Nnorme;
     escal = Luzx * Nx + Luzy * Ny + Luzz * Nz;
     if(escal>=0.0) escal = 0.5;
     else escal =( - escal * 1.0 /(2.0*contraste) ) + 0.7;
     Normales[2*(i+j*dvs)+1].norma = escal;

     Nx =(double)(altp1 - altp2);
     Ny =(double)(altp4 - altp2);

     Nnorme = sqrt( Nx * Nx + Ny * Ny + Nz * Nz);
     Normales[2*(i+j*dvs)].x = Nx = Nx / Nnorme;
     Normales[2*(i+j*dvs)].y = Ny = Ny / Nnorme;
     Normales[2*(i+j*dvs)].z = Nz = Nz / Nnorme;
     escal = Luzx * Nx + Luzy * Ny + Luzz * Nz;

     if(escal>=0.0) escal = 0.5;
     else escal =( - escal * 1.0 /(2.0*contraste) ) + 0.7;
     Normales[2*(i+j*dvs)].norma = escal;

  }
}

    double I = 0;

    for(j=1; j<=dvs-1; j++)
    for(i=1; i<=dvs-1; i++)
    {    
     if(L_PROY[j][i].SeDibuja==1)
     {
       I = Normales[2*(i+j*dvs)].norma + Normales[2*(i+j*dvs)+1].norma +
       Normales[2*(i-1+(j-1)*dvs)].norma + Normales[2*(i-1+(j-1)*dvs)+1].norma +
       Normales[2*(i-1+j*dvs)+1].norma + Normales[2*(i+(j-1)*dvs)].norma;
       L_PROY[j][i].N.x =(Normales[2*(i+j*dvs)].x + Normales[2*(i+j*dvs)+1].x +
       Normales[2*(i-1+(j-1)*dvs)].x + Normales[2*(i-1+(j-1)*dvs)+1].x +
       Normales[2*(i-1+j*dvs)+1].x + Normales[2*(i+(j-1)*dvs)].x)/6.0f;
	   L_PROY[j][i].N.y =(Normales[2*(i+j*dvs)].y + Normales[2*(i+j*dvs)+1].y +
       Normales[2*(i-1+(j-1)*dvs)].y + Normales[2*(i-1+(j-1)*dvs)+1].y +
       Normales[2*(i-1+j*dvs)+1].y + Normales[2*(i+(j-1)*dvs)].y)/6.0f;
	   L_PROY[j][i].N.z =(Normales[2*(i+j*dvs)].z + Normales[2*(i+j*dvs)+1].z +
       Normales[2*(i-1+(j-1)*dvs)].z + Normales[2*(i-1+(j-1)*dvs)+1].z +
       Normales[2*(i-1+j*dvs)+1].z + Normales[2*(i+(j-1)*dvs)].z)/6.0f;

	   
	   I = I / 6.0;

     L_PROY[j][i].Intensidad =(unsigned char) I;
	 L_PROY[j][i].If =(float) I;
	 //L_PROY[j][i].If =(float) Normales[2*(i+j*dvs)].norma;
     }
    }

 ax = 2 - difx;
 bx = dvs - 2 - difx;
 ay = 2 + dify;
 by = dvs - 2 + dify;

 for(i = ax+1; i < bx ; i+=2)
 {
  PROY[i+ay*(dvs+1)].Intensidad =(PROY[i-1+ay*(dvs+1)].Intensidad+PROY[i+1+ay*(dvs+1)].Intensidad)/ 2;
  PROY[i+by*(dvs+1)].Intensidad =(PROY[i-1+by*(dvs+1)].Intensidad+PROY[i+1+by*(dvs+1)].Intensidad)/ 2;
  PROY[i+ay*(dvs+1)].If =(PROY[i-1+ay*(dvs+1)].If+PROY[i+1+ay*(dvs+1)].If)/ 2;
  PROY[i+by*(dvs+1)].If =(PROY[i-1+by*(dvs+1)].If+PROY[i+1+by*(dvs+1)].If)/ 2;
 }

 for(j = ay+1; j < by ; j+=2)
 {
  PROY[ax+j*(dvs+1)].Intensidad =(PROY[ax+(j-1)*(dvs+1)].Intensidad+PROY[ax+(j+1)*(dvs+1)].Intensidad)/ 2;
  PROY[bx+j*(dvs+1)].Intensidad =(PROY[bx+(j-1)*(dvs+1)].Intensidad+PROY[bx+(j+1)*(dvs+1)].Intensidad)/ 2;
  PROY[ax+j*(dvs+1)].If =(PROY[ax+(j-1)*(dvs+1)].If+PROY[ax+(j+1)*(dvs+1)].If)/ 2;
  PROY[bx+j*(dvs+1)].If =(PROY[bx+(j-1)*(dvs+1)].If+PROY[bx+(j+1)*(dvs+1)].If)/ 2;
 }


}




//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

void
TGrid::CalcularNormalesTodo(void)
{
  int i,j;
  int indax,indbx,inday,indby;
  int altp1,altp2,altp3,altp4;
  double Nx,Ny,Nz,Nnorme,escal;
  int ax,bx,ay,by;

  escal = 1.0;

  if(Nesima==0) return;

  indax =(abs(difx) + 1) / 2 + 2 ;
  indbx = dvs - indax ;
  inday =(abs(dify) + 1) / 2 + 2;
  indby = dvs - inday ;

// for( i = indax ; i < indbx-1 ; i++)
// for( j = inday ; j < indby-1 ; j++)
  for( i = 0 ; i <= dvs-1 ; i++)
  for( j = 0 ; j <= dvs-1 ; j++)
 {
  /*
  if(L_PROY[j][i].SeDibuja==1 &&
         L_PROY[j+1][i].SeDibuja==1 &&
         L_PROY[j][i+1].SeDibuja==1 &&
         L_PROY[j+1][i+1].SeDibuja==1)
         {
  */
//  if(i<brdax+2 || i>=brdbx-2 || brdby-2<=j || j<brday+2)
//    {

     /*
     altp1 = L_PROY[j][i].P3d.z;
     altp2 = L_PROY[j][i+1].P3d.z;
     altp3 = L_PROY[j+1][i].P3d.z;
     altp4 = L_PROY[j+1][i+1].P3d.z;
     */

     altp1 = L_GP[j][i].alt;
     altp2 = L_GP[j][i+1].alt;
     altp3 = L_GP[j+1][i].alt;
     altp4 = L_GP[j+1][i+1].alt;

     Nx =(double)(altp1 - altp2);
     Ny =(double)(altp3 - altp1);
     Nz =(double) rad * 2.0;

     Nnorme = sqrt( Nx * Nx + Ny * Ny + Nz * Nz);
     Normales[2*(i+j*dvs)+1].x = Nx = Nx / Nnorme;
     Normales[2*(i+j*dvs)+1].y = Ny = Ny / Nnorme;
     Normales[2*(i+j*dvs)+1].z = Nz = Nz / Nnorme;
     escal = Luzx * Nx + Luzy * Ny + Luzz * Nz;
     if(escal>=0.0) escal = 0.5;
     else escal =( - escal * 1.0 /(2.0*contraste) ) + 0.7;
     Normales[2*(i+j*dvs)+1].norma = escal;


     Nx =(double)(altp1 - altp2);
     Ny =(double)(altp4 - altp2);

     Nnorme = sqrt( Nx * Nx + Ny * Ny + Nz * Nz);
     Normales[2*(i+j*dvs)].x = Nx = Nx / Nnorme;
     Normales[2*(i+j*dvs)].y = Ny = Ny / Nnorme;
     Normales[2*(i+j*dvs)].z = Nz = Nz / Nnorme;
     escal = Luzx * Nx + Luzy * Ny + Luzz * Nz;
     if(escal>=0.0) escal = 0.5;
     else escal =( - escal * 1.0 /(2.0*contraste) ) + 0.7;
     Normales[2*(i+j*dvs)].norma = escal;
}

    double I = 0;

    for(j=1; j<=dvs-1; j++)
    for(i=1; i<=dvs-1; i++)
    {    
     if(L_PROY[j][i].SeDibuja==1)
     {
       I = Normales[2*(i+j*dvs)].norma + Normales[2*(i+j*dvs)+1].norma +
       Normales[2*(i-1+(j-1)*dvs)].norma + Normales[2*(i-1+(j-1)*dvs)+1].norma +
       Normales[2*(i-1+j*dvs)+1].norma + Normales[2*(i+(j-1)*dvs)].norma;
       L_PROY[j][i].N.x =(Normales[2*(i+j*dvs)].x + Normales[2*(i+j*dvs)+1].x +
       Normales[2*(i-1+(j-1)*dvs)].x + Normales[2*(i-1+(j-1)*dvs)+1].x +
       Normales[2*(i-1+j*dvs)+1].x + Normales[2*(i+(j-1)*dvs)].x)/6.0f;
	   L_PROY[j][i].N.y =(Normales[2*(i+j*dvs)].y + Normales[2*(i+j*dvs)+1].y +
       Normales[2*(i-1+(j-1)*dvs)].y + Normales[2*(i-1+(j-1)*dvs)+1].y +
       Normales[2*(i-1+j*dvs)+1].y + Normales[2*(i+(j-1)*dvs)].y)/6.0f;
	   L_PROY[j][i].N.z =(Normales[2*(i+j*dvs)].z + Normales[2*(i+j*dvs)+1].z +
       Normales[2*(i-1+(j-1)*dvs)].z + Normales[2*(i-1+(j-1)*dvs)+1].z +
       Normales[2*(i-1+j*dvs)+1].z + Normales[2*(i+(j-1)*dvs)].z)/6.0f;

	   I = I / 6.0;

     L_PROY[j][i].Intensidad =(unsigned char) I;
	 L_PROY[j][i].If =(float) I;
	 //L_PROY[j][i].If =(float) Normales[2*(i+j*dvs)].norma;
     }
    }

 ax = 2 - difx;
 bx = dvs - 2 - difx;
 ay = 2 + dify;
 by = dvs - 2 + dify;

 for(i = ax+1; i < bx ; i+=2)
 {
  PROY[i+ay*(dvs+1)].Intensidad =(PROY[i-1+ay*(dvs+1)].Intensidad+PROY[i+1+ay*(dvs+1)].Intensidad)/ 2;
  PROY[i+by*(dvs+1)].Intensidad =(PROY[i-1+by*(dvs+1)].Intensidad+PROY[i+1+by*(dvs+1)].Intensidad)/ 2;
  PROY[i+ay*(dvs+1)].If =(PROY[i-1+ay*(dvs+1)].If+PROY[i+1+ay*(dvs+1)].If)/ 2;
  PROY[i+by*(dvs+1)].If =(PROY[i-1+by*(dvs+1)].If+PROY[i+1+by*(dvs+1)].If)/ 2;
 }

 for(j = ay+1; j < by ; j+=2)
 {
  PROY[ax+j*(dvs+1)].Intensidad =(PROY[ax+(j-1)*(dvs+1)].Intensidad+PROY[ax+(j+1)*(dvs+1)].Intensidad)/ 2;
  PROY[bx+j*(dvs+1)].Intensidad =(PROY[bx+(j-1)*(dvs+1)].Intensidad+PROY[bx+(j+1)*(dvs+1)].Intensidad)/ 2;
  PROY[ax+j*(dvs+1)].If =(PROY[ax+(j-1)*(dvs+1)].If+PROY[ax+(j+1)*(dvs+1)].If)/ 2;
  PROY[bx+j*(dvs+1)].If =(PROY[bx+(j-1)*(dvs+1)].If+PROY[bx+(j+1)*(dvs+1)].If)/ 2;
 }


}



///************************************************************************

void
TGrid::DibujarAlturaBorde(int area,TBITMAP *pagina)
{
    int FactColAltSup;
    int FactColAltInf;
    int i,j,k,l,in;
    int brdax,brdbx,brday,brdby;
    int indax,indbx,inday,indby;    
    int brd1,brd2;
    point3d A;
//    punto L1,L2;
//    int altp1,altp2,altp3,altp4;
    int altagua;
    int altitud;
//    char col1,col2,col3,col4,colta,coltb;
//    int colpromedio;
//    float aux;
    V3D_f v1,v2,v3,v4;
    short int v1x,v1y,v2x,v2y,v3x,v3y,v4x,v4y;

    FactColAltSup =(int)((float)( RELAGUASUP /(NALTS-2) ) );
    FactColAltInf =(int)((float)( RELAGUAINF / 1.0) );

    brd1 =(dvs/2) -((Hijo->dvs)/4) + 1;
    brd2 = dvs - brd1 ;
    brdax=brd1+((Hijo->difx+1)/2); brdbx=brd2-((1-Hijo->difx)/2);
    brday=brd1+((Hijo->dify+1)/2); brdby=brd2-((1-Hijo->dify)/2);

    indax =(abs(difx) + 1) / 2 + 2 ;
    indbx = dvs - indax ;
    inday =(abs(dify) + 1) / 2 + 2;
    indby = dvs - inday ;

    altagua=0;

  int incremento_i = 1,incremento_j = 1,incremento_k = 1,incremento_l = 1;
  int iaux,jaux,kaux,laux;
  int imax,jmax,kmax,lmax;
  int iutil,jutil;
  int jstart,lstart;
  int medio;

  jstart = 0;
  lstart = 0;

  imax = indbx - indax ;

  jmax = indby - inday ;

  kmax = jmax ;

  lmax = imax ;

  medio = dvs / 2;

  if((area==1) ||(area==8)) { imax = 1; jmax = 1; incremento_l = -1; lstart = indbx-1;}
  if((area==2) ||(area==3)) { kmax = 1; lmax = 1;  incremento_j = 1; jstart = inday;}
  if((area==4) ||(area==5)) { imax = 1; jmax = 1; incremento_l = 1; lstart = indax;}
  if((area==6) ||(area==7)) { kmax = 1; lmax = 1;  incremento_j = -1; jstart = indby-1;}

  for( j = 0,jaux = jstart; j < jmax ; j++,jaux+=incremento_j)
  for( i = 0,iaux = indax,incremento_i = 1; i < imax ; i++,iaux+=incremento_i)
  for( l = 0,laux = lstart; l < lmax ; l++,laux+=incremento_l)
  for( k = 0,kaux = inday,incremento_k = 1; k < kmax ; k++,kaux+=incremento_k)
   {

       if(area==1 || area==8 || area==4 || area==5)
       {
        in = laux + kaux *(dvs+1);

        A.x = rad*(2*laux-dvs);
        A.y = -rad*(2*kaux-dvs);

        iutil = laux;
        jutil = kaux;

        if( k == medio - inday)
        {
         incremento_k = -incremento_k;
         kaux = indby;
        }

       }
       else
       {
        in = iaux + jaux *(dvs+1);

        A.x = rad*(2*iaux-dvs);
        A.y = -rad*(2*jaux-dvs);

        iutil = iaux;
        jutil = jaux;

        if( i == medio - indax)
        {
         incremento_i = -incremento_i;
         iaux = indbx;
        }

       }

        if(iutil<brdax+1 || iutil>=brdbx-1 || brdby-1<=jutil || jutil<brday+1)
           {

       if(L_PROY[jutil][iutil].SeDibuja==1 &&
           L_PROY[jutil][iutil+1].SeDibuja==1 &&
           L_PROY[jutil+1][iutil].SeDibuja==1 &&
           L_PROY[jutil+1][iutil+1].SeDibuja==1
           ) {

                    v1.x = L_PROY[jutil][iutil].P.x;
                    v1.y = L_PROY[jutil][iutil].P.y;
                    v1.z = 0.0;
                    v1.c = L_PROY[jutil][iutil].Intensidad;
                    altitud =  L_GP[jutil][iutil].alt;
                    v1.cf =((float) altitud/(float) MAXALT);
					v1.C = PaletaAlturas(v1.cf,altitud,ALPAL);

                    v2.x = L_PROY[jutil][iutil+1].P.x;
                    v2.y = L_PROY[jutil][iutil+1].P.y;
                    v2.z = 0.0;
                    v2.c = L_PROY[jutil][iutil+1].Intensidad;
                    altitud =  L_GP[jutil][iutil+1].alt;
					v2.cf =((float) altitud/(float) MAXALT);
					v2.C = PaletaAlturas(v2.cf,altitud,ALPAL);

                    v3.x = L_PROY[jutil+1][iutil].P.x;
                    v3.y = L_PROY[jutil+1][iutil].P.y;
                    v3.z = 0.0;
                    v3.c = L_PROY[jutil+1][iutil].Intensidad;
                    altitud =  L_GP[jutil+1][iutil].alt;
					v3.cf =((float) altitud/(float) MAXALT);
					v3.C = PaletaAlturas(v3.cf,altitud,ALPAL);

                    v4.x = L_PROY[jutil+1][iutil+1].P.x;
                    v4.y = L_PROY[jutil+1][iutil+1].P.y;
                    v4.z = 0.0;
                    v4.c = L_PROY[jutil+1][iutil+1].Intensidad;
                    altitud =  L_GP[jutil+1][iutil+1].alt;
					v4.cf =((float) altitud/(float) MAXALT);
					v4.C = PaletaAlturas(v4.cf,altitud,ALPAL);

//stretch_blit(GP[iutil+jutil*dvspp].TORIGEN,GP[iutil+jutil*dvspp].T,0,0,4,4,0,0,8,8);
//if(Nesima==4) GT[iutil+jutil*dvspp] = Padre->Padre->Padre->Padre->texturas[iutil+jutil*dvspp];

//textura = GT[iutil+jutil*dvspp];
/*
if(Nesima<=6 && Nesima>2) textura = texturas[GP[iutil+jutil*dvspp].NP];
else
textura = GT[iutil+jutil*dvspp];

v1.u = 0.0;
v1.v = 0.0;
//v1.c = col1;

v2.u = textura->w-1;
v2.v = 0.0;
//v2.c = col2;

v3.u = 0.0;
v3.v = textura->w-1;
//v3.c = col3;

v4.u = textura->w-1;
v4.v = textura->w-1;
*/
//v4.c = col4;
//triangle3d_f(pagina,POLYTYPE_ATEX,textura,&v1,&v2,&v3);
//triangle3d_f(pagina,POLYTYPE_ATEX,textura,&v2,&v3,&v4);
v1x =(int) v1.x;v1y =(int) v1.y;
v2x =(int) v2.x;v2y =(int) v2.y;
v3x =(int) v3.x;v3y =(int) v3.y;
v4x =(int) v4.x;v4y =(int) v4.y;
if(Nesima!=0)
{
//triangle3d_f(pagina,POLYTYPE_GCOL,textura,&v1,&v2,&v3);
//triangle3d_f(pagina,POLYTYPE_GCOL,textura,&v2,&v3,&v4);
//line(pagina,v1x,v1y,v2x,v2y,55);
//line(pagina,v2x,v2y,v3x,v3y,55);
//line(pagina,v1x,v1y,v3x,v3y,55);
glLoadIdentity();
glPointSize(2.0);
glBegin(GL_TRIANGLE_STRIP);
glColor3f(v1.C.r,v1.C.g,v1.C.b);
glVertex3f(v1.x,v1.y,-800.0);
glColor3f(v2.C.r,v2.C.g,v2.C.b);
glVertex3f(v2.x,v2.y,-800.0);
glColor3f(v3.C.r,v3.C.g,v3.C.b);
glVertex3f(v3.x,v3.y,-800.0);
glColor3f(v4.C.r,v4.C.g,v4.C.b);
glVertex3f(v4.x,v4.y,-800.0);
glEnd();
/*
glBegin(GL_LINE_STRIP);
glColor3f(0.5,0.5,0.5);
glVertex3f(v1.x,v1.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v2.x,v2.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v3.x,v3.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v4.x,v4.y,-800.0);
glEnd();
*/
//printf("%f\n",v1.x);
}
else
{
//triangle3d_f(pagina,POLYTYPE_GCOL,textura,&v1,&v2,&v3);
//triangle3d_f(pagina,POLYTYPE_GCOL,textura,&v2,&v3,&v4);
//line(pagina,v1x,v1y,v2x,v2y,55);
//line(pagina,v2x,v2y,v3x,v3y,55);
//line(pagina,v1x,v1y,v3x,v3y,55);
glLoadIdentity();
glPointSize(2.0);
glBegin(GL_TRIANGLE_STRIP);
glColor3f(v1.C.r,v1.C.g,v1.C.b);
glVertex3f(v1.x,v1.y,-800.0);
glColor3f(v2.C.r,v2.C.g,v2.C.b);
glVertex3f(v2.x,v2.y,-800.0);
glColor3f(v3.C.r,v3.C.g,v3.C.b);
glVertex3f(v3.x,v3.y,-800.0);
glColor3f(v4.C.r,v4.C.g,v4.C.b);
glVertex3f(v4.x,v4.y,-800.0);
glEnd();
/*
glBegin(GL_LINE_STRIP);
glColor3f(0.5,0.5,0.5);
glVertex3f(v1.x,v1.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v2.x,v2.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v3.x,v3.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v4.x,v4.y,-800.0);
glEnd();
*/

}

Poligonos+=2;
/*
v1.c = 150;
v2.c = 140;
triangle3d_f(pagina,POLYTYPE_FLAT,textura,&v1,&v2,&v3);
triangle3d_f(pagina,POLYTYPE_FLAT,textura,&v2,&v3,&v4);
*/

/*
if(key[KEY_P])
{
fprintf(res,"punto i:%i j:%i\n",iutil,jutil);
fprintf(res,"punto v1 x:%f y:%f\n",v1.x,v1.y);
fprintf(res,"punto v2 x:%f y:%f\n",v2.x,v2.y);
fprintf(res,"punto v3 x:%f y:%f\n",v3.x,v3.y);
fprintf(res,"punto v4 x:%f y:%f\n",v4.x,v4.y);
}
triangle(pagina,v1x,v1y,v2x,v2y,v3x,v3y,39);
triangle(pagina,v2x,v2y,v3x,v3y,v4x,v4y,255);
//putpixel(pagina,(short) v1.x,(short) v1.y , 255);
line(pagina,v1x,v1y,v2x,v2y,255);
line(pagina,v2x,v2y,v3x,v3y,255);
line(pagina,v1x,v1y,v3x,v3y,255);
*/
//               if(G[in].anim > 7) line(pagina,v1.x,v1.y,v1.x,v1.y-30,60);
               }

         }
       }

}

//-----------------------------------------------------------------------------------------


void
TGrid::DibujarAlturaTodo(int area,TBITMAP *pagina)
{
    int i,j,k,l,in;
    point3d A;
    int indax,indbx,inday,indby;
    int altitud;
//    char col1,col2,col3,col4,colta,coltb;
    V3D_f v1,v2,v3,v4;
    short int v1x,v1y,v2x,v2y,v3x,v3y,v4x,v4y;

    indax =(abs(difx) + 1) / 2 + 2 ;
    indbx = dvs - indax ;
    inday =(abs(dify) + 1) / 2 + 2;
    indby = dvs - inday ;

  int incremento_i = 1,incremento_j = 1,incremento_k = 1,incremento_l = 1;
  int iaux,jaux,kaux,laux;
  int imax,jmax,kmax,lmax;
  int iutil,jutil;
  int jstart,lstart;
  int medio;

  jstart = 0;
  lstart = 0;

  imax = indbx - indax ;

  jmax = indby - inday ;

  kmax = jmax ;

  lmax = imax ;

  medio = dvs / 2;

  if((area==1) ||(area==8)) { imax = 1; jmax = 1; incremento_l = -1; lstart = indbx-1;}
  if((area==2) ||(area==3)) { kmax = 1; lmax = 1;  incremento_j = 1; jstart = inday;}
  if((area==4) ||(area==5)) { imax = 1; jmax = 1; incremento_l = 1; lstart = indax;}
  if((area==6) ||(area==7)) { kmax = 1; lmax = 1;  incremento_j = -1; jstart = indby-1;}

  for( j = 0,jaux = jstart; j < jmax ; j++,jaux+=incremento_j)
  for( i = 0,iaux = indax,incremento_i = 1; i < imax ; i++,iaux+=incremento_i)
  for( l = 0,laux = lstart; l < lmax ; l++,laux+=incremento_l)
  for( k = 0,kaux = inday,incremento_k = 1; k < kmax ; k++,kaux+=incremento_k)

  {
       if(area==1 || area==8 || area==4 || area==5)
       {
        in = laux + kaux *(dvs+1);

        A.x = rad*(2*laux-dvs);
        A.y = -rad*(2*kaux-dvs);

        iutil = laux;
        jutil = kaux;

        if( k == medio - inday)
        {
         incremento_k = -incremento_k;
         kaux = indby;
        }

       }
       else
       {
        in = iaux + jaux *(dvs+1);

        A.x = rad*(2*iaux-dvs);
        A.y = -rad*(2*jaux-dvs);

        iutil = iaux;
        jutil = jaux;

        if( i == medio - indax)
        {
         incremento_i = -incremento_i;
         iaux = indbx;
        }

       }


       if(L_PROY[jutil][iutil].SeDibuja==1 &&
           L_PROY[jutil][iutil+1].SeDibuja==1 &&
           L_PROY[jutil+1][iutil].SeDibuja==1 &&
           L_PROY[jutil+1][iutil+1].SeDibuja==1
           ) {
                    v1.x = L_PROY[jutil][iutil].P.x;
                    v1.y = L_PROY[jutil][iutil].P.y;
                    v1.z = 0.0;
                    v1.c = L_PROY[jutil][iutil].Intensidad;
                    altitud =  L_GP[jutil][iutil].alt;
                    v1.cf =((float) altitud/(float) MAXALT);
					v1.C = PaletaAlturas(v1.cf,altitud,ALPAL);

                    v2.x = L_PROY[jutil][iutil+1].P.x;
                    v2.y = L_PROY[jutil][iutil+1].P.y;
                    v2.z = 0.0;
                    v2.c = L_PROY[jutil][iutil+1].Intensidad;
                    altitud =  L_GP[jutil][iutil+1].alt;
					v2.cf =((float) altitud/(float) MAXALT);
					v2.C = PaletaAlturas(v2.cf,altitud,ALPAL);

                    v3.x = L_PROY[jutil+1][iutil].P.x;
                    v3.y = L_PROY[jutil+1][iutil].P.y;
                    v3.z = 0.0;
                    v3.c = L_PROY[jutil+1][iutil].Intensidad;
                    altitud =  L_GP[jutil+1][iutil].alt;
					v3.cf =((float) altitud/(float) MAXALT);
					v3.C = PaletaAlturas(v3.cf,altitud,ALPAL);

                    v4.x = L_PROY[jutil+1][iutil+1].P.x;
                    v4.y = L_PROY[jutil+1][iutil+1].P.y;
                    v4.z = 0.0;
                    v4.c = L_PROY[jutil+1][iutil+1].Intensidad;
                    altitud =  L_GP[jutil+1][iutil+1].alt;
					v4.cf =((float) altitud/(float) MAXALT);
					v4.C = PaletaAlturas(v4.cf,altitud,ALPAL);

          
//textura = GT[iutil+jutil*dvspp];
/*
if(Nesima<=6 && Nesima>2) textura = texturas[GP[iutil+jutil*dvspp].NP];
else
textura = GT[iutil+jutil*dvspp];


v1.u = 0.0;
v1.v = 0.0;

v2.u = textura->w-1;
v2.v = 0.0;

v3.u = 0.0;
v3.v = textura->w-1;

v4.u = textura->w-1;
v4.v = textura->w-1;
*/
v1x =(int) v1.x;v1y =(int) v1.y;
v2x =(int) v2.x;v2y =(int) v2.y;
v3x =(int) v3.x;v3y =(int) v3.y;
v4x =(int) v4.x;v4y =(int) v4.y;
if(Nesima!=0)
{
//triangle3d_f(pagina,POLYTYPE_GCOL,textura,&v1,&v2,&v3);
//triangle3d_f(pagina,POLYTYPE_GCOL,textura,&v2,&v3,&v4);
//line(pagina,v1x,v1y,v2x,v2y,55);
//line(pagina,v2x,v2y,v3x,v3y,55);
//line(pagina,v1x,v1y,v3x,v3y,55);
glLoadIdentity();
glPointSize(2.0);
glBegin(GL_TRIANGLE_STRIP);
glColor3f(v1.C.r,v1.C.g,v1.C.b);
glVertex3f(v1.x,v1.y,-800.0);
glColor3f(v2.C.r,v2.C.g,v2.C.b);
glVertex3f(v2.x,v2.y,-800.0);
glColor3f(v3.C.r,v3.C.g,v3.C.b);
glVertex3f(v3.x,v3.y,-800.0);
glColor3f(v4.C.r,v4.C.g,v4.C.b);
glVertex3f(v4.x,v4.y,-800.0);
glEnd();
/*
glBegin(GL_LINE_STRIP);
glColor3f(0.5,0.5,0.5);
glVertex3f(v1.x,v1.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v2.x,v2.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v3.x,v3.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v4.x,v4.y,-800.0);
glEnd();
*/
}
else
{
//triangle3d_f(pagina,POLYTYPE_GCOL,textura,&v1,&v2,&v3);
//triangle3d_f(pagina,POLYTYPE_GCOL,textura,&v2,&v3,&v4);
//line(pagina,v1x,v1y,v2x,v2y,55);
//line(pagina,v2x,v2y,v3x,v3y,55);
//line(pagina,v1x,v1y,v3x,v3y,55);
glLoadIdentity();
glPointSize(2.0);
glBegin(GL_TRIANGLE_STRIP);
glColor3f(v1.C.r,v1.C.g,v1.C.b);
glVertex3f(v1.x,v1.y,-800.0);
glColor3f(v2.C.r,v2.C.g,v2.C.b);
glVertex3f(v2.x,v2.y,-800.0);
glColor3f(v3.C.r,v3.C.g,v3.C.b);
glVertex3f(v3.x,v3.y,-800.0);
glColor3f(v4.C.r,v4.C.g,v4.C.b);
glVertex3f(v4.x,v4.y,-800.0);
glEnd();
/*
glBegin(GL_LINE_STRIP);
glColor3f(0.5,0.5,0.5);
glVertex3f(v1.x,v1.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v2.x,v2.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v3.x,v3.y,-800.0);
glColor3f(0.5,0.5,0.5);
glVertex3f(v4.x,v4.y,-800.0);
glEnd();
*/
}

Poligonos+=2;
                   }
         }
}


//******************
///************************************************************************

void
TGrid::glDibujarAlturaBorde(int area,TBITMAP *pagina,TCAMARA3D* CAM)
{
    int FactColAltSup;
    int FactColAltInf;
    int i,j,k,l,in;
    int brdax,brdbx,brday,brdby;
    int indax,indbx,inday,indby;    
    int brd1,brd2;
    point3d A;
//    punto L1,L2;
//    int altp1,altp2,altp3,altp4;
    int altagua;
    int altitud;
//    char col1,col2,col3,col4,colta,coltb;
//    int colpromedio;
//    float aux;
    V3D_f v1,v2,v3,v4;
    short int v1x,v1y,v2x,v2y,v3x,v3y,v4x,v4y;

    FactColAltSup =(int)((float)( RELAGUASUP /(NALTS-2) ) );
    FactColAltInf =(int)((float)( RELAGUAINF / 1.0) );

    brd1 =(dvs/2) -((Hijo->dvs)/4) + 1;
    brd2 = dvs - brd1 ;
    brdax=brd1+((Hijo->difx+1)/2); brdbx=brd2-((1-Hijo->difx)/2);
    brday=brd1+((Hijo->dify+1)/2); brdby=brd2-((1-Hijo->dify)/2);

    indax =(abs(difx) + 1) / 2 + 2 ;
    indbx = dvs - indax ;
    inday =(abs(dify) + 1) / 2 + 2;
    indby = dvs - inday ;

    altagua=0;

  int incremento_i = 1,incremento_j = 1,incremento_k = 1,incremento_l = 1;
  int iaux,jaux,kaux,laux;
  int imax,jmax,kmax,lmax;
  int iutil,jutil;
  int jstart,lstart;
  int medio;

  jstart = 0;
  lstart = 0;

  imax = indbx - indax ;

  jmax = indby - inday ;

  kmax = jmax ;

  lmax = imax ;

  medio = dvs / 2;

  if((area==1) ||(area==8)) { imax = 1; jmax = 1; incremento_l = -1; lstart = indbx-1;}
  if((area==2) ||(area==3)) { kmax = 1; lmax = 1;  incremento_j = 1; jstart = inday;}
  if((area==4) ||(area==5)) { imax = 1; jmax = 1; incremento_l = 1; lstart = indax;}
  if((area==6) ||(area==7)) { kmax = 1; lmax = 1;  incremento_j = -1; jstart = indby-1;}

  for( j = 0,jaux = jstart; j < jmax ; j++,jaux+=incremento_j)
  for( i = 0,iaux = indax,incremento_i = 1; i < imax ; i++,iaux+=incremento_i)
  for( l = 0,laux = lstart; l < lmax ; l++,laux+=incremento_l)
  for( k = 0,kaux = inday,incremento_k = 1; k < kmax ; k++,kaux+=incremento_k)
   {

       if(area==1 || area==8 || area==4 || area==5)
       {
        in = laux + kaux *(dvs+1);

        A.x = rad*(2*laux-dvs);
        A.y = -rad*(2*kaux-dvs);

        iutil = laux;
        jutil = kaux;

        if( k == medio - inday)
        {
         incremento_k = -incremento_k;
         kaux = indby;
        }

       }
       else
       {
        in = iaux + jaux *(dvs+1);

        A.x = rad*(2*iaux-dvs);
        A.y = -rad*(2*jaux-dvs);

        iutil = iaux;
        jutil = jaux;

        if( i == medio - indax)
        {
         incremento_i = -incremento_i;
         iaux = indbx;
        }

       }

        if(iutil<brdax+1 || iutil>=brdbx-1 || brdby-1<=jutil || jutil<brday+1)
           {
       if(L_PROY[jutil][iutil].SeDibuja==1 &&
           L_PROY[jutil][iutil+1].SeDibuja==1 &&
           L_PROY[jutil+1][iutil].SeDibuja==1 &&
           L_PROY[jutil+1][iutil+1].SeDibuja==1
           ) {
                    v1.x = L_PROY[jutil][iutil].P3d.x;
                    v1.y = L_PROY[jutil][iutil].P3d.y;
                    v1.z = L_PROY[jutil][iutil].P3d.z;
                    v1.c = L_PROY[jutil][iutil].Intensidad;
                    altitud =  L_GP[jutil][iutil].alt;
					v1.u = L_GP[jutil][iutil].txa;
					v1.v = L_GP[jutil][iutil].tya;
                    v1.cf =((float) altitud/(float) MAXALT);
					v1.C = PaletaAlturas(v1.cf,altitud,ALPAL);
					if(Nesima>0) {
					v1.C.r = v1.C.r*L_PROY[jutil][iutil].If;
					v1.C.g = v1.C.g*L_PROY[jutil][iutil].If;
					v1.C.b = v1.C.b*L_PROY[jutil][iutil].If;
					}
					v1.N = L_PROY[jutil][iutil].N;

                    v2.x = L_PROY[jutil][iutil+1].P3d.x;
                    v2.y = L_PROY[jutil][iutil+1].P3d.y;
                    v2.z = L_PROY[jutil][iutil+1].P3d.z;
                    v2.c = L_PROY[jutil][iutil+1].Intensidad;
                    altitud =  L_GP[jutil][iutil+1].alt;
					v2.u = L_GP[jutil][iutil+1].txa;
					v2.v = L_GP[jutil][iutil+1].tya;
					v2.cf =((float) altitud/(float) MAXALT);
					v2.C = PaletaAlturas(v2.cf,altitud,ALPAL);
					if(Nesima>0) {
					v2.C.r = v2.C.r*L_PROY[jutil][iutil+1].If;
					v2.C.g = v2.C.g*L_PROY[jutil][iutil+1].If;
					v2.C.b = v2.C.b*L_PROY[jutil][iutil+1].If;
					}
					v2.N = L_PROY[jutil][iutil+1].N;

                    v3.x = L_PROY[jutil+1][iutil].P3d.x;
                    v3.y = L_PROY[jutil+1][iutil].P3d.y;
                    v3.z = L_PROY[jutil+1][iutil].P3d.z;
                    v3.c = L_PROY[jutil+1][iutil].Intensidad;
                    altitud =  L_GP[jutil+1][iutil].alt;
					v3.u = L_GP[jutil+1][iutil].txa;
					v3.v = L_GP[jutil+1][iutil].tya;
					v3.cf =((float) altitud/(float) MAXALT);
					v3.C = PaletaAlturas(v3.cf,altitud,ALPAL);
					if(Nesima>0) {
					v3.C.r = v3.C.r*L_PROY[jutil+1][iutil].If;
					v3.C.g = v3.C.g*L_PROY[jutil+1][iutil].If;
					v3.C.b = v3.C.b*L_PROY[jutil+1][iutil].If;
					}
					v3.N = L_PROY[jutil+1][iutil].N;

                    v4.x = L_PROY[jutil+1][iutil+1].P3d.x;
                    v4.y = L_PROY[jutil+1][iutil+1].P3d.y;
                    v4.z = L_PROY[jutil+1][iutil+1].P3d.z;
                    v4.c = L_PROY[jutil+1][iutil+1].Intensidad;
                    altitud =  L_GP[jutil+1][iutil+1].alt;
					v4.u = L_GP[jutil+1][iutil+1].txa;
					v4.v = L_GP[jutil+1][iutil+1].tya;
					v4.cf =((float) altitud/(float) MAXALT);
					v4.C = PaletaAlturas(v4.cf,altitud,ALPAL);
					if(Nesima>0) {
					v4.C.r = v4.C.r*L_PROY[jutil+1][iutil+1].If;
					v4.C.g = v4.C.g*L_PROY[jutil+1][iutil+1].If;
					v4.C.b = v4.C.b*L_PROY[jutil+1][iutil+1].If;
					}
					v4.N = L_PROY[jutil+1][iutil+1].N;

//stretch_blit(GP[iutil+jutil*dvspp].TORIGEN,GP[iutil+jutil*dvspp].T,0,0,4,4,0,0,8,8);
//if(Nesima==4) GT[iutil+jutil*dvspp] = Padre->Padre->Padre->Padre->texturas[iutil+jutil*dvspp];

//textura = GT[iutil+jutil*dvspp];
/*
if(Nesima<=6 && Nesima>2) textura = texturas[GP[iutil+jutil*dvspp].NP];
else
textura = GT[iutil+jutil*dvspp];

v1.u = 0.0;
v1.v = 0.0;
//v1.c = col1;

v2.u = textura->w-1;
v2.v = 0.0;
//v2.c = col2;

v3.u = 0.0;
v3.v = textura->w-1;
//v3.c = col3;

v4.u = textura->w-1;
v4.v = textura->w-1;
*/
//v4.c = col4;
//triangle3d_f(pagina,POLYTYPE_ATEX,textura,&v1,&v2,&v3);
//triangle3d_f(pagina,POLYTYPE_ATEX,textura,&v2,&v3,&v4);
v1x =(int) v1.x;v1y =(int) v1.y;
v2x =(int) v2.x;v2y =(int) v2.y;
v3x =(int) v3.x;v3y =(int) v3.y;
v4x =(int) v4.x;v4y =(int) v4.y;
glLoadIdentity();
gluLookAt(0.0,0.0,CAM->Oz,CAM->u.x,CAM->u.y,CAM->u.z+CAM->Oz,CAM->w.x,CAM->w.y,CAM->w.z);

//glPolygonMode(GL_FRONT,GL_FILL);
/*
glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
glFrontFace(GL_CW);
*/

glColor4f(1.0f,1.0f,1.0f,1.0f);

/*
glBegin(GL_TRIANGLE_STRIP);
glNormal3f(v1.N.x,v1.N.y,v1.N.z);
glColor3f(v1.C.r,v1.C.g,v1.C.b);
glTexCoord2f(1.0,0.0);
glVertex3f(v1.x,v1.y,v1.z);

glNormal3f(v2.N.x,v2.N.y,v2.N.z);
glColor3f(v2.C.r,v2.C.g,v2.C.b);
glTexCoord2f(1.0,1.0);
glVertex3f(v2.x,v2.y,v2.z);

glNormal3f(v3.N.x,v3.N.y,v3.N.z);
glColor3f(v3.C.r,v3.C.g,v3.C.b);
glTexCoord2f(0.0,1.0);
glVertex3f(v3.x,v3.y,v3.z);

glNormal3f(v4.N.x,v4.N.y,v4.N.z);
glColor3f(v4.C.r,v4.C.g,v4.C.b);
glTexCoord2f(0.0,0.0);
glVertex3f(v4.x,v4.y,v4.z);
glEnd();
*/
if(Nesima==1) {
	//echo
	v1x = v1.x;
}
glBegin(GL_TRIANGLE_STRIP);
glNormal3f(v1.N.x,v1.N.y,v1.N.z);
glColor3f(v1.C.r,v1.C.g,v1.C.b);
glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v1.u,v1.v);
//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
//glTexCoord2f(v1.u,v1.v);
glVertex3f(v1.x,v1.y,v1.z);

glNormal3f(v2.N.x,v2.N.y,v2.N.z);
glColor3f(v2.C.r,v2.C.g,v2.C.b);
glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v2.u,v2.v);
//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
//glTexCoord2f(v2.u,v2.v);
glVertex3f(v2.x,v2.y,v2.z);

glNormal3f(v3.N.x,v3.N.y,v3.N.z);
glColor3f(v3.C.r,v3.C.g,v3.C.b);
glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v3.u,v3.v);
//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
//glTexCoord2f(v3.u,v3.v);
glVertex3f(v3.x,v3.y,v3.z);

glNormal3f(v4.N.x,v4.N.y,v4.N.z);
glColor3f(v4.C.r,v4.C.g,v4.C.b);
glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v4.u,v4.v);
//glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
//glTexCoord2f(v4.u,v4.v);
glVertex3f(v4.x,v4.y,v4.z);
glEnd();

/*
glBegin(GL_LINE_STRIP);
glColor3f(0.5,0.5,0.5);
glVertex3f(v1.x,v1.y,v1.z);
glColor3f(0.5,0.5,0.5);
glVertex3f(v2.x,v2.y,v2.z);
glColor3f(0.5,0.5,0.5);
glVertex3f(v3.x,v3.y,v3.z);
glColor3f(0.5,0.5,0.5);
glVertex3f(v4.x,v4.y,v4.z);
glEnd();
*/
Poligonos+=2;
			}
         }
       }

}

//-----------------------------------------------------------------------------------------


void
TGrid::glDibujarAlturaTodo(int area,TBITMAP *pagina,TCAMARA3D* CAM)
{
    int i,j,k,l,in;
    point3d A;
    int indax,indbx,inday,indby;
    int altitud;
//    char col1,col2,col3,col4,colta,coltb;
    V3D_f v1,v2,v3,v4;
    short int v1x,v1y,v2x,v2y,v3x,v3y,v4x,v4y;

    indax =(abs(difx) + 1) / 2 + 2 ;
    indbx = dvs - indax ;
    inday =(abs(dify) + 1) / 2 + 2;
    indby = dvs - inday ;

  int incremento_i = 1,incremento_j = 1,incremento_k = 1,incremento_l = 1;
  int iaux,jaux,kaux,laux;
  int imax,jmax,kmax,lmax;
  int iutil,jutil;
  int jstart,lstart;
  int medio;

  jstart = 0;
  lstart = 0;

  imax = indbx - indax ;

  jmax = indby - inday ;

  kmax = jmax ;

  lmax = imax ;

  medio = dvs / 2;

  if((area==1) ||(area==8)) { imax = 1; jmax = 1; incremento_l = -1; lstart = indbx-1;}
  if((area==2) ||(area==3)) { kmax = 1; lmax = 1;  incremento_j = 1; jstart = inday;}
  if((area==4) ||(area==5)) { imax = 1; jmax = 1; incremento_l = 1; lstart = indax;}
  if((area==6) ||(area==7)) { kmax = 1; lmax = 1;  incremento_j = -1; jstart = indby-1;}

  for( j = 0,jaux = jstart; j < jmax ; j++,jaux+=incremento_j)
  for( i = 0,iaux = indax,incremento_i = 1; i < imax ; i++,iaux+=incremento_i)
  for( l = 0,laux = lstart; l < lmax ; l++,laux+=incremento_l)
  for( k = 0,kaux = inday,incremento_k = 1; k < kmax ; k++,kaux+=incremento_k)

  {
       if(area==1 || area==8 || area==4 || area==5)
       {
        in = laux + kaux *(dvs+1);

        A.x = rad*(2*laux-dvs);
        A.y = -rad*(2*kaux-dvs);

        iutil = laux;
        jutil = kaux;

        if( k == medio - inday)
        {
         incremento_k = -incremento_k;
         kaux = indby;
        }

       }
       else
       {
        in = iaux + jaux *(dvs+1);

        A.x = rad*(2*iaux-dvs);
        A.y = -rad*(2*jaux-dvs);

        iutil = iaux;
        jutil = jaux;

        if( i == medio - indax)
        {
         incremento_i = -incremento_i;
         iaux = indbx;
        }

       }

       if(L_PROY[jutil][iutil].SeDibuja==1 &&
           L_PROY[jutil][iutil+1].SeDibuja==1 &&
           L_PROY[jutil+1][iutil].SeDibuja==1 &&
           L_PROY[jutil+1][iutil+1].SeDibuja==1
           ) {
                    v1.x = L_PROY[jutil][iutil].P3d.x;
                    v1.y = L_PROY[jutil][iutil].P3d.y;
                    v1.z = L_PROY[jutil][iutil].P3d.z;
                    v1.c = L_PROY[jutil][iutil].Intensidad;
                    altitud =  L_GP[jutil][iutil].alt;
					v1.u = L_GP[jutil][iutil].txa;
					v1.v = L_GP[jutil][iutil].tya;
                    v1.cf =((float) altitud/(float) MAXALT);
					v1.C = PaletaAlturas(v1.cf,altitud,ALPAL);
					if(Nesima>0) {
					v1.C.r = v1.C.r*L_PROY[jutil][iutil].If;
					v1.C.g = v1.C.g*L_PROY[jutil][iutil].If;
					v1.C.b = v1.C.b*L_PROY[jutil][iutil].If;
					}
					v1.N = L_PROY[jutil][iutil].N;

                    v2.x = L_PROY[jutil][iutil+1].P3d.x;
                    v2.y = L_PROY[jutil][iutil+1].P3d.y;
                    v2.z = L_PROY[jutil][iutil+1].P3d.z;
                    v2.c = L_PROY[jutil][iutil+1].Intensidad;
                    altitud =  L_GP[jutil][iutil+1].alt;
					v2.u = L_GP[jutil][iutil+1].txa;
					v2.v = L_GP[jutil][iutil+1].tya;
					v2.cf =((float) altitud/(float) MAXALT);
					v2.C = PaletaAlturas(v2.cf,altitud,ALPAL);
					if(Nesima>0) {
					v2.C.r = v2.C.r*L_PROY[jutil][iutil+1].If;
					v2.C.g = v2.C.g*L_PROY[jutil][iutil+1].If;
					v2.C.b = v2.C.b*L_PROY[jutil][iutil+1].If;
					}
					v2.N = L_PROY[jutil][iutil+1].N;

                    v3.x = L_PROY[jutil+1][iutil].P3d.x;
                    v3.y = L_PROY[jutil+1][iutil].P3d.y;
                    v3.z = L_PROY[jutil+1][iutil].P3d.z;
                    v3.c = L_PROY[jutil+1][iutil].Intensidad;
                    altitud =  L_GP[jutil+1][iutil].alt;
					v3.u = L_GP[jutil+1][iutil].txa;
					v3.v = L_GP[jutil+1][iutil].tya;
					v3.cf =((float) altitud/(float) MAXALT);
					v3.C = PaletaAlturas(v3.cf,altitud,ALPAL);
					if(Nesima>0) {
					v3.C.r = v3.C.r*L_PROY[jutil+1][iutil].If;
					v3.C.g = v3.C.g*L_PROY[jutil+1][iutil].If;
					v3.C.b = v3.C.b*L_PROY[jutil+1][iutil].If;
					}
					v3.N = L_PROY[jutil+1][iutil].N;

                    v4.x = L_PROY[jutil+1][iutil+1].P3d.x;
                    v4.y = L_PROY[jutil+1][iutil+1].P3d.y;
                    v4.z = L_PROY[jutil+1][iutil+1].P3d.z;
                    v4.c = L_PROY[jutil+1][iutil+1].Intensidad;
                    altitud =  L_GP[jutil+1][iutil+1].alt;
					v4.u = L_GP[jutil+1][iutil+1].txa;
					v4.v = L_GP[jutil+1][iutil+1].tya;
					v4.cf =((float) altitud/(float) MAXALT);
					v4.C = PaletaAlturas(v4.cf,altitud,ALPAL);
					if(Nesima>0) {
					v4.C.r = v4.C.r*L_PROY[jutil+1][iutil+1].If;
					v4.C.g = v4.C.g*L_PROY[jutil+1][iutil+1].If;
					v4.C.b = v4.C.b*L_PROY[jutil+1][iutil+1].If;
					}
					v4.N = L_PROY[jutil+1][iutil+1].N;
          
//textura = GT[iutil+jutil*dvspp];
/*
if(Nesima<=6 && Nesima>2) textura = texturas[GP[iutil+jutil*dvspp].NP];
else
textura = GT[iutil+jutil*dvspp];


v1.u = 0.0;
v1.v = 0.0;

v2.u = textura->w-1;
v2.v = 0.0;

v3.u = 0.0;
v3.v = textura->w-1;

v4.u = textura->w-1;
v4.v = textura->w-1;
*/
v1x =(int) v1.x;v1y =(int) v1.y;
v2x =(int) v2.x;v2y =(int) v2.y;
v3x =(int) v3.x;v3y =(int) v3.y;
v4x =(int) v4.x;v4y =(int) v4.y;
glLoadIdentity();
gluLookAt(0.0,0.0,CAM->Oz,CAM->u.x,CAM->u.y,CAM->u.z+CAM->Oz,CAM->w.x,CAM->w.y,CAM->w.z);


//glPolygonMode(GL_FRONT,GL_FILL);
/*
glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
glFrontFace(GL_CW);
*/

glColor4f(1.0f,1.0f,1.0f,1.0f);
/*
glBegin(GL_TRIANGLE_STRIP);
glNormal3f(v1.N.x,v1.N.y,v1.N.z);
glColor3f(v1.C.r,v1.C.g,v1.C.b);
glTexCoord2f(1.0,0.0);
glVertex3f(v1.x,v1.y,v1.z);

glNormal3f(v2.N.x,v2.N.y,v2.N.z);
glColor3f(v2.C.r,v2.C.g,v2.C.b);
glTexCoord2f(1.0,1.0);
glVertex3f(v2.x,v2.y,v2.z);

glNormal3f(v3.N.x,v3.N.y,v3.N.z);
glColor3f(v3.C.r,v3.C.g,v3.C.b);
glTexCoord2f(0.0,1.0);
glVertex3f(v3.x,v3.y,v3.z);

glNormal3f(v4.N.x,v4.N.y,v4.N.z);
glColor3f(v4.C.r,v4.C.g,v4.C.b);
glTexCoord2f(0.0,0.0);
glVertex3f(v4.x,v4.y,v4.z);
glEnd();
*/
glBegin(GL_TRIANGLE_STRIP);
glNormal3f(v1.N.x,v1.N.y,v1.N.z);
glColor3f(v1.C.r,v1.C.g,v1.C.b);
glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v1.u,v1.v);
glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
//glTexCoord2f(v1.u,v1.v);
glVertex3f(v1.x,v1.y,v1.z);

glNormal3f(v2.N.x,v2.N.y,v2.N.z);
glColor3f(v2.C.r,v2.C.g,v2.C.b);
glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v2.u,v2.v);
glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
//glTexCoord2f(v2.u,v2.v);
glVertex3f(v2.x,v2.y,v2.z);

glNormal3f(v3.N.x,v3.N.y,v3.N.z);
glColor3f(v3.C.r,v3.C.g,v3.C.b);
glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v3.u,v3.v);
glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
//glTexCoord2f(v3.u,v3.v);
glVertex3f(v3.x,v3.y,v3.z);

glNormal3f(v4.N.x,v4.N.y,v4.N.z);
glColor3f(v4.C.r,v4.C.g,v4.C.b);
glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v4.u,v4.v);
glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
//glTexCoord2f(v4.u,v4.v);
glVertex3f(v4.x,v4.y,v4.z);
glEnd();

/*
glBegin(GL_LINE_STRIP);
glColor3f(0.5,0.5,0.5);
glVertex3f(v1.x,v1.y,v1.z);
glColor3f(0.5,0.5,0.5);
glVertex3f(v2.x,v2.y,v2.z);
glColor3f(0.5,0.5,0.5);
glVertex3f(v3.x,v3.y,v3.z);
glColor3f(0.5,0.5,0.5);
glVertex3f(v4.x,v4.y,v4.z);
glEnd();
*/
Poligonos+=2;
			}
         }
}
