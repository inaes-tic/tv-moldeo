#include"supereg/suprand.h"
 
/*
const float tablagauss_extended[1001]=

     {-3.490,-3.100,-2.880,-2.750,-2.650,-2.575,-2.510,-2.455,-2.410,-2.365,
      -2.325,-2.290,-2.255,-2.225,-2.200,-2.170,-2.145,-2.120,-2.100,-2.075,
      -2.055,-2.035,-2.015,-1.995,-1.975,-1.960,-1.942.-1.925,-1.910,-1.895,      
      -1.880,-1.865,-1.850,-1.840,-1.825,-1.810,-1.800,-1.786,-1.775,-1.765,
      -1.750,-1.740,-1.729,-1.718,-1.705,-1.795,
      -1.64,
      -1.55,
      -1.48,
      -1.40,
      -1.34,
      -1.28,
      -1.22,
      -1.17,
      -1.13,
      -1.08,
      -1.04,
      -0.99,
      -0.95,
      -0.92,
      -0.88,
      -0.84,
      -0.81,
      -0.77,
      -0.74,
      -0.71,
      -0.67,
      -0.64,
      -0.61,
      -0.58,
      -0.55,
      -0.52,
      -0.50,
      -0.47,
      -0.44,
      -0.41,
      -0.39,
      -0.36,
      -0.33,
      -0.31,
      -0.28,
      -0.25,
      -0.23,
      -0.20,
      -0.18,
      -0.15,
      -0.13,
      -0.10,
      -0.08,
      -0.05,
      -0.02,
      0,
      0.03,
      0.05,
      0.08,
      0.10,
      0.13,
      0.15,
      0.18,
      0.20,
      0.23,
      0.25,
      0.28,
      0.31,
      0.33,
      0.36,
      0.39,
      0.41,
      0.44,
      0.47,
      0.50,
      0.52,
      0.55,
      0.58,
      0.61,
      0.64,
      0.67,
      0.71,
      0.74,
      0.77,
      0.81,
      0.84,
      0.88,
      0.92,
      0.95,
      0.99,
      1.04,
      1.08,
      1.13,
      1.17,
      1.23,
      1.28,
      1.34,
      1.41,
      1.48,
      1.56,
      1.65,
      1.75,
      1.88,
      2.06,
      2.33,
      3.49};

*/


/*
Devuelve un entero al azar entre 0  y el valor modulo
Si la semilla es la misma devuelve el mismo valor
la idea de esta funcion es hacer un random no tan bueno
pero rapido, pseudoaleatorio
*/
unsigned int Randonne(float semil,unsigned int modulo)
{
 int a,b,i;
 int Z1,Z2;
 a=127; b=129;
 Z1 =(int) semil;

 for(i=0; i<=4; i++)
     {Z2=Z1*a+b;
      Z2=ldiv(Z2,32767).rem;
      Z1=Z2;
      }

 Z2=ldiv(Z2,modulo+1).rem;
 return((unsigned int) sqrt(float(Z2*Z2)));
}


/*Devuelve un entero al azar entre 0 y rango*/
int randon(int rango)
{
 float col;

 col =((float) rand() *(float) rango ) /((float)RAND_MAX) ;

 return((int) col);
}


/*
Devuelve un valor aleatorio dada una cierta semilla, pero
esta vez la distribucion de esta variable aleatoria es
la de un funcion de Gauss
*/
float gauss(float sem)
{
  return(tablagauss[Randonne(sem,100)]);
}

