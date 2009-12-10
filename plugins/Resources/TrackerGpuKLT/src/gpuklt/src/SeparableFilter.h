
// --------------------------------------------------------------------------
/*! \file SeparableFilter.h
* \brief SeparableFilter Class Header File
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


#ifndef _SEPARABLE_FILTER_H
#define _SEPARABLE_FILTER_H

#include "defs.h"
#include "ImgFilter.h"
#include "FragmentProgram.h"


int getLevel(float Sig, float Extent=2.5);


/*! \class SeparableFilter
    \brief Represents Separable Image Filters
	
	This object represents a computation involving applying a separable Image Filter
	which can be implemented by apply 1D row filter followed by 1D column filter instead	
	of the 2D filter which is less efficient.

	Object Interface is similar to ImgFilter and Kernel Objects.

*/
class SeparableFilter:public ImgFilter {
  
protected:
	int				   arch;
	float			   sigma;
	int				   num_kernel_levels;		   	    //!< How many N x N kernels will be used for SSS construction
    int				   kernel_levels[2*MAX_INTERVALS];	//!< Max different kernel footprints.
	
    CGcontext		   cgContext;						//!< Recovered CG Context
    CGprofile	       vertexProfile;					//!< Recovered Vertex Shader Profile
    CGprofile		   fragmentProfile;					//!< Recovered Fragment Shader Profile

	GLuint  texInputID;    //!< Texture Image ID
	bool   swapNeeded;     //!< Some Swapping of Img Coordinates : where is (0,0)?
	float *row_kernel;     //!< Row Kernel    footprint
	float *col_kernel;     //!< Column Kernel footpr
	
	float *row_deriv;      //!< Derivative    
	float *col_deriv;      //!< Derivative

	FragmentProgram * row_shader[NUM_FILTER_LEVELS];      //!< Set of row shaders with different footprints (for diff. scales)
	FragmentProgram * col_shader[NUM_FILTER_LEVELS];	  //!< Set of column shaders with diff. footprints (for diff. scales)

	GLenum texTarget;

public:
	//! Constructor
	SeparableFilter(GLenum target);

	//! Constructor
	SeparableFilter(int arch, GLenum target, float Sig, CGcontext& context, CGprofile& vProfile , CGprofile& fProfile);

	//!Pre-compute the level of Kernels footprints that will be needed
	void computeKernelLevels  ();

	//! Create the Convolution Kernels.
	void setRowKernel(int w, float *rowKernelBuffer, float *derivBuffer);

	//! Create the Convolution Kernels.
	void setColKernel(int c, float *colKernelBuffer, float *derivBuffer); 

	//! Generate Fragment Programs Procedurally.
	//void createAndLoadColShaders(); 
	void createAndLoadColShaders_ATI(); 
	void createAndLoadColShaders_NVIDIA();
	
	//! generate Fragment Programs Procedurally
	//void createAndLoadRowShaders();
	void createAndLoadRowShaders_ATI();
	void createAndLoadRowShaders_NVIDIA();

	//! Perform 1D Column Convolution : Input Texture Quad, Output Render Quad
	void convolveRow(float Op_top, float Op_left, float Op_bot, float Op_right, 
					 float Ip_top, float Ip_left, float Ip_bot, float Ip_right, float offset);

	//! Perform 1D Column Convolution : Input Texture Quad, Output Render Quad
	void convolveCol(float Op_top, float Op_left, float Op_bot, float Op_right, 
					 float Ip_top, float Ip_left, float Ip_bot, float Ip_right, float offset);

	//! Enable this Kernel Processor
	void activate  ();
	
	//! Disable this Kernel Processor
	void deactivate();

	//! Bind The Texture 'texID' as Input Stream
	void bindInput (GLuint texID);
	
	//! Return the textureID of currently bound input.
	GLuint getTexID() { return texInputID; };

};

#endif
