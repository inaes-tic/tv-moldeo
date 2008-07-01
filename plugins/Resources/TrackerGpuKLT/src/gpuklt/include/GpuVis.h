
// --------------------------------------------------------------------------
/*! \file GpuVis.h
* \brief GpuVis Class Header File
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


#ifndef _GPU_VIS_H
#define _GPU_VIS_H


#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <vector>

#include <windows.h>

/// Cg ///
#include <Cg/cgGL.h>

#pragma warning( disable: 4251 )


#include "framebufferObject.h"
#include "glErrorUtil.h"
#include "defs.h"
#include "stopwatch.hpp"

#include "utils.h"
#include "VertexProgram.h"
#include "FragmentProgram.h"
#include "Kernel.h"
#include "DataManager.h"
#include "GaussianFilter.h"
#include "GpuVis_Options.h"
#include "GpuVis_Image.hpp"


#define SWAP(x,y) { int tmp = x; x = y; y = tmp; }

/*! \class Point2D
    \brief 2D point class
*/
class Point2D
{
public:
	float x,y;
	Point2D(float X,float Y) { x = X; y = Y; }
	~Point2D() { }
};

/*! \class GpuKLT_Feature
    \brief Object represents a single KLT Feature
   This Object stores information associated with a KLT Feature
*/
class DLL_SPEC  GpuKLT_Feature
{

public:
	float					x,y;					 //!< Location
	float					normx, normy;            //!< Normalized Feature Coordinates [ 0 - 1 ]
	float					tr_x, tr_y;              //!< Feature position in the previous frame.
	bool				    valid;					 //!< True for a valid feature point.
	std::vector<Point2D*>   track;					 //!< list of feature positions in the past frames. Forms the feature tracks in video.

	//! Constructor
	GpuKLT_Feature();

	//! Destructor
	~GpuKLT_Feature();

	//! Print Feature Details
	void print();

	//! Print Feature Tracks
	void printTracks();

	//! Update Feature Positions
	int updatePos(float kltConvergeThreshold, float kltSSDthresh, int kltborder, float delta, float res, float d1, float d2, float w, float h);

};



/*! \class GpuKLT_FeatureList
    \brief Object represents a List of KLT Features
   This Object stores information regarding a collection of Features.
   For eg. all the GpuKLT Features found in an image.
*/
class DLL_SPEC  GpuKLT_FeatureList
{
public:

	int					_nFeats;			//!< Maximum number of features that will be stored, tracked
	int					_ftCount;			//!< Current number of valid features.
	GpuKLT_Feature	  **_list;				//!< The feature list.

	//! Constructor - allocates memory for \a n features
	GpuKLT_FeatureList(int n);

	//! Destructor
	~GpuKLT_FeatureList()
	{
		releaseMemory();
	}

	//! Print Summary of what Feature List contains.
	void printSummary();

	//! Print Feature Tracks.
	void printTracks();

	//! Release Memory Allocated for Feature Lists.
	void releaseMemory();

	//! Draw Points
	void drawPoints(GLenum texTarget, int imgw, int imgh, int winW, int winH);

	//! Draw Tracks
	void drawTracks(GLenum texTarget, int imgw, int imgh, int winW, int winH);

};



/*! \class GpuVis
    \brief Object performs different operations on the GPU.
    It requires a single initialization call, application
    needs to interface with this object by passing in image objects
    This class extracts KLT features and returns them in KLT feature lists.
*/
class DLL_SPEC GpuVis
{

private:

  char              CGcodeDirectory[64];   //!< Folder where all CG shaders are located.

  bool		        verbose;	   		   //!< Flag indicates verbosity of output.
  bool    		    subTiming;	  		   //!< Flag inidicates if substeps will be timed.
  bool				enableDebug;

  int		        nFrames, nLevels;      //!< nFrames = 2 for 2D Tracking, nLevels = pyramid levels.
  int               kltwin;                //!< KLT window size (in pixels) (default: 7)
  int				kltsearchrange;        //!< KLT feature search range.
  int				kltsubsampling;        //!< KLT subsampling
  int				kltpyrlevels;          //!< KLT uses so many pyramid levels at the particular kltsubsampling.
  int               kltborder;             //!< KLT border      (in pixels) (in original window)
  int               kltnumfeats;           //!< KLT Number of Features.
  float             klteigenthreshold;     //!< KLT Min Eigen Value Threshold.
  int               kltmindist;            //!< KLT Minimum Distance between KLT Corners.
  int			    kltiterations;         //!< KLT Num Iterations.
  float             kltSSDThreshold;       //!< KLT SSD Threshold
  float				kltConvergeThreshold;  //!< KLT min distance convergence threshold
  int			    architecture;          //!< ATI or NVIDIA
  GLenum			_texTarget;

