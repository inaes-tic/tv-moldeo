
// --------------------------------------------------------------------------
/*! \file ImgFilter.h
* \brief ImgFilter Class Header File
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


#ifndef _IMG_FILTER_H
#define _IMG_FILTER_H


#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

  
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <math.h>

//#include "Kernel.h"

/*! \class ImgFilter
    \brief Represents a Local Filter Operation on an Image Stream in GPGPU framework
	
	This object represents a computation involving applying a filter to an image using GPGPU
	framework. 
	
	Invoking Computation and Specifying Domain,Range to the filter function is similar to
	that for the Kernel object.

*/
class ImgFilter {
  
protected: 
    int                  width,  height;  //!< Kernel width and height;
	int                  hwidth, hheight; //!< half of kernel, =n for 2n+1 X 2n+1 kernel
	float              **kernel;          //!< Kernel footprint.
	int                  ranW, ranH;	  //!< (0 -> ranW, 0 -> ranH) Function Range
  
public:
	//! Constructor
	ImgFilter() {};
	//! Constructor
	ImgFilter ( int w, int h)    {  width  = w;  height= h; }
	//! Destructor
	~ImgFilter() {};
	//! Set Kernel Width 
	void setKernelWidth (int w)  {  width  = w;	 hwidth  = (int)floor((float)w/2); }
	//! Set Kernel Height
	void setKernelHeight(int h)  {  height = h;	 hheight = (int)floor((float)h/2); }
	//! Set the range of the function represented by filter in terms of output pixel coordinates.
	void setRange (int ranWidth, int ranHeight) { ranW = ranWidth; ranH = ranHeight;}
	//! Return the filter width
	int getWidth(void); 
	//! Return the filter height
	int getHeight(void);
	//! Activate the shader program making up the filter
	void activate();
	//! Deactivate the shader program making up the filter
	void deactivate();
	//! Specify the input texture for the shader (input image to the filter)
	void setInput(GLuint texID);
	//! Invoke the filter by rendering a quad
	void execute(int    top, int    left, int    bot, int    right);
	//! Invoke the filter by rendering a quad, pass in the output box extent and input box texcoords
	void execute(float vtop, float vleft, float vbot, float vright,	
			     int    top, int    left, int    bot, int    right);
};

#endif
