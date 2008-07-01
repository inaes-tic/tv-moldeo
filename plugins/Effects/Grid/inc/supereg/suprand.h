#include<stdlib.h>
#include<math.h>

#ifndef __SUPRAND_H__
#define __SUPRAND_H__

const float tablagauss[101]=

     {(float)(-3.49),(float)(-2.33),(float)(-2.05),(float)(-1.88),(float)(-1.75),(float)(-1.64),(float)(-1.55),(float)(-1.48),(float)(-1.40),(float)(-1.34),
	(float)(-1.28),(float)(-1.22),(float)(-1.17),(float)(-1.13),(float)(-1.08),(float)(-1.04),(float)(-0.99),(float)(-0.95),(float)(-0.92),(float)(-0.88),
	 (float)(-0.84),(float)(-0.81),(float)(-0.77),(float)(-0.74),(float)(-0.71),(float)(-0.67),(float)(-0.64),(float)(-0.61),(float)(-0.58),(float)(-0.55),
	 (float)(-0.52),(float)(-0.50),(float)(-0.47),(float)(-0.44),(float)(-0.41),(float)(-0.39),(float)(-0.36),(float)(-0.33),(float)(-0.31),(float)(-0.28),
	 (float)(-0.25),(float)(-0.23),(float)(-0.20),(float)(-0.18),(float)(-0.15),(float)(-0.13),(float)(-0.10),(float)(-0.08),(float)(-0.05),(float)(-0.02),
	 (float)(0.0),(float)(0.03),(float)(0.05),(float)(0.08),(float)(0.10),(float)(0.13),(float)(0.15),(float)(0.18),(float)(0.20),(float)(0.23),(float)(0.25),
	 (float)(0.28),(float)(0.31),(float)(0.33),(float)(0.36),(float)(0.39),(float)(0.41),(float)(0.44),(float)(0.47),(float)(0.50),(float)(0.52),
	 (float)(0.55),(float)(0.58),(float)(0.61),(float)(0.64),(float)(0.67),(float)(0.71),(float)(0.74),(float)(0.77),(float)(0.81),(float)(0.84),
	 (float)(0.88),(float)(0.92),(float)(0.95),(float)(0.99),(float)(1.04),(float)(1.08),(float)(1.13),(float)(1.17),(float)(1.23),(float)(1.28),
	 (float)(1.34),(float)(1.41),(float)(1.48),(float)(1.56),(float)(1.65),(float)(1.75),(float)(1.88),(float)(2.06),(float)(2.33),(float)(3.49)};


unsigned int Randonne(float semil,unsigned int modulo);
/*Devuelve un entero al azar entre 0 y rango*/
int randon(int rango);
/*
Devuelve un valor aleatorio dada una cierta semilla, pero
esta vez la distribucion de esta variable aleatoria es
la de un funcion de Gauss
*/
float gauss(float sem);

#endif


