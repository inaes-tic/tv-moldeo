
// --------------------------------------------------------------------------
/*! \file GaussianFilter.h
* \brief GaussianFilter Class Header File
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


#ifndef _GAUSSIAN_FILTER_H
#define _GAUSSIAN_FILTER_H
  
#include "SeparableFilter.h"

/*! \class GaussianFilter
    \brief Represents a filter that does Gaussian Filtering
    Encapsulates the Gaussian Filtering Operation using GPGPU with Cg Shaders
*/
class GaussianFilter:public SeparableFilter {
  
private:
    float _sigma;   //!< Std. Deviation sigma of zero mean, isotropic 1D Gaussian Distribution
	int   _sample;  //!< Multiple of 'sigma' before which discrete filter kernel is clamped to 0.
	int   _size;    //!< Width of 1D Filter Kernel.

	//! Compute the Gaussian Kernel Footprint.
	void evaluateFunc(float **fvals, float **deriv, int& n, float sigma);
	
public:
	//! Constructor
	GaussianFilter (int arch, GLenum target, float Sig, CGcontext& context, CGprofile& vProfile , CGprofile& fProfile);
	
	//! Destructor
	~GaussianFilter() {};

	//! Set the value of Sigma
	void  Set      (float Sig);

	//! Reset the value of Sigma and recompute kernel footprint.
	void  Reset    (float Sig);

	//! Return Size of Gaussian Kernel
	int   getSize() { return _size; }

	//! Return Sigma of Gaussian Kernel
	float getSigma(){ return _sigma;}

};



#endif
