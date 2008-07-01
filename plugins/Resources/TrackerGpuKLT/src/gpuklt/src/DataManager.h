
// --------------------------------------------------------------------------
/*! \file DataManager.h
* \brief DataManager Class Header File.
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


#ifndef _TEXTURE_MANAGER_H
#define _TEXTURE_MANAGER_H

/// Glew ///
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif


#include <GL/glut.h>

#include "..\include\GpuVis_Options.h"
#include "..\include\GpuVis_Image.hpp"
#include "framebufferObject.h"
#include "renderBuffer.h"
#include "Kernel.h"


#ifndef _MAX_TEXTURE_WIDTH
#define _MAX_TEXTURE_WIDTH 4096
#endif

#ifndef _MAX_TEXTURE_HEIGHT
#define _MAX_TEXTURE_HEIGHT 4096
#endif

#ifndef _MAX_VIDEO_FRAMES
#define _MAX_VIDEO_FRAMES  12
#endif

#ifndef _MAX_PYRAMID_LEVELS
#define _MAX_PYRAMID_LEVELS  6
#endif

#ifndef _TEXTURE_BORDER_WIDTH
#define _TEXTURE_BORDER_WIDTH 64
#endif

#ifndef _MAIN_BUF
#define _MAIN_BUF 0
#endif

#ifndef _AUXI_BUF
#define _AUXI_BUF 1
#endif


/*********************************************************************************/
/*! \class TextureUnit
*   \brief Texture Unit stores an OpenGL texture and its related attributes. This is a single unit of storage in GPU memory.
*/
class TextureUnit {
public:
	int		_w;
	int		_h;
	GLuint  _texID;
	GLenum  _texTarget;
	GLenum  _texInternalFormat;
	GLenum  _texFormat;
	GLenum  _texDataType;
	GLenum  _texFilterType;

	TextureUnit (int w, int h, GLenum texTarget, GLenum internalFormat, GLenum texFormat, GLenum datatype, GLenum filterType);
	~TextureUnit();
	void resetTextureUCHAR(unsigned char *data);
	void resetTextureFLOAT(float *data);
};



/*********************************************************************************/
/*! \class FrameUnit 
*   \brief Frame Unit stores a few texture blocks that make up the Pyramid for a 
	single video frame. It contains 2 blocks for each pyramid level - main and auxi Buffers.
*/
class FrameUnit {
public:
	int    _nlev;
	TextureUnit * mainBuf[_MAX_PYRAMID_LEVELS];
	TextureUnit * auxiBuf[_MAX_PYRAMID_LEVELS];
	FrameUnit (GLenum arch, int w, int h, int n, GLenum texTarget, GLenum texInternalFormat);
	~FrameUnit ();
};


class GpuKLT_FeatureList;   // forward declaration


/*********************************************************************************/
/*! \class DataManager
*   \brief Manages Texture Memory on Graphics Card.
*
*  It manages 2D Texture units used for read / write using FBO's.
*  The Texture Manager 
*   (1) Allocates suitable memory
*   (2) Manages the collection of texture units for the various stages .. 
*/	
class DataManager {

private:
	int                     _image_w,_image_h;         ///< Orig Image Dimensions
	int                     _boundary;                 ///< Size of Padding.
	int					    _maxTexWidth;              ///< Max texture Width
	int					    _maxTexHeight;			   ///< Max texture Height
	int					    _nLevels;                  ///< The number of pyramid levels.
	int                     _nFrames;                  ///< data for how many frames ?
	int                     _maxFeatureTableSize;      ///< This is the maximum feature table size, this is the max number of features that KLT will try to track.
	int                     _nkltIterations;           ///< Number of time kltTrack is called on each feature at a particular Pyramid Level.
	int					    _currFrameIndex;
	int					    _prevFrameIndex;		   ///< Indices for current and prev frama (TOGGLE 0 1)

	int						_kltSubSampling;		   ///< Pixel Subsampling rate - could be 2, 4, 8..
	int						_kltPyramidLevels;         ///< Typically 1 or 2, this is smaller than the actual pyramid level (depends on kltSubsampling)

	int						_arch;
	GLenum					_texTarget;
	GLenum					_texInternalFormat;

	// The following are the storage units.
	// for (A) Undistortion, Build Pyramids
	TextureUnit *           _inputImageTU;			   ///< Input Image									  : Texture Unit.
	TextureUnit *           _distortionMapTU;          ///< Distortion Map								  : Texture Unit.
	FrameUnit   *           _pyramid[_MAX_VIDEO_FRAMES];///<  [Main buffer, aux buffer] for each videoframe : Group of Texture Units.
	
	// for (B) Computing Cornerness for Selecting Features
	TextureUnit *           _kltCornernessTU1;          ///< KLT Cornerness						        : Texture Unit 1
	TextureUnit *           _kltCornernessTU2;          ///< KLT Cornerness						        : Texture Unit 2

	// for (C) Tracking Features
	TextureUnit *           _kltFeaturesTU;            ///< KLT Feature Table						        : Texture Unit      
	TextureUnit *           _kltFloatingWindowTU;      ///< KLT Floating Window							    : Texture Unit       
	TextureUnit *           _kltPartialSumTU;          ///< KLT Partial Sum of 7x7 Patches.                 : Texture Unit
	TextureUnit *           _kltMatrixElemTU;          ///< KLT 2x2 Matrix Elements and 2x1 vector Elements : Texture Unit 

	
	float					_xOffset, _yOffset;        ///< Some precomputed values related to the texture dimensions in for (C)
	int						_xSize, _ySize;			   ///< Some precomputed values related to the texture dimensions.

public:

