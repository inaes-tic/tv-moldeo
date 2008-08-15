
// --------------------------------------------------------------------------
/*! \file GpuVis_Options.h
* \brief GpuVis_Options Class Header File
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




#ifndef _GPU_VIS_OPTIONS_H
#define _GPU_VIS_OPTIONS_H

#include "defs.h"

#pragma warning( disable: 4251 )


/*! \class GpuVis_Options
    \brief Object contains options and global setting for the GPU VIS computation.

    Created during startup and used for initializing the GPU VIS Computor.
*/
class DLL_SPEC  GpuVis_Options
{  

public:

  char       _CGdirectory[64];              //!< Folder where all CG shaders are located.
  bool       _enableTiming;					//!< Flag indicates if substeps will be timed. 
  bool       _verbose;						//!< Flag indicates verbosity of output.
  int	     _nFrames, _nLevels;            //!< Store info for so many video frames in GPU memory. num levels in pyramid.
  int        _klt_search_range;             //!< Pixel range in which to search for features while tracking.
  int		 _kltwin;                       //!< KLT Window Size
  int        _kltborder;                    //!< Window Border Size to be avoided.
  int        _kltnumfeats;                  //!< Number of features.
  float      _klteigenthreshold;            //!< Eigen Value threshold for Cornerness.
  int        _kltmindist;                   //!< Minimum Distance between KLT features.
  int        _kltiterations;                //!< Number of track iterations within the same klt_pyramid level.
  float      _kltSSDthresh;                 //!< KLT SSD Threshold
  float		 _kltConvthresh;                //!< KLT Min distance convergence threshold
  int		 _arch;                         //!< ATI or NVIDIA
  bool		 _enableDebug;

	//! Constructor
	GpuVis_Options() {
		// Default Settings
		_enableTiming        = false;
		_verbose             = false;

	};

	//! Destructor
	~GpuVis_Options() {};
	
	//! Set Shader Path
	void setCGShadersDirPath(char *directory);

	//! Set other Parameters.
	void setMiscOptions(bool subTiming, bool verbose, int nFrames, int nLevels, int search_range, int arch, bool debug);
};


#endif
