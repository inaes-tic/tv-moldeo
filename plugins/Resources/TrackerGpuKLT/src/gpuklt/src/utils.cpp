// --------------------------------------------------------------------------
/*! \file utils.cpp
* \brief utils functions.
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
#include <sstream>
#include <string.h>
#include <windows.h>
#include "utils.h"
#include "..\include\GpuVis.h"

#include <GL/glut.h>

/*! \fn void glDebug()
\brief Print OpenGL Errors
*/
void glDebug()
{
	GLenum err = glGetError();
	printf("%s\n",gluErrorString(err));
}

/*! \fn std::string int2string(const int& number)
\brief Convert integer to string
*/
std::string int2string(const int& number) {
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

/*! \fn std::string float2string(const float& number)
\brief Convert floating pt number to string
*/
std::string float2string(const float& number) {
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

/*! \fn double rint( double x)
\brief Round off double to Integer : Substitute for math.h 's round() function
*/
double rint2( double x)
// Copyright (C) 2001 Tor M. Aamodt, University of Toronto
// Permisssion to use for all purposes commercial and otherwise granted.
// THIS MATERIAL IS PROVIDED "AS IS" WITHOUT WARRANTY, OR ANY CONDITION OR
// OTHER TERM OF ANY KIND INCLUDING, WITHOUT LIMITATION, ANY WARRANTY
// OF MERCHANTABILITY, SATISFACTORY QUALITY, OR FITNESS FOR A PARTICULAR
// PURPOSE.
{
    if( x > 0 ) {
        __int64 xint = (__int64) (x+0.5);
        if( xint % 2 ) {
            // then we might have an even number...
            double diff = x - (double)xint;
            if( diff == -0.5 )
                return double(xint-1);
        }
        return double(xint);
    } else {
        __int64 xint = (__int64) (x-0.5);
        if( xint % 2 ) {
            // then we might have an even number...
            double diff = x - (double)xint;
            if( diff == 0.5 )
                return double(xint+1);
        }
        return double(xint);
    }
}

/*! \fn void gpgpu_setup(int w,int h)
\brief Setup OpenGL Viewport, Projection and Modelview matrices.
*/
void gpgpu_setup(int w,int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*! \fn void reshape(int w, int h)
\brief GLUT Reshape callback function
*/
void reshape(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    // Trivial 1-1 Mapping between Pixel and Texel
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(-1,1,-1,1,0,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


/*! \fn bool querySupportedTextureSize(GLenum format, GLenum type, int w, int h)
\brief Tests if a texture of 'type' in 'format' internal format and size (w,h) can
be created on Video Memory.
*/
bool querySupportedTextureSize(GLenum format, GLenum type, int w, int h)
{
	GLint  width = w;
	GLint height = h;

	glTexImage2D(GL_PROXY_TEXTURE_2D, 0, format, width, height, 0, format, type, NULL);

	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0,GL_TEXTURE_HEIGHT, &height);
	if (width==0 || height == 0)
		return false;   /* Can't use that texture */

	return true;
}


/*! \fn void DrawText(GLint x, GLint y, char* s, GLfloat r, GLfloat g, GLfloat b)
\brief Draws Text on the screen to report statistics, timings etc.
*/
void DrawText(GLint x, GLint y, char* s, GLfloat r, GLfloat g, GLfloat b)
{
    int lines;
    char* p;

    glMatrixMode(GL_PROJECTION);
     glPushMatrix();
     glLoadIdentity();
     glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH),
	    0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);
     glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glColor3f(r,g,b);
      glRasterPos2i(x, y);
      for(p = s, lines = 0; *p; p++) {
	  if (*p == '\n') {
	      lines++;
	      glRasterPos2i(x, y-(lines*18));
	  }
	  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
      }
      glPopMatrix();
     glMatrixMode(GL_PROJECTION);
     glPopMatrix();
     glMatrixMode(GL_MODELVIEW);
	 glFlush();
}


/*! \fn void initGlew()
\brief Init Glew library.
*/
void initGlew() {
	int err = glewInit();
    if (GLEW_OK != err)
	{
      fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
      exit(-1);
	}
}


/* Macro used by qsort */
#define SWAP3(list, i, j)               \
{register int *pi, *pj, tmp;            \
     pi=list+3*(i); pj=list+3*(j);      \
                                        \
     tmp=*pi;    \
     *pi++=*pj;  \
     *pj++=tmp;  \
                 \
     tmp=*pi;    \
     *pi++=*pj;  \
     *pj++=tmp;  \
                 \
     tmp=*pi;    \
     *pi=*pj;    \
     *pj=tmp;    \
}


/*! \fn void qsort(int *pointlist, int n)
\brief Qsort for an integer array
*/
void qsort(int *pointlist, int n)
{
  unsigned int i, j, ln, rn;

  while (n > 1)
  {
    SWAP3(pointlist, 0, n/2);
    for (i = 0, j = n; ; )
    {
      do
        --j;
      while (pointlist[3*j+2] < pointlist[2]);
      do
        ++i;
      while (i < j && pointlist[3*i+2] > pointlist[2]);
      if (i >= j)
        break;
      SWAP3(pointlist, i, j);
    }
    SWAP3(pointlist, j, 0);
    ln = j;
    rn = n - ++j;
    if (ln < rn)
    {
      qsort(pointlist, ln);
      pointlist += 3*j;
      n = rn;
    }
    else
    {
      qsort(pointlist + 3*j, rn);
      n = ln;
    }
  }
}
#undef SWAP3

/*
 * Algorithm from N. Wirth's book, implementation by N. Devillard.
 * This code in public domain.
 */

typedef float elem_type ;

#define ELEM_SWAP(a,b) { register elem_type t=(a);(a)=(b);(b)=t; }

/*---------------------------------------------------------------------------
   Function :   kth_smallest()
   In       :   array of elements, # of elements in the array, rank k
   Out      :   one element
   Job      :   find the kth smallest element in the array
   Notice   :   use the median() macro defined below to get the median.

                Reference:

                  Author: Wirth, Niklaus
                   Title: Algorithms + data structures = programs
               Publisher: Englewood Cliffs: Prentice-Hall, 1976
    Physical description: 366 p.
                  Series: Prentice-Hall Series in Automatic Computation

 ---------------------------------------------------------------------------*/

/*! \fn int kth_largest(int a[], int n, int k)
\brief return the kth largest number from a annray of n integers
*/
int kth_largest(int a[], int n, int k)
{
    register int i, j, l, m ;
    register elem_type x ;
	int      max = -9999999;

	if (k >= n)
	{
		if (a[3*k+2] > max)
			max = a[3*k+2];
		return max;
	}

    l=0 ; m=n-1 ;
    while (l<m) {
        x=a[3*k+2] ;
        i=l ;
        j=m ;
        do {
            while (a[3*i+2] > x) i++ ;
            while (x > a[3*j+2]) j-- ;
            if (i<=j) {
				ELEM_SWAP(a[3*i  ],a[3*j  ]);
				ELEM_SWAP(a[3*i+1],a[3*j+1]);
                ELEM_SWAP(a[3*i+2],a[3*j+2]) ;
                i++ ; j-- ;
            }
        } while (i<=j) ;
        if (j<k) l=i ;
        if (k<i) m=j ;
    }
    return a[3*k+2] ;
}


