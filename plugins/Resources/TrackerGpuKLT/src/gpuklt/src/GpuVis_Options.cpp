// --------------------------------------------------------------------------
/*! \file GpuVis_Options.cpp
* \brief GpuVis_Options Class File. 
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

#include <string.h>
#include "..\include\GpuVis_Options.h"

/*!
\fn void GpuVis_Options::setCGShadersDirPath(char *directory)
\brief Sets Directory Path to Folder Containing CG Shaders
*/
void GpuVis_Options::setCGShadersDirPath(char *directory)
{
	strcpy(_CGdirectory,directory);
}

/*!
* \fn void GpuVis_Options::setMiscOptions(bool enableTiming, bool verbose, int windowWidth, int windowHeight, int nFrames, int nLevels, int search_range)
* \brief Set Miscellaneous Parameters for Vision Algorithms, KLT Tracking.
* \param enableTiming : Flag to enable timer.
* \param verbose      : Flag to control console msgs.
* \param windowWidth  : Win dimension
* \param windowHeight : Win dimension
* \param nFrames      : Num Video Frames to be kept in GPU Memory at any instance.
* \param nLevels      : Num Pyramid Levels in Main Pyramid (Image Intensity, Gradients)
* \param search_range : KLT Feature Search Range in pixels (eg. + / - 50 pixels )
*/
void GpuVis_Options::setMiscOptions(bool enableTiming, bool verbose, int nFrames, int nLevels, int search_range, int arch, bool debug)
{	
	_nFrames          = nFrames;
	_nLevels          = nLevels;
	_enableTiming     = enableTiming;
	_verbose          = verbose;
	_klt_search_range = search_range;
	_arch             = arch;
	_enableDebug      = debug;
}



