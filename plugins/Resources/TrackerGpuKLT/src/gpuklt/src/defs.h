
// --------------------------------------------------------------------------
/*! \file defs.h
* \brief Header File for definitions.
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

#ifndef _DEFS_H
#define _DEFS_H

/// MACROS
#define MAX_STR_LEN                 128
#define DEFAULT_WIN_WIDTH           640
#define DEFAULT_WIN_HEIGHT          480

#define MIN_OCTAVES                 1
#define MAX_OCTAVES                 6

#define MIN_INTERVALS               2
#define MAX_INTERVALS               4

#define  NUM_FILTER_LEVELS  (MAX_INTERVALS + 3)        // n, where filter Size = 4*n + 1,


#ifndef M_PI
#define M_PI                        3.14159265358979323846
#endif

#define NUM_ORIENTATION_DIRECTIONS  9     // The number of directions = 4 * 9 = 36, rgba = 4 channels.

#define MAX_DESCRIPTORS             2000

/*
#ifdef _BUILDING_GPU_VIS
    #define DLL_SPEC __declspec(dllexport)
#else
    #define DLL_SPEC __declspec(dllimport)
#endif
*/
#define DLL_SPEC

enum {GPU_NVIDIA, GPU_ATI};


#endif
