
// --------------------------------------------------------------------------
/*! \file utils.h
* \brief utils functions Header File
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



#ifndef _UTILS_H
#define _UTILS_H


#include "..\include\GpuVis.h"

void glDebug					(					  );
double rint2						( double x            );
std::string int2string			( const int& number   );
std::string float2string		( const float& number );
void reshape					( int w, int h        );
void gpgpu_setup			    (int w,int h);
bool querySupportedTextureSize  ( GLenum format, GLenum type, int w, int h);
void qsort                      ( int *pointlist, int n);
int  kth_largest                (int a[], int n, int k);
void DrawText					(GLint x, GLint y, char* s, GLfloat r, GLfloat g, GLfloat b);

#endif
