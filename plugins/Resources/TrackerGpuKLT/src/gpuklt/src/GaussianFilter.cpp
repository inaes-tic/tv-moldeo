
// --------------------------------------------------------------------------
/*! \file GaussianFilter.cpp
* \brief GaussianFilter Class File. 
*
* GpuUrban Library Code:
*/

//----------------------------------------------------------------------
//
// Date: Feb 01, 2006. UNC. (c) Sudipta N. Sinha @ Urbanscape
//
// Work: GpuUrban Library Src Code
//
// Copyright Notice:
//
//   This code and attached libraries cannot be modified, reproduced or copied in any 
//   form without the expressed consent of Dept. of Computer Science, UNC Chapel Hill.
// 
//   Copyright Dept. of Computer Science, UNC Chapel Hill.
//   All rights reserved.
//
//----------------------------------------------------------------------


#define _BUILDING_GPU_VIS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GaussianFilter.h"



/*!
\fn void GaussianFilter::evaluateFunc(float **fvals, float **deriv, int& n, float sigma)
\brief Evaluate size of 1D filter kernel and compute its elements (1D gaussian and 1D gaussian deriv)
*/
void GaussianFilter::evaluateFunc(float **fvals, float **deriv, int& n, float sigma)
{
	float  sigmaSq = sigma*sigma;
	float  sqrt2PI = sqrt(2*M_PI);
	int    x;

	n  = getLevel(sigma);
    
	(*fvals) = new float [ 2*n + 1];
	(*deriv) = new float [ 2*n + 1];
    float sum1 = 0;
	for (x=-n;x<=n;x++) 
	{
		(*fvals)[x+n] = exp(-(pow((float)x,2))/(2*sigmaSq)) / (sigma * sqrt2PI);
		(*deriv)[x+n] = (-x/sigmaSq) * (*fvals)[x+n];
		sum1 += (*fvals)[x+n];
	}

	for (x=-n;x<=n;x++)
	{
		if (sum1 != 0.0 ) (*fvals)[x+n] = (*fvals)[x+n] / sum1;
	}

	return;
}



/*!
\fn GaussianFilter::GaussianFilter (int arch, GLenum target, float Sig, CGcontext& context, CGprofile& vProfile , CGprofile& fProfile)
:SeparableFilter(Sig, context, vProfile, fProfile)
\brief Constructor : Create the 1D filter kernel : calls the constructor of parent class.
*/
GaussianFilter::GaussianFilter (int arch, GLenum target, float Sig, CGcontext& context, CGprofile& vProfile , CGprofile& fProfile)
:SeparableFilter(arch, target, Sig, context, vProfile, fProfile)
{
	float *  fvals, * deriv;

	_sigma  = Sig;

	evaluateFunc(&fvals, &deriv, _size, _sigma);

	for (int i=-_size ;i<= _size;i++)
		printf("%f %f\n",fvals[i+_size], deriv[i+_size]);
	
	setRowKernel( 2*_size + 1, fvals, deriv);
	setColKernel( 2*_size + 1, fvals, deriv);
	
	printf("_size %d\n",_size);

	if (target == GL_TEXTURE_2D)
	{
		createAndLoadRowShaders_ATI();
		createAndLoadColShaders_ATI();
	}
	else if ( target == GL_TEXTURE_RECTANGLE_NV)
	{
		createAndLoadRowShaders_NVIDIA();
		createAndLoadColShaders_NVIDIA();
	}

    printf("\tGpuVis:GaussianFilter::Constructor Successfully Executed...\n");
	delete[] fvals;
}


/*!
\fn void  GaussianFilter::Set(float Sig)
\brief Set the filter kernel
*/
void  GaussianFilter::Set(float Sig)
{
	float *  fvals, *deriv;

	_sigma  = Sig;

	evaluateFunc(&fvals, &deriv, _size, _sigma);
	setRowKernel( 2*_size* + 1, fvals, deriv);
	setColKernel( 2*_size* + 1, fvals, deriv);

	delete[] fvals;
}


/*!
\fn void  GaussianFilter::Reset(float Sig)
\brief Reset the filter kernel
*/
void  GaussianFilter::Reset(float Sig)
{
	Set(Sig);
}