	GLuint						 depthTexture;             ///< Used for Early Z Culling with FBO's depth attachment.
	TextureUnit *			 debugTable;               ///< Texture Unit for debugging - REMOVE LATER ON !

	FramebufferObject	   *_fbos[_MAX_PYRAMID_LEVELS];///< Frame Buffer Objects (1 for each pyramid level i.e. image dimensions).
	//FramebufferObject       _fbo;					   ///< The 1st framebuffer object 
	FramebufferObject       _fbo1;					   ///< The 2nd framebuffer object
	FramebufferObject       _fbo2;					   ///< The 2nd framebuffer object
	Renderbuffer	        _rb;			 		   ///< Optional: The renderbuffer object used for depth
	float                  *data;                      ///< A pointer to a floating point buffer - used for uploaded data into Texture Memory.

	//! Constructor
	DataManager(int arch, int w, int h, int nframes, int nlevels, int kltsubsampling, int kltpyrlevels, int kltnumfeats, int kltiterations ) ;

	//! Destructor
	~DataManager() { };
	
	//! Initializes all the texture units in GPU.
	void                     initTextureUnits     (int w, int h);

	//! Resets the input texture unit (for video/image sequence processing
	void                     resetInputTexture    (unsigned char * image);

	//! Toggle between Current and Previous Frame [ FOR nFrames == 2 ONLY ]
	void                     toggleFrames         ();

	//! Upload the Feature list to GPU memory
	void					 uploadFeatureTable   (GpuKLT_FeatureList *list);

	//! Display The Whole Pyramid 
	void				     displayPyramids      (int winW, int winH, Kernel *kernel, Kernel *red, Kernel *blue);
	
	//! Display Pyramid Block
	void				     displayPyramidBlock  (int aux, int frame, int level, Kernel *kernel);

	//! Display Original Frame
	void		             displayOrigFrame     (Kernel *kernel);

	//! Display Distortion Map
	void                     displayDistortionMap (Kernel *kernel); 
	
	//! Display Cornerness Frame
	void                     displayCornerness    (Kernel *kernel, Kernel *colorkernel);

	//! Display Undistorted Frame
	void				     displayUndistortedFrame(Kernel *k1, Kernel *kernel, int winw, int winh, float time, int maxcount, int trcount, int ftcount, int ftadded);
	
	//! Display Feature Table
	void					 displayFeatureTable  (Kernel *kernel); 

	//! Display Floating Window
	void					 displayFloatingWindow(Kernel *kernel); 
	
	//! Display Partial Sum
	void					 displayPartialSum    (Kernel *kernel);
	
	//! Display Matrix Element
	void					 displayMatrixElem    (Kernel *kernel);


	//! Display Debug Table
	void					 displayDebugTable	  (Kernel *kernel);


	//! Accessor Function :
	TextureUnit *			 getPyramidBlock	  (int frameIndex, int level,int which_plane);

	//! Accessor Function : 
	TextureUnit *            getInputTextureUnit  ()				 { return _inputImageTU; }

	//! Accessor Function :
	TextureUnit *			 getDistortionMapTextureUnit ()			 { return _distortionMapTU; }

	//! Accessor Function :
	TextureUnit *            getFeaturesTextureUnit      ()			 { return _kltFeaturesTU; }

	//! Accessor Function :
	TextureUnit *            getFloatingWindowTextureUnit()			 { return _kltFloatingWindowTU; }

	//! Accessor Function :
	TextureUnit *            getPartialSumTextureUnit    ()			 { return _kltPartialSumTU; }

	//! Accessor Function : 
	TextureUnit *            getMatrixElemTextureUnit    ()			 { return _kltMatrixElemTU; }

	//! Accessor Function : 
	TextureUnit *            getCornerness1TextureUnit   ()			 { return  _kltCornernessTU1; }
	//! Accessor Function : 
	TextureUnit *            getCornerness2TextureUnit   ()			 { return  _kltCornernessTU2; }

	//! Accessor Function : Returns width of video frame
	int                      getImgWidth				 ()			 { return _image_w; }
	
	//! Accessor Function : Returns height of video frame
	int                      getImgHeight				 ()			 { return _image_h; }

	//! Accessor Function : Returns number of levels within octave.
	int				         getNumLevels				 ()			 { return _nLevels; }

	//! Accessor Function : Get current Frame Index
	int					     getCurrentFrameIndex		 ()			 { return _currFrameIndex; }
	
	//! Accessor Function : Get num KLT Iterations
	int						 getNumKLTIterations		 ()          { return _nkltIterations; }

	//! Accessor Function : Get max feature table size
	int						 getFeatureTableSize		 ()          { return _maxFeatureTableSize; }

	
	//! Accessor Function : Get Texture Unit dimension
	int						 getXSize					 ()		     { return _xSize; }

	//! Accessor Function : Get Texture Unit dimension
	int						 getYSize					 ()			 { return _ySize; }

	//! Accessor Function : Get Texture Unit dimension reciprocal
	float					 getXOffset					 ()		     { return _xOffset; }

	//! Accessor Function : Get Texture Unit dimension reciprocal
	float					 getYOffset					 ()			 { return _yOffset; }

};


#endif
