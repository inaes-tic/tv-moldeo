// --------------------------------------------------------------------------
/*! \file GpuVis_Image.cpp
* \brief GpuVis_Image Class File. 
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
#include <iostream>
#include <windows.h>

#include "..\include\GpuVis_Image.hpp"

/*!
 \fn unsigned char * GpuVis_Image::getData()
 \brief Returns buffer
 This returns a pointer to the image data buffer.
*/
unsigned char * GpuVis_Image::getData()
 {
	return m_imagedata;
 }


void GpuVis_Image::clean()
{
}

/*!
\fn  int GpuVis_Image::getWidth()
\brief Return Image Width
*/
 int GpuVis_Image::getWidth()
 {
	 return w;
 }


/*!
\fn  int GpuVis_Image::getHeight()
\brief Return Image Height
*/
 int GpuVis_Image::getHeight() 
 {
	return h;
 }
 
/*!
\fn  void  GpuVis_Image::allocateMemory(int w, int h, int p)
\brief Allocate memory to the buffer
*/
void  GpuVis_Image::allocateMemory(int w, int h, int p)
{
	m_imagedata = new unsigned char [ w * h * p];
	can_deallocate = true;
}

/*!
\fn  void  GpuVis_Image::deallocateMemory()
\brief Allocate memory to the buffer
*/
void  GpuVis_Image::deallocateMemory()
{
	if (m_imagedata && can_deallocate)
		delete m_imagedata;
}

void GpuVis_Image::setPointer(unsigned char * pt, int pw, int ph)
{
	m_imagedata = pt;
	w =	pw;
	h = ph;
	can_deallocate = false;
}