  CGparameter	    passChan_Param;		   //!< Fragment Shader Parameter : to display different channels in texture.
  CGparameter	    xoff_Param;		       //!< Fragment Shader Parameter : to floatingwindow_Kernel
  CGparameter	    yoff_Param;			   //!< Fragment Shader Parameter : to floatingwindow_Kernel

  CGparameter	    xoff1_Param;		   //!< Fragment Shader Parameter : to mSolve_Kernel
  CGparameter	    yoff1_Param;		   //!< Fragment Shader Parameter : to mSolve_Kernel

  CGparameter	    xoff2_Param;		   //!< Fragment Shader Parameter : to mSolve_f_Kernel
  CGparameter	    yoff2_Param;		   //!< Fragment Shader Parameter : to mSolve_f_Kernel

  CGparameter	    xoff3_Param;		   //!< Fragment Shader Parameter : to w1_Kernel
  CGparameter	    yoff3_Param;		   //!< Fragment Shader Parameter : to w1_Kernel

  CGparameter	    xoff4_Param;		   //!< Fragment Shader Parameter : to w2_Kernel
  CGparameter	    yoff4_Param;		   //!< Fragment Shader Parameter : to w2_Kernel


  CGparameter		eigenThresh_Param;     //!< Fragment Shader parameter : for minimum eigen value threshold.

  Kernel		   *passThru_Kernel;			   //!< Fragment Shader : pass Thru Shader
  Kernel		   *cornerness_pass1_Kernel; 	   //!< Fragment Shader : Used to Compute Cornerness Pass 1
  Kernel           *cornerness_earlyZ_Kernel;      //!< Fragment Shader : Used for Early Z-Culling - this sets the mask
  Kernel           *cornerness_pass2_Kernel;       //!< Fragment Shader : Used to Compute Cornerness Pass 2
  Kernel           *cornerness_clearZ_Kernel;      //!< Fragment Shader : Clear the depthTexture used for Early Z Culling

  Kernel		   *passChan_Kernel;			   //!< Fragment Shader : Used for displaying a particular channel
  Kernel		   *undistort_Kernel;			   //!< Fragment Shader : Rgb 2 Grey Conversion
  Kernel		   *floatingWindow_Kernel;		   //!< Fragment Shader : KLT_Track Step 1
  Kernel		   *w1_Kernel;					   //!< Fragment Shader : KLT_Track Step 1
  Kernel		   *w2_Kernel;					   //!< Fragment Shader : KLT_Track Step 1
  Kernel		   *pSum1_Kernel;		   //!< Fragment Shader : KLT_Track Step 2
  Kernel		   *pSum2_Kernel;		   //!< Fragment Shader : KLT_Track Step 2
  Kernel		   *mElem1_Kernel;		   //!< Fragment Shader : KLT_Track Step 3
  Kernel		   *mElem2_Kernel;		   //!< Fragment Shader : KLT_Track Step 3
  Kernel		   *mElem3_Kernel;		   //!< Fragment Shader : KLT_Track Step 3
  Kernel		   *mSolve_Kernel;		   //!< Fragment Shader : KLT_Track Step 4
  Kernel           *mSolve_f_Kernel;       //!< KLT Track final step.
  Kernel		   *blue_Kernel;		   //!< Fragment Shader : Output blue fragments
  Kernel		   *red_Kernel;			   //!< Fragment Shader : Output red fragments
  Kernel		   *white_Kernel;		   //!< Fragment Shader : Output white fragments

  // BILIN_TEST
  Kernel           *bilin;

  GaussianFilter   *g1;					   //!< Gaussian Filter Object
  GaussianFilter   *g2;					   //!< Gaussian Filter Object

  CGcontext		    cgContext;			   //!< Recovered CG Context
  CGprofile	        vertexProfile;		   //!< Recovered Vertex Shader Profile
  CGprofile		    fragmentProfile;	   //!< Recovered Fragment Shader Profile

  //TimerClass	   *timer;				   //!< Timer Object


  Stopwatch		   *timer;

  char			   *timerString;

  GpuVis_Image	   *image;		   	       //!< Input Image

