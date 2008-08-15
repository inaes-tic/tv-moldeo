
// --------------------------------------------------------------------------
/*! \file GpuVis_Image.h
* \brief GpuVis_Image Class Header File
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


#ifndef _GPU_VIS_IMAGE_H
#define _GPU_VIS_IMAGE_H


#pragma warning( disable: 4251 )

#include <windows.h>

#include "defs.h"
#include <stdio.h>
#include <iostream>

/*! \class GpuVIS_Image
    \brief Object represents the Image data structure recognized by the GPU VIS library.
	
   This Object stores Image Data which is in Main Memory and is used to transfer Image Data
   from the user application to the graphics card memory for subsequent GPGPU computations.
*/
class DLL_SPEC  GpuVis_Image
{ 
 int									w;       //!< widht
 int									h;       //!< height
 unsigned char							 * m_imagedata;
 unsigned int								   m_id;
 bool can_deallocate;

public: 

	 //! Constructor
	 GpuVis_Image()
	 { m_id = -1; 
	   can_deallocate = false;
	 };

	 //! Destructor
	 ~GpuVis_Image() 
	 { };

	 //! Release image memory
	 void clean();
	 
	 //! Get ptr to data buffer
	 unsigned char * getData();

	 //! Get width
     int             getWidth();

	 //! Get height
     int             getHeight();

	 //! Free Memory
	 void			 deallocateMemory();
	 
	 //! Allocate Memory
	 void			 allocateMemory(int w, int h, int p);
	 
     void setPointer(unsigned char * pt, int pw, int ph);
};

#endif

