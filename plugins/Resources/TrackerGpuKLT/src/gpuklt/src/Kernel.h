
// --------------------------------------------------------------------------
/*! \file Kernel.h
* \brief Kernel Class Header File
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


#ifndef _KERNEL_H
#define _KERNEL_H

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

  
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "FragmentProgram.h"

/*! \class Kernel
    \brief Represents a Kernel Processing Unit on an Image Stream in GPGPU framework
	
	This object represents a computation that is invoked locally over the image specified
	as texture. Computation is invoked by specifying Output Coordinates in the render target
	and also by specifying the input texture coordinates. Thus a Kernel represents a function
	that maps an input Quad Memory Block to an output Quad memory block.

	In simplest GPGPU scenario, the Kernel can be set up to do 1-1 Pixel to Texel Mapping.
	By Setting the Range of the Function we can apply the kernel to a subset of the range
	and also specify a subset of the function domain by appropriately choosing texture coordinates.
*/
class Kernel {
private:

	//! Render == compute using kernel
	//! (vtop,vleft,vbot,vright) : Quad Vertex Coordinates [-1,1]
	//! (top,left,bot,right)     : Texture Coordinates for input [0 - 1) , [0 - 1)
	void exec(float vtop, float vleft, float vbot, float vright, 
			  float  top, float  left, float  bot, float  right);

public:

	FragmentProgram  *fProgram;           //!< Fragment Program Object
	bool              swapNeeded;         //!< If True (0,0) = Top-left, else Bot-left
	int               ranW, ranH;         //!< Function Range (0 - ranW) X (0 - ranH )
	GLenum			 _texTarget;

	//! Constructor
	Kernel () 
	{ 
		fProgram = NULL; 
	};

	//! Constructor : Kernel Processor Object (abstraction of stream processor)
	Kernel(int arch, GLenum texTarget, CGcontext fContext, CGprofile fProfile , char * directory, char *FPname, bool programInFile);

	//! Set Function Range ( 0 - ranWidth ) and ( 0 - ranHeight )
	void setRange (int ranWidth, int ranHeight) { ranW = ranWidth; ranH = ranHeight;}

	//! Enable this Kernel Processo
	void activate();

	//! Disable this Kernel Processor
	void deactivate();

	//! Bind The Texture 'texID' as Input Stream
	void bindInput(GLuint texID);
	
	//! Render Output Quad Op(top,left,bot,right) within render target Quad (ranH,0,0,ranW)using Input Texture Ip( , , , ) 
	void execute(float Op_top, float Op_left,float Op_bot, float Op_right, 
			     float Ip_top, float Ip_left, float Ip_bot, float Ip_right);

};

#endif