  DataManager	   *dm; 				   //!< Texture Manager Object

  std::vector<GpuKLT_Feature*>   ftList1;  //!< Feature List.

  //! Radial Undistortion Step
  void			   undistort	  		     (bool flagClear);

  //! Build Pyramid (Image Intensity, Gradients)
  void			   buildPyramid			     ();

  //! Compute Min Eigen Values (Cornerness Function)
  void			   computeMinEigenValues     (GpuKLT_FeatureList **ftList, int mindist, float klteigenthreshold);

  //! Readback the Cornerness Values
  int			   readBackCornerNess        (int boxleft, int boxtop, int boxwidth, int boxheight);

  //! Clear the depthTexture in the cornerness FBO
  void			   clearCornerness();

  // Glut Initialization
  void  initGlut				 (int w, int h);

  // Glew Initialization
  void initGlew					 ();

public:
	float				 		  proctime, avgtime;	//!< per frame processing time.
	GLint                        _currentDrawBuffer;    //!< This variable is used by the display code
	float						* cornerNessBuffer;     //!< Float buffer used for reading back cornerness.
	int							  pointcount;			//!< Used during select / re-select features.
	int						    * pointlist;			//!< Used during select / re-select features.
	unsigned char				* featuremap;	/* Boolean array recording proximity of features */

	unsigned char               * buffers[8];

	//! Constructor
	GpuVis						 (GpuVis_Options& parameters);

	//! Destructor
	~GpuVis						 ();

	//! This routine initializes the GpuVIS Object during startup.
	void Init					 (GpuVis_Image& image);

	//! This routine computes the Image Pyramid on the GPU.
	void computePyramid			 (GpuVis_Image& image);

	//! This routine reads back the image pyramid (image intensity only)
	void readBackPyramid		 (unsigned char **charbuf);

	//! This routine selects good KLT Features to track from first frame.
	GpuKLT_FeatureList *         selectGoodFeatures	();

	//! This routine reselects good KLT Features to make up for features lost during tracking.
	int reselectGoodFeatures    (GpuKLT_FeatureList **list, int dir, float percentage);

	//! This routine uploads the current feature lists to GPU memory
	void uploadFeaturesToGPU(GpuKLT_FeatureList **ftList);

	//! This routine (does non-maximal suppression using sort) enforces the minimum distance between KLT features (On the CPU)
	void enforceMinDist(int *pointlist, int npoints,
									GpuKLT_FeatureList ** ftlist, int mindist, bool overwriteAll);

	//! This routine does the KLT tracking on the GPU.
	void trackFeatures			 (GpuKLT_FeatureList **ftList);

	//! KLT Track Debugging Routine (REMOVE LATER)
	void debugTrack(GpuKLT_FeatureList **ftList);

	//! Display Routines : show whole or parts of different texture units.
	void displayVideoMemory		 (int chan, int winW, int winH);
	//! Display Routines : show whole or parts of different texture units.
	void displayCornerness		 (int chan);
	//! Display Routines : show whole or parts of different texture units.
	void displayOrigFrame		 (int chan);
	//! Display Routines : show whole or parts of different texture units.
	void displayDistortionMap	 (int chan);
	//! Display Routines : show whole or parts of different texture units.
	void displayFeatureTable     (int chan);
	//! Display Routines : show whole or parts of different texture units.
	void displayFloatingWindow   (int chan);
	//! Display Routines : show whole or parts of different texture units.
	void displayPartialSum		 (int chan);
	//! Display Routines : show whole or parts of different texture units.
	void displayMatrixElem       (int chan);
	//! Display Routines : show whole or parts of different texture units.
	void displayPyramidBlock	 (int aux, int level, int chan);
	//! Display Routines : show whole or parts of different texture units.
	void displayUndistortedFrame (int chan, int winw, int winh, int maxcount, int trcount, int ftcount, int adcount);
	//! Display Routines : show whole or parts of different texture units.
	void displayFtList			 (GpuKLT_FeatureList *list, int imgW, int imgH, int winW, int winH);
	//! Display Routines : show whole or parts of different texture units.
	void drawDebugPatches		 (int ft_num, int fx, int fy, int iteration, int frame, GLuint attachment );
	//! Display Routines : show whole or parts of different texture units.
	void displayDebugTable		 ();
	//! Accessor Function
	int  getNLevels              () { return nLevels; }
};


//! CG Error Callback Routine
extern "C"
{
  void  cgErrorCallback();
};

#endif

