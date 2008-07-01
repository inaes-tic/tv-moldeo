
// --------------------------------------------------------------------------
/*! \file GpuVis.cpp
* \brief GpuVis Class File. 
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

#include <windows.h>

#include "..\include\GpuVIS.h"

#include <stdio.h>
#include <iostream>
#include <fstream>



static void fillFeaturemap(
  int x, int y, 
  unsigned char *featuremap, 
  int mindist, 
  int ncols, 
  int nrows)
{
  int ix, iy;

  for (iy = y - mindist ; iy <= y + mindist ; iy++)
    for (ix = x - mindist ; ix <= x + mindist ; ix++)
      if (ix >= 0 && ix < ncols && iy >= 0 && iy < nrows)
        featuremap[iy*ncols+ix] = 1;
}



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

void _quicksort(int *pointlist, int n)
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
      _quicksort(pointlist, ln);
      pointlist += 3*j;
      n = rn;
    }
    else
    {
      _quicksort(pointlist + 3*j, rn);
      n = ln;
    }
  }
}
#undef SWAP3


/*! \fn void GpuVis::enforceMinDist(int *pointlist, int npoints,                 
					 GpuKLT_FeatureList ** ftlist, int mindist, bool overwriteAll)
\brief Enforce the minimum distance between KLT features (On the CPU)
*/
void GpuVis::enforceMinDist(int *pointlist, int npoints,                 
					 GpuKLT_FeatureList ** ftlist, int mindist, bool overwriteAll)
{
  int            oldcounter, counter, off, indx;   
  int            x, y, val;     /* Location and trackability of pixel under consideration */
  int            ncols, nrows;
  int           *ptr;
  float          recip_X, recip_Y;
  Stopwatch		 emd_timer;

  // Sort the pointlist.
  if (subTiming) { emd_timer.Reset(); emd_timer.Start(); }

  _quicksort(pointlist, npoints);

  if (subTiming) {  
		emd_timer.Stop();
		printf("\n\t\t   [ Sort %d ints ]: %8.2lf msec\n\n", npoints, emd_timer.GetTime()*1000);
	
  }

  GpuKLT_FeatureList *featurelist = *ftlist;
  ncols   = dm->getImgWidth();
  nrows   = dm->getImgHeight();
  recip_X = 1.0 / ncols;
  recip_Y = 1.0 / nrows;

  /* Allocate memory for feature map and clear it */
  memset(featuremap, 0, ncols*nrows);
		
  /* Necessary because code below works with (mindist-1) */
  mindist--;
 
  featurelist->_ftCount = 0;

  /* If we are keeping all old good features, then add them to the featuremap */  
  if (!overwriteAll)
	{
		for (indx = 0 ; indx < featurelist->_nFeats ; indx++)
		{
			 if (featurelist->_list[indx]->valid)  
			 {
				x   = (int) featurelist->_list[indx]->x;
				y   = (int) featurelist->_list[indx]->y;
				featurelist->_ftCount++;			
				fillFeaturemap(x, y, featuremap, mindist, ncols, nrows);
			 } 
		 }
	}
	  
  bool done = false;
  oldcounter= counter = 1;
  indx      = 0;
  int k     = 0;
  ptr       = pointlist;
  // while we haven't found the required number of features.
  while (1) 
  {	

    /* If we can't add all the points, then fill in the rest
       of the featurelist with -1's */
    if (k>= npoints)  {
      while (indx < featurelist->_nFeats)  {	
        if (overwriteAll || !featurelist->_list[indx]->valid) {
           featurelist->_list[indx]->x     = -1;
		   featurelist->_list[indx]->y     = -1;
		   featurelist->_list[indx]->normx = -1;
		   featurelist->_list[indx]->normy = -1;
		   featurelist->_list[indx]->valid = false;
        }
        indx++;
      }
      break;
    }

	off = 3*k;
	x   = *(ptr + off);
	y   = *(ptr + off +1);
	val = *(ptr + off +2);

	assert(x >= 0);   assert(x < ncols);
	assert(y >= 0);   assert(y < nrows);

	while (!overwriteAll && 
           indx < featurelist->_nFeats &&
           featurelist->_list[indx]->valid)
      indx++;

    if (indx >= featurelist->_nFeats)  
		break;

	// If no neighbor has been selected, then add feature to the current list
	if (!featuremap[y*ncols+x])  
	{
	  featurelist->_list[indx]->x     = x;
	  featurelist->_list[indx]->y     = y;
	  featurelist->_list[indx]->normx = x * recip_X ;
	  featurelist->_list[indx]->normy = y * recip_Y ;
	  featurelist->_list[indx]->valid = true;
	  featurelist->_list[indx]->track.clear();
	  indx++;
	  //printf("Adding feature pt %03d\n",indx);

	  featurelist->_ftCount++;
	  // Fill in surrounding region of feature map.
	  fillFeaturemap(x, y, featuremap, mindist, ncols, nrows);
	}

	k++;

  } // while

}




/*!
\fn GpuKLT_Feature::GpuKLT_Feature()
\brief Constructor
*/
GpuKLT_Feature::GpuKLT_Feature()
{
	x = -1; y = -1; valid = false;
	track.clear();
}


GpuKLT_Feature::~GpuKLT_Feature()
{
}

/*!
\fn void GpuKLT_Feature::print();
\brief Display Feature Details
*/
void GpuKLT_Feature::print()
{
	printf("Pos:\t%10.5f,%10.5f\t(%7.2f,%7.2f)\n",normx,normy,x,y);
}

/*!
\fn void GpuKLT_Feature::printTracks()
\brief Display Feature Tracks
*/
void GpuKLT_Feature::printTracks()
{
	int	n = track.size();

	if (n==0)
		printf("(%7.2f,%7.2f)\n", x, y);
	else	
		printf("(%7.2f,%7.2f) -> (%7.2f,%7.2f)\n", track[n-1]->x, track[n-1]->y, x, y);
}



/*!
\fn int GpuKLT_Feature::updatePos(float kltConvergeThreshold, float kltSSDthresh, int kltborder, float delta, float res, float d1, float d2, float w, float h)
\brief Update the Feature Position (d1,d2) are the delta_x and delta_y resp.
*/
int GpuKLT_Feature::updatePos(float kltConvergeThreshold, float kltSSDthresh, int kltborder, float delta, float res, float d1, float d2, float w, float h)
{

	double delx, dely;
	bool  discardFlag;
	
	if (res > kltSSDthresh)
		discardFlag = true;
	else if (d1 < 0.0 && d2 < 0.0)
		discardFlag = true;
	else 
	{
		Point2D * pt = new Point2D(x,y);
		track.push_back(pt);

		delx         = normx - d1;
		dely         = normy - d2;
		normx        = normx - delx;
		normy		 = normy - dely;
		x			 = normx * w;
		y			 = normy * h;

		if ( (x < kltborder) || (x > (w - kltborder)) || (y < kltborder) || (y > (h - kltborder)) )
			discardFlag = true;
		else
			discardFlag = false;
	}

	if (discardFlag)
	{	
		
		//for (int j=0;j<track.size();j++)
		//	delete track[j];
		track.clear();
		valid = false;
		return -1;
	}
	else
		return 1;
}

	
/*!
\fn GpuKLT_FeatureList::GpuKLT_FeatureList(int n)
\brief Constructor allocates space for \a n features
*/
GpuKLT_FeatureList::GpuKLT_FeatureList(int n)
{
	_nFeats = n;

	_list   = new GpuKLT_Feature* [ _nFeats ];

	for (int k=0;k<_nFeats;k++)
		_list[k] = new GpuKLT_Feature();
}


/*!
\fn 	void GpuKLT_FeatureList::printSummary();
\brief  Display Details of Feature List
*/
void GpuKLT_FeatureList::printSummary()
{
	printf("printSummary OUTPUT\n");
	for (int i=0;i<_nFeats;i++)
		if (_list[i]->valid)
			_list[i]->print();
	printf("printSummary OUTPUT ftCount = %d\n",_ftCount);
}

/*!
\fn 	void GpuKLT_FeatureList::printTracks()
\brief  Display Details of Feature Tracks
*/
void GpuKLT_FeatureList::printTracks()
{
	printf("printTracks OUTPUT\n");
	for (int i=0;i<_nFeats;i++)
		if (_list[i]->valid)
			_list[i]->printTracks();
}


/*! 
\fn void GpuKLT_FeatureList::releaseMemory()
\brief This frees memory allocated for storing list of features.
*/
void GpuKLT_FeatureList::releaseMemory()
{
	for (int i=0;i<_nFeats;i++)
		delete _list[i];
	delete[] _list;
	_nFeats = 0;

	return;
}

/*! 
\fn void GpuKLT_FeatureList::drawTracks(int imgw, int imgh, int winW, int winH)
\brief This draws the current feature tracks.
*/
void GpuKLT_FeatureList::drawTracks(GLenum texTarget, int imgw, int imgh, int winW, int winH)
{


	glViewport(0, 0, winW, winH);
	glMatrixMode(GL_PROJECTION);    
	glLoadIdentity();               
	gluOrtho2D(0,imgw, 0, (int)(1.33333*imgh)); 
	glMatrixMode(GL_MODELVIEW);     
	glLoadIdentity();
    glLineWidth (1.0);
	glDisable(texTarget);

	glPointSize(2.0); 

	for (int i=0;i<_nFeats;i++)
	{		
		int k;
		if (_list[i]->valid && _list[i]->tr_x != -1.0 && _list[i]->tr_y != -1.0)
		{
			glBegin(GL_POINTS);
			glColor3f(1.0,0.0,0.0);
			for (k=0;k< _list[i]->track.size();k++)
				glVertex2f( _list[i]->track[k]->x, imgh - _list[i]->track[k]->y);
			glEnd();
			glBegin(GL_LINE_STRIP);
			for (k=0;k< _list[i]->track.size();k++)
				glVertex2f( _list[i]->track[k]->x, imgh - _list[i]->track[k]->y);
			glVertex2f( _list[i]->x , imgh - _list[i]->y  );
			glEnd();
			
		}
	}
	glPointSize(1.0);
	glLineWidth(1.0);

	return;

}


/*! 
\fn void GpuKLT_FeatureList::drawPoints(int imgw, int imgh, int winW, int winH)
\brief This draws the current feature points.
*/
void GpuKLT_FeatureList::drawPoints(GLenum texTarget, int imgw, int imgh, int winW, int winH)
{
	int i;
	glViewport(0, 0, winW, winH);
	glMatrixMode(GL_PROJECTION);    
	glLoadIdentity();               
	gluOrtho2D(0,imgw, 0, (int)(1.33333*imgh)); 
	glMatrixMode(GL_MODELVIEW);     
	glLoadIdentity();
	
	glDisable(texTarget);
	glPointSize(2.0); 
	
	glBegin(GL_POINTS);
	for (i=0;i<_nFeats;i++)
	{		
		if (_list[i]->valid)
		{
			glVertex2f( _list[i]->x , imgh - _list[i]->y  );
		}
	}
	glEnd();
	glLineWidth(2.0);
	for (i=0;i<_nFeats;i++)
	{		
		if (_list[i]->valid)
		{
			glBegin(GL_LINE_LOOP);
			glVertex2f( _list[i]->x -3.5, imgh - _list[i]->y-3.5  );
			glVertex2f( _list[i]->x +3.5, imgh - _list[i]->y-3.5  );
			glVertex2f( _list[i]->x +3.5, imgh - _list[i]->y+3.5  );						
			glVertex2f( _list[i]->x -3.5, imgh - _list[i]->y+3.5  );
			glEnd();
		}
	}

	glPointSize(1.0);
	glLineWidth(1.0);

	return;
}

/*!
\fn void initGlew()
\brief GLEW Initialization within GPUSIFT library
*/
void GpuVis::initGlew() {
	int err = glewInit();
    if (GLEW_OK != err)
	{
      fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
      exit(-1);
	}
}

/*!
\fn void cgErrorCallback()
\brief CG Error Callback Routine.
*/
extern "C"
{
	void cgErrorCallback()
	{
		CGerror lastError = cgGetError();
		if(lastError)
		{
			printf("%s\n\n", cgGetErrorString(lastError));
			//printf("%s\n", cgGetLastListing(cgContext));
			printf("Cg error, exiting...\n");    
			exit(0);
		}
	} 
}




/*! 
\fn GpuVis::GpuVis(GpuVis_Options& gopt)
\brief Constructor
*/
GpuVis::GpuVis(GpuVis_Options& gopt)
{

	float subsampling;
	int   pot;

    // Transfer State from GpuVis_Option to this object.

    strcpy(CGcodeDirectory, gopt._CGdirectory);

    verbose             = gopt._verbose;
    subTiming           = gopt._enableTiming;
    nFrames			    = gopt._nFrames;
    nLevels			    = gopt._nLevels;
    kltsearchrange	    = gopt._klt_search_range;
	kltwin              = gopt._kltwin;
 	kltborder           = gopt._kltborder;
	kltnumfeats         = gopt._kltnumfeats;
	klteigenthreshold   = gopt._klteigenthreshold;
	kltmindist          = gopt._kltmindist;
	kltiterations       = gopt._kltiterations;
	kltSSDThreshold     = gopt._kltSSDthresh;
	kltConvergeThreshold= gopt._kltConvthresh;
	architecture	    = gopt._arch;
	enableDebug			= gopt._enableDebug;

	switch (architecture)
	{
	case GPU_ATI:
		strcat(CGcodeDirectory,"\\ati");
		_texTarget = GL_TEXTURE_2D;
		break;
	case GPU_NVIDIA: 
		strcat (CGcodeDirectory,"\\nvidia");
		_texTarget = GL_TEXTURE_RECTANGLE_NV;
		break;
	default:
		printf("Architecture must be ati or nvidia\n");
		exit(1);
	}
	

	// Calculate the Klt sub sampling rate and the number of pyr levels it will use.
	
	subsampling = ((float) kltsearchrange) / (0.5*kltwin);

	if (subsampling < 1.0)  {		    /* 1.0 = 0+1 */
		kltpyrlevels	  = 1;
		pot               = 0;
	} else if (subsampling <= 3.0)  {	/* 3.0 = 2+1 */
		kltpyrlevels	  = 2;
		kltsubsampling    = 2;
		pot               = 1;
	} else if (subsampling <= 5.0)  {	/* 5.0 = 4+1 */
		kltpyrlevels	  = 2;
		kltsubsampling    = 4;
		pot               = 2;
	} else if (subsampling <= 9.0)  {	/* 9.0 = 8+1 */
		kltpyrlevels	  = 2;
		kltsubsampling    = 8;
		pot               = 3;
	} else {
		/* The following lines are derived from the formula:
		search_range = 
		window_halfwidth * \sum_{i=0}^{nPyramidLevels-1} 8^i,
		which is the same as:
		search_range = 
		window_halfwidth * (8^nPyramidLevels - 1)/(8 - 1).
		Then, the value is rounded up to the nearest integer. */
		float val      = (float) (log(7.0*subsampling+1.0)/log(8.0));
		kltpyrlevels   = (int) (val + 0.99);
		kltsubsampling = 8;
		pot            = 3 * (kltpyrlevels - 1);
		printf("\tGpuVis: WARNING: NOT TESTED NOT TESTED\n");
		exit(1);
	}
    
	// Check that klt has all the levels it needs.
	if (nLevels > (pot+1) )
		printf("\tGpuVis: subsample-2 gaussian pyramid has more levels than KLT needs\n");
	else
		printf("\tGpuVis: subsample-2 gaussian pyramid has the same levels as KLT needs\n");

   initGlew();
 
   printf("\tGpuVis: Glut/Glew was initialized...\n");

   //timer = new TimerClass();
	timer = new Stopwatch();
}

GpuVis::~GpuVis()
{
}

/*! 
\fn void GpuVis::Init(GpuVis_Image& inputImage);
\brief Initialize the GpuVis Object - call once at startup.
*/
void GpuVis::Init(GpuVis_Image& inputImage)
{

  // Setup Cg call back function.
  cgSetErrorCallback(cgErrorCallback);

  // Create cgContext.
  cgContext = cgCreateContext();

  // NOTE Sudipta 6th July 2006
  // On NVIDIA, with Cg-1.5beta2 we saw that
  // float32 textures weren't being properly processed
  // with fp40 and vp40 profiles.
  // Changing the profiles to arbfp1 and arbvp1 made it work
  // So with Nvidia Cg-1.4.1, fp40 and vp40 are ok.
  // OR with Nvidia Cg-1.5b2  arbfp1 and arbvp1 should be used.

  // Get the best Cg profile for this hardware
  /*
  vertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
  if (vertexProfile == CG_PROFILE_VP40)
	printf("\tGpuVis: Using NV VP40 Profile ...\n");
  else if (vertexProfile==CG_PROFILE_VP30)
	printf("\tGpuVis: Using NV VP30 Profile ...\n");
  else if (vertexProfile==CG_PROFILE_ARBVP1)
	printf("\tGpuVis: Using ARBVP1 Profile ...\n");
  assert(vertexProfile != CG_PROFILE_UNKNOWN);

  fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
  if (fragmentProfile == CG_PROFILE_FP40)
 	printf("\tGpuVis: Using NV FP40 Profile ...\n");
  else if (fragmentProfile==CG_PROFILE_FP30)
	printf("\tGpuVis: Using NV FP30 Profile ...\n");
  else if (fragmentProfile==CG_PROFILE_ARBFP1)
	printf("\tGpuVis: Using ARBFP1 Profile ...\n");
  assert(fragmentProfile != CG_PROFILE_UNKNOWN);
  */

  if (architecture == GPU_ATI) 
  {
	vertexProfile = CG_PROFILE_ARBVP1;
	fragmentProfile = CG_PROFILE_ARBFP1;
  }
  else if (architecture == GPU_NVIDIA)
  {
	vertexProfile = CG_PROFILE_VP40;
	fragmentProfile = CG_PROFILE_FP40;
  }
  else
  {
	vertexProfile = CG_PROFILE_UNKNOWN;
	fragmentProfile = CG_PROFILE_UNKNOWN;
  }

  assert(fragmentProfile != CG_PROFILE_UNKNOWN);
  assert(vertexProfile != CG_PROFILE_UNKNOWN);

  cgGLSetOptimalOptions(vertexProfile); 
  cgGLSetOptimalOptions(fragmentProfile);

  // Create Kernels.
  passThru_Kernel			 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "pass.cg"         ,   true);
  cornerness_pass1_Kernel    = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "corner1.cg"      ,   true);
  cornerness_earlyZ_Kernel   = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "corner2a.cg"     ,   true);
  cornerness_pass2_Kernel    = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "corner2b.cg"     ,   true);
  cornerness_clearZ_Kernel   = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "corner2c.cg"     ,   true);
  passChan_Kernel			 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "chan.cg"         ,   true);
  undistort_Kernel			 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "undistort.cg"    ,   true);
  blue_Kernel				 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "blue.cg"         ,   true);
  red_Kernel				 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "red.cg"          ,   true);
  white_Kernel				 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "white.cg"        ,   true);
  floatingWindow_Kernel		 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "floatingwin.cg"  ,   true);
 
  w1_Kernel					 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "w1.cg"			 ,   true);
  w2_Kernel					 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "w2.cg"			 ,   true);
 
  pSum1_Kernel				 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "partialsum1.cg"  ,   true);
  pSum2_Kernel				 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "partialsum2.cg"  ,   true);
  mElem1_Kernel				 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "matrixelem1.cg"  ,   true);
  mElem2_Kernel				 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "matrixelem2.cg"  ,   true);
  mElem3_Kernel				 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "matrixelem3.cg"  ,   true);
  mSolve_Kernel				 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "matrixsolve.cg",   true);
  mSolve_f_Kernel			 = new Kernel(architecture, _texTarget, cgContext, fragmentProfile, CGcodeDirectory, "matrixsolve_f.cg",   true);


  g1						 = new GaussianFilter	(architecture, _texTarget, 0.7, cgContext, vertexProfile, fragmentProfile); g1->Reset(0.7);
  g2					     = new GaussianFilter	(architecture, _texTarget, 1.4, cgContext, vertexProfile, fragmentProfile); g2->Reset(1.4); 
  
  passChan_Param			 = cgGetNamedParameter	(passChan_Kernel->fProgram->_FP      , "channel");
  eigenThresh_Param			 = cgGetNamedParameter	(cornerness_pass2_Kernel->fProgram->_FP, "eigen_thresh");
  
  xoff_Param			     = cgGetNamedParameter	(floatingWindow_Kernel->fProgram->_FP, "xoff");
  yoff_Param			     = cgGetNamedParameter	(floatingWindow_Kernel->fProgram->_FP, "yoff");
  
  xoff1_Param			     = cgGetNamedParameter	(mSolve_Kernel->fProgram->_FP	     , "xoff_1");
  yoff1_Param			     = cgGetNamedParameter	(mSolve_Kernel->fProgram->_FP		 , "yoff_1");

  xoff2_Param			     = cgGetNamedParameter	(mSolve_f_Kernel->fProgram->_FP	     , "xoff_2");
  yoff2_Param			     = cgGetNamedParameter	(mSolve_f_Kernel->fProgram->_FP		 , "yoff_2");

  xoff3_Param			     = cgGetNamedParameter	(w1_Kernel->fProgram->_FP	     , "xoff");
  yoff3_Param			     = cgGetNamedParameter	(w1_Kernel->fProgram->_FP		 , "yoff");

  xoff4_Param			     = cgGetNamedParameter	(w2_Kernel->fProgram->_FP	     , "xoff");
  yoff4_Param			     = cgGetNamedParameter	(w2_Kernel->fProgram->_FP		 , "yoff");


  printf("\tGpuVis: All Shaders loaded ...\n");

  // Initialize Data Manager, set up textures and FBO.
  dm      = new DataManager( architecture, inputImage.getWidth(), inputImage.getHeight(), nFrames, nLevels, kltsubsampling, kltpyrlevels, kltnumfeats, kltiterations ) ;
  image   = &inputImage;
  int w   = dm->getImgWidth();
  int h   = dm->getImgHeight();

  /* Allocate memory for feature map and clear it */
  featuremap	   = new unsigned char	  [ w * h ];
  cornerNessBuffer = new float			  [ w * h ];
  pointlist        = new int			  [ w * h * 3 ];

  kltConvergeThreshold /= w; // Normalize the Min-distance threshold by the image width.

  for (int i=0;i<nLevels;i++)
  {
	buffers[i] = new unsigned char [ w * h];
	w = w >> 1; 		
	h = h >> 1;
  }
  proctime = 0.0;
  avgtime  = 0.0;

  printf("\t GpuVis: GPU Data Manager was created...\n");

}



/*! \fn void GpuVis::computePyramid(GpuVis_Image& inputImage)
\brief This function computes the Main Pyramid on the GPU.
*/
void GpuVis::computePyramid(GpuVis_Image& inputImage)
{
	CheckErrorsGL("BEGIN : computePyramid");

	// Step 0 : Setup GPU
	if (subTiming) { timer->Reset(); timer->Start(); }
	dm->resetInputTexture(inputImage.getData());
	
	if (subTiming) { 
		glFinish(); 
		timer->Stop(); 
		printf("\t[ Upload Texture   ] :  %8.2lf msec\n",timer->GetTime()*1000);
	}

	dm->toggleFrames();
	image = &inputImage;
	
	dm->_fbos[0]->Bind();

	// Step 1 : Undistort Input Image
	if (subTiming) { timer->Reset(); timer->Start(); }
	undistort(false);
	if (subTiming) { 
		glFinish(); 
		timer->Stop();
		printf("\t[ Init - Undistort ] :  %8.2lf msec\n",timer->GetTime()*1000);
	}

	// Step 2 : Build Pyramid
	if (subTiming) { timer->Reset(); timer->Start(); }
	buildPyramid();
	if (subTiming) { 
		glFinish(); 
		timer->Stop();
		printf("\t[ Build Pyramid    ] :  %8.2lf msec\n",timer->GetTime()*1000);
	}

	if (0)
	{
		if (subTiming) { timer->Reset(); timer->Start(); }
		readBackPyramid(buffers);
		if (subTiming) { 
			glFinish(); 
			timer->Stop();
			printf("\t[ Readback ] : %8.2lf msec\n",timer->GetTime()*1000);
		}
	}

	return ;

}

/*! \fn GpuKLT_FeatureList * GpuVis::selectGoodFeatures()
\brief This function computes good features to track using KLT_Tracking algorithm.
*/
GpuKLT_FeatureList * GpuVis::selectGoodFeatures()
{

	GpuKLT_FeatureList *ftList = NULL;
	int npoints; 

	// Step 1 : Compute Cornerness Function
	if (subTiming) { timer->Reset(); timer->Start(); }
	computeMinEigenValues(NULL, kltmindist, klteigenthreshold);
	if (subTiming) { glFinish(); timer->Stop(); }
	if (subTiming) printf("\t[ Compute Cornerness ]\t: %8.2lf msec\n",timer->GetTime()*1000);

	// Step 2 : Readback Cornerness Map
	if (subTiming) { timer->Reset(); timer->Start(); }
	npoints = readBackCornerNess( 0, 0, dm->getImgWidth(), dm->getImgHeight() );

	if (subTiming) { glFinish(); timer->Stop(); }
	if (subTiming) printf("\t[ Rdback Cornerness ]\t: %8.2lf msec\n",timer->GetTime()*1000);
	
	// Step 3 : Compute Final List of Features.
	if (subTiming) { timer->Reset(); timer->Start(); }

	// Allocate feature list with target number of features.
	ftList = new GpuKLT_FeatureList( kltnumfeats);

	// Non-maximal Suppression 
	enforceMinDist(pointlist, npoints, &ftList, kltmindist, true);

	if (subTiming) { timer->Stop(); }
	if (subTiming) printf("\t[ Extract on CPU ]\t: %8.2lf msec\n",timer->GetTime()*1000);
	
	// Upload Feature Table onto GPU
	if (subTiming) { timer->Reset(); timer->Start(); }
	dm->uploadFeatureTable(ftList);
	if (subTiming) { timer->Stop(); }
	if (subTiming) printf("\t[ (3) Uploading Feature to GPU ]\t: %8.2lf msec\n",timer->GetTime()*1000);

	return ftList ;
}


/*! \fn void GpuVis::reselectGoodFeatures(GpuKLT_FeatureList **ftList, int dir, float percentage)
\brief This function computes good features to be re-injected when some features have been lost during KLT Tracking.
\param ftList: Feature List
\param dir: direction 1: left, 2: right, 3: top, 4: bot
\param percentage: what fraction of image to look for features in (Urbanscape needs this since vehicle driving direction means new features will appear on one side of frame)
*/
int GpuVis::reselectGoodFeatures(GpuKLT_FeatureList **ftList, int dir, float percentage)
{

	int  nfeats, npoints; 
	int  w_left, w_top, w_width, w_height;

	// Step 1 : Compute Cornerness Function
	if (subTiming) { timer->Reset(); timer->Start(); }
	
	computeMinEigenValues(ftList, kltmindist, klteigenthreshold);
	
	if (subTiming) { 
			glFinish(); 
			timer->Stop();
			printf("\t\t[ Compute Cornerness ]: %8.2lf msec\n",timer->GetTime()*1000);
	}

	// Step 2 : Readback Cornerness Map

	switch (dir)
	{
		case 1: // left
			w_left  = 0;										w_top = 0; 
			w_width = percentage * dm->getImgWidth();			w_height = dm->getImgHeight();
			break;
		case 2: // right
			w_left  = (1 - percentage) * dm->getImgWidth() ;	w_top = 0; 
			w_width = percentage * dm->getImgWidth();			w_height = dm->getImgHeight();
			break;
		case 3: // top
			w_left  = 0;					w_top = 0; 
			w_width = dm->getImgWidth();	w_height = percentage * dm->getImgHeight();
			break;
		case 4: // bot
			w_left  = 0;					w_top = (1.0 - percentage) * dm->getImgHeight() ; 
			w_width = dm->getImgWidth();	w_height = percentage * dm->getImgHeight();
			break;
	}

	if (subTiming) { timer->Reset(); timer->Start(); }
	
	npoints = readBackCornerNess(w_left, w_top, w_width, w_height);
	
	if (subTiming) { 
			glFinish(); 
			timer->Stop();
			printf("\t\t[ Rdback Cornerness  ]: %8.2lf msec\n",timer->GetTime()*1000);	
	}

	// Find the kth greatest cornerness value.
	int oldcount = (*ftList)->_ftCount;

	// Step 3 : Compute Final List of Features.
	if (subTiming) { timer->Reset(); timer->Start(); }

	// Non-maximal Suppression - FOLLOWING VERSION USES SORT
	enforceMinDist(pointlist, npoints, ftList, kltmindist, false);

	if (subTiming) { 
			timer->Stop();
			printf("\t\t[ Extract feats. CPU ]: %8.2lf msec\n",timer->GetTime()*1000);
	}

	nfeats = (*ftList)->_ftCount - oldcount;

	return nfeats;

}


/*! \fn void GpuVis::uploadFeaturesToGPU(GpuKLT_FeatureList **ftList)
\brief This function uploads the current Feature List onto the GPU.
*/
void GpuVis::uploadFeaturesToGPU(GpuKLT_FeatureList **ftList)
{
	dm->uploadFeatureTable(*ftList);
}

 



/*! \fn void GpuVis::drawDebugPatches		 (int ft_num, int fx, int fy, int iteration, int frame, GLuint attachment )
\brief This function does debugging ..
*/
void GpuVis::drawDebugPatches		 (int ft_num, int fx, int fy, int iteration, int frame, GLuint attachment )
{
	int destx, desty;
	float buf[49*4];

	desty = 7*iteration;
		
	if (frame==0)
	{
		destx = 20*ft_num + 0;
	}
	else if (frame==1)
		destx = 20*ft_num + 7;

	glEnable(_texTarget);
	glPixelStorei(GL_PACK_ALIGNMENT, 4*sizeof(GLubyte) );  	
	glReadBuffer(attachment);
	glReadPixels( fx, fy, 7, 7, GL_RGBA, GL_FLOAT, buf);

	//for (int q=0;q<49*4;q++)
	//	buf[q]=0.8;

	glBindTexture(_texTarget, dm->debugTable->_texID);
	glTexSubImage2D (_texTarget, 0, destx, desty, 7, 7 , GL_RGBA, GL_FLOAT, buf);

}





         

/*! \fn void GpuVis::trackFeatures	(GpuKLT_FeatureList **ftList)
\brief This function does KLT Tracking on GPU
	Here are the steps of the tracking algo
			   a. Compute 7x7 blocks (I1-I2, I2-I1, grad_X, grad_Y) output into _kltFloatingWindowTU
			   b. Compute Partial Sum 1  output into _kltPartialSumTU
			   c. Compute Partial Sum 2  output into _kltMatrixElemTU
			   d. Invoke  Solver         output into _kltFeaturesTU
			
			Fragment Programs:	floatingWindow_Kernel	partialSum_Kernel matrixElem_Kernel	    matrixSolve_Kernel
			Storage Buffers  :  PyramidTU              _kltFeaturesTU	  _kltFloatingWindowTU	_kltPartialSumTU	_kltMatrixElemTU
*/
void GpuVis::trackFeatures	(GpuKLT_FeatureList **ftList)
{

	static bool firstTime = true;

	int			  currFrame, otherFrame, iter, count;
	float         px, py, xOffset, yOffset, xoff, yoff, tx1, ty1, ty2;

	TextureUnit * imgA_TU, * imgB_TU;
	int			  maxIter		= dm->getNumKLTIterations();
	int			  coarseLev		= nLevels-1;
	float		  reciprocalX	= dm->getXOffset();
	float		  reciprocalY	= dm->getYOffset();
	int			  nFeats		= dm->getFeatureTableSize();

	int			  step			= nFeats / 10;
	int			  sampling, pyr_level;
	currFrame					= dm->getCurrentFrameIndex();
	otherFrame					= 1 - currFrame;

	dm->_fbo1.Bind();
	
	if (firstTime)
	{
		dm->_fbo1.AttachTexture(GL_COLOR_ATTACHMENT0_EXT, _texTarget, dm->getFloatingWindowTextureUnit()->_texID );
		dm->_fbo1.AttachTexture(GL_COLOR_ATTACHMENT1_EXT, _texTarget, dm->getPartialSumTextureUnit()->_texID );
		dm->_fbo1.AttachTexture(GL_COLOR_ATTACHMENT2_EXT, _texTarget, dm->getMatrixElemTextureUnit()->_texID );
		dm->_fbo1.AttachTexture(GL_COLOR_ATTACHMENT3_EXT, _texTarget, dm->getFeaturesTextureUnit()->_texID);
		firstTime = false;
	}

	gpgpu_setup(dm->getXSize(),dm->getYSize());
	
	iter = 0;

	for (int i = kltpyrlevels - 1; i>= 0 ; i--)
    {
		sampling = i * kltsubsampling;
		pyr_level = 0;
		while (sampling>1) {
			pyr_level++;
			sampling = sampling >> 1;
		}

		imgA_TU   = dm->getPyramidBlock( otherFrame  , pyr_level , _MAIN_BUF);
		imgB_TU   = dm->getPyramidBlock( currFrame   , pyr_level , _MAIN_BUF);

		switch (architecture)
		{
			case GPU_ATI:
				xoff =  1.0 / (1.0 * imgA_TU->_w); 	
				yoff =  1.0 / (1.0 * imgA_TU->_h);
				break;
			case GPU_NVIDIA:
				xoff =  1.0; 	
				yoff =  1.0;
				break;
		}

		for (int j=0;j<maxIter;j++)
        {
			int p;

			//================================
			// STEP 1 : Compute FLOATING WINDOW
			//================================
			
			glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

			// ==========DEBUG =======================// ==========DEBUG =======================
		    // ==========DEBUG =======================// ==========DEBUG =======================
		
			if (enableDebug)
			{
				w1_Kernel->activate	();
				w1_Kernel->bindInput(dm->getFeaturesTextureUnit()->_texID);
				w1_Kernel->setRange	(dm->getXSize(), dm->getYSize());

				glEnable(GL_TEXTURE_2D);

				cgGLSetTextureParameter		( w1_Kernel->fProgram->_FPtexture1, imgA_TU->_texID );
				cgGLEnableTextureParameter	( w1_Kernel->fProgram->_FPtexture1 );

				cgGLSetTextureParameter		( w1_Kernel->fProgram->_FPtexture2, imgB_TU->_texID );
				cgGLEnableTextureParameter	( w1_Kernel->fProgram->_FPtexture2 );
			
				if (architecture == GPU_ATI)
				{
					cgGLSetParameter1f			( xoff3_Param, xoff );
					cgGLSetParameter1f			( yoff3_Param, yoff );
				}
				else if (architecture == GPU_NVIDIA)
				{
					cgGLSetParameter1f			( xoff3_Param, 1.0 * imgA_TU->_w );
					cgGLSetParameter1f			( yoff3_Param, 1.0 * imgA_TU->_h );
				}

				count = 0;

				//Render
				for (p=0;p<10 ; p++)	{
					for (int q=0;q<step;q++)	{
						px      = q * 7; 	py = p * 7;
						xOffset = (count + 0.5) * reciprocalX ;	 
						yOffset = (iter  + 0.5) * reciprocalY ;
						glBegin(GL_QUADS);
						float val = 3.5;
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,  yOffset  );
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     -3.5, -3.5    );
						glVertex2f(px  ,py  );
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,  yOffset  );
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     3.5,  -3.5    );
						glVertex2f(px+7,py  );
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,   yOffset );
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     3.5,   3.5    );
						glVertex2f(px+7,py+7);
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,   yOffset );	
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     -3.5, 3.5     );
						glVertex2f(px  ,py+7);
						glEnd();
						count++;
					} // for q
				} // for p

				w1_Kernel->deactivate();

				drawDebugPatches		 (0, 0, 0, iter, 0, GL_COLOR_ATTACHMENT0_EXT );
				drawDebugPatches		 (1, 7, 0, iter, 0, GL_COLOR_ATTACHMENT0_EXT );
				drawDebugPatches		 (2,14, 0, iter, 0, GL_COLOR_ATTACHMENT0_EXT );
				drawDebugPatches		 (3,21, 0, iter, 0, GL_COLOR_ATTACHMENT0_EXT );
            
				// ==========DEBUG =======================// ==========DEBUG =======================
				// ==========DEBUG =======================// ==========DEBUG =======================


				w2_Kernel->activate	();
				w2_Kernel->bindInput(dm->getFeaturesTextureUnit()->_texID);
				w2_Kernel->setRange	(dm->getXSize(), dm->getYSize());

				glEnable(GL_TEXTURE_2D);

				cgGLSetTextureParameter		( w2_Kernel->fProgram->_FPtexture1, imgA_TU->_texID );
				cgGLEnableTextureParameter	( w2_Kernel->fProgram->_FPtexture1 );

				cgGLSetTextureParameter		( w2_Kernel->fProgram->_FPtexture2, imgB_TU->_texID );
				cgGLEnableTextureParameter	( w2_Kernel->fProgram->_FPtexture2 );

				if (architecture == GPU_ATI)
				{
					cgGLSetParameter1f			( xoff4_Param, xoff );
					cgGLSetParameter1f			( yoff4_Param, yoff );
				}
				else if (architecture == GPU_NVIDIA)
				{
					cgGLSetParameter1f			( xoff4_Param, 1.0 * imgA_TU->_w );
					cgGLSetParameter1f			( yoff4_Param, 1.0 * imgA_TU->_h );
				}

				count = 0;

				//Render
				for (p=0;p<10 ; p++)	{
					for (int q=0;q<step;q++)	{
						px      = q * 7; 	py = p * 7;
						xOffset = (count + 0.5) * reciprocalX ;	 
						yOffset = (iter  + 0.5) * reciprocalY ;
						glBegin(GL_QUADS);
						float val = 3.5;
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,  yOffset  );
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     -3.5, -3.5    );
						glVertex2f(px  ,py  );
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,  yOffset  );
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     3.5,  -3.5    );
						glVertex2f(px+7,py  );
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,   yOffset );
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     3.5,   3.5    );
						glVertex2f(px+7,py+7);
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,   yOffset );	
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     -3.5, 3.5     );
						glVertex2f(px  ,py+7);
						glEnd();
						count++;
					} // for q
				} // for p

				w2_Kernel->deactivate();

				drawDebugPatches		 (0, 0, 0, iter, 1, GL_COLOR_ATTACHMENT0_EXT );
				drawDebugPatches		 (1, 7, 0, iter, 1, GL_COLOR_ATTACHMENT0_EXT );
				drawDebugPatches		 (2,14, 0, iter, 1, GL_COLOR_ATTACHMENT0_EXT );
				drawDebugPatches		 (3,21, 0, iter, 1, GL_COLOR_ATTACHMENT0_EXT );
			
			} // if enableDebug

		    // ==========END OF DEBUG =======================// ==========DEBUG =======================
		    // ==========END OF DEBUG =======================// ==========DEBUG =======================
							
			floatingWindow_Kernel->activate	();
			floatingWindow_Kernel->bindInput(dm->getFeaturesTextureUnit()->_texID);
			floatingWindow_Kernel->setRange	(dm->getXSize(), dm->getYSize());

			glEnable(_texTarget);

			cgGLSetTextureParameter		( floatingWindow_Kernel->fProgram->_FPtexture1, imgA_TU->_texID );
			cgGLEnableTextureParameter	( floatingWindow_Kernel->fProgram->_FPtexture1 );

			cgGLSetTextureParameter		( floatingWindow_Kernel->fProgram->_FPtexture2, imgB_TU->_texID );
			cgGLEnableTextureParameter	( floatingWindow_Kernel->fProgram->_FPtexture2 );

			if (architecture == GPU_ATI)
			{
				cgGLSetParameter1f			( xoff_Param, xoff );
				cgGLSetParameter1f			( yoff_Param, yoff );
			}
			else if (architecture == GPU_NVIDIA)
			{
				cgGLSetParameter1f			( xoff_Param, 1.0 * imgA_TU->_w );
				cgGLSetParameter1f			( yoff_Param, 1.0 * imgA_TU->_h );
			}
			count = 0;

			//Render
			for (p=0;p<10 ; p++)	{
				for (int q=0;q<step;q++)	{
					px      = q * 7; 	py = p * 7;
					xOffset = (count + 0.5) * reciprocalX ;	 
					yOffset = (iter  + 0.5) * reciprocalY ;
					glBegin(GL_QUADS);
					float val = 3.5;
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,  yOffset  );
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     -val, -val    );
					glVertex2f(px  ,py  );
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,  yOffset  );
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     val,  -val    );
					glVertex2f(px+7,py  );
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,   yOffset );
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     val,   val    );
					glVertex2f(px+7,py+7);
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     xOffset ,   yOffset );	
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     -val, val     );
					glVertex2f(px  ,py+7);
					glEnd();
					count++;
				} // for q
			} // for p

			floatingWindow_Kernel->deactivate();

			//================================
			// STEP 2 : Compute PARTIAL SUM
			//================================
	
			dm->_fbo1.IsValid(std::cout);
			CheckErrorsGL("tracking:: 3",std::cout);

			glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);	
			
			pSum1_Kernel->activate	();
			pSum1_Kernel->bindInput	(dm->getFloatingWindowTextureUnit()->_texID);
			pSum1_Kernel->setRange	(dm->getXSize(), dm->getYSize());

			// Render
			for (p=0;p<10 ; p++)	{ //10
				for (int q=0;q<step;q++)	{
					px    =   q * 7 + 3; 	py = p * 7 + 3;
					count =   p * step + q;

					ty1   = ( py - 3 +0.5) * reciprocalY;
					ty2   = ( py + 3 +0.5) * reciprocalY;
				
					glBegin (GL_LINES);
						tx1   = ( px - 3 +0.5) * reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     tx1, ty1 );
						glVertex2f (count,0 );

						tx1   = ( px - 3 +0.5) * reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     tx1, ty2 );				
						glVertex2f (count,7 );

					glEnd ();

				}
			}
			pSum1_Kernel->deactivate();

			pSum2_Kernel->activate	();
			pSum2_Kernel->bindInput	(dm->getFloatingWindowTextureUnit()->_texID);
			pSum2_Kernel->setRange	(dm->getXSize(), dm->getYSize());

			// Render
			for (p=0;p<10 ; p++)	{
				for (int q=0;q<step;q++)	{
					px    =   q * 7 + 3; 	py = p * 7 + 3;
					count =   p * step + q;
					ty1   = ( py - 3 +0.5) * reciprocalY;
					ty2   = ( py + 3 +0.5) * reciprocalY;
	
					glBegin (GL_LINES);
						tx1   = ( px - 3 +0.5) * reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     tx1, ty1 );
						tx1 += reciprocalX;
						glVertex2f (count,7 );

						tx1   = ( px - 3 +0.5) * reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     tx1, ty2 );
						tx1 += reciprocalX;
						glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     tx1, ty2 );
						tx1 += reciprocalX;						
						glVertex2f (count,14 );
					glEnd ();

				}
			}
			pSum2_Kernel->deactivate();
			

			//================================
			// STEP 3 : Compute MATRIX ELEMS
			//================================
	
			dm->_fbo1.IsValid(std::cout);
			CheckErrorsGL("tracking:: 4",std::cout);

			glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
			
		    mElem1_Kernel->activate	 ();
			mElem1_Kernel->bindInput (dm->getPartialSumTextureUnit()->_texID);
			mElem1_Kernel->setRange	 (dm->getXSize(), dm->getYSize());
			glEnable(_texTarget);

			// Render
			for (p=0;p<nFeats;p++) 
			{	
				tx1   = (p+0.5) * reciprocalX;
				glBegin (GL_POINTS);
					ty1   = 0.5* reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     tx1, ty1 );
					glVertex2f (p,1);
				glEnd ();
			}
			mElem1_Kernel->deactivate();

			mElem2_Kernel->activate	 ();
			mElem2_Kernel->bindInput (dm->getPartialSumTextureUnit()->_texID);
			mElem2_Kernel->setRange	 (dm->getXSize(), dm->getYSize());
			glEnable(_texTarget);
			cgGLSetTextureParameter		( mElem2_Kernel->fProgram->_FPtexture1, dm->getFeaturesTextureUnit()->_texID);
			cgGLEnableTextureParameter	( mElem2_Kernel->fProgram->_FPtexture1 );

			// Render
			for (p=0;p<nFeats;p++) 
			{	
				tx1   = (   p+0.5) * reciprocalX;
				//ty2   = (iter+0.5) * reciprocalY;
				glBegin (GL_POINTS);
					ty1   = 7.5 *reciprocalY;				
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     tx1, ty1 );
				
					glVertex2f (p,2 );
				glEnd ();
			}
			mElem2_Kernel->deactivate();
	
			mElem3_Kernel->activate	 ();
			mElem3_Kernel->bindInput (dm->getFeaturesTextureUnit()->_texID);
			mElem3_Kernel->setRange	 (dm->getXSize(), dm->getYSize());
			glEnable(_texTarget);

			// Render
			for (p=0;p<nFeats;p++) 
			{	
				tx1   = (   p+0.5) * reciprocalX;
				ty1   = (iter+0.5) * reciprocalY;
				glBegin (GL_POINTS);				
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     tx1, ty1 );		
					glVertex2f (p,3 );
				glEnd ();
			}
			mElem3_Kernel->deactivate();


			//================================
			// STEP 4 : SOLVE EQUATIONS
			//================================
			Kernel * solver;
			float   pixsz_x, pixsz_y;

			if (i==0 && j== (maxIter -1))
				solver = mSolve_f_Kernel;
			else 
				solver = mSolve_Kernel;
				
			dm->_fbo1.IsValid(std::cout);
			CheckErrorsGL("tracking:: 5");

			glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
			solver->activate  ();
			solver->bindInput (dm->getMatrixElemTextureUnit()->_texID);
			solver->setRange  (dm->getXSize(), dm->getYSize());
			glEnable(_texTarget);

			if (architecture == GPU_ATI)
			{
				pixsz_x = xoff;
				pixsz_y = yoff;
			}
			else if (architecture == GPU_NVIDIA)
			{
				pixsz_x = 1.0 / (1.0*dm->getImgWidth());
				pixsz_y = 1.0 / (1.0*dm->getImgHeight());
			}

			if (i==0 && j== (maxIter -1))
			{
				cgGLSetParameter1f			( xoff2_Param, pixsz_x );
				cgGLSetParameter1f			( yoff2_Param, pixsz_y );
			}
			else
			{
				cgGLSetParameter1f			( xoff1_Param, pixsz_x );
				cgGLSetParameter1f			( yoff1_Param, pixsz_y );
			}

			// Render
			for (p=0;p<nFeats;p++) 
			{	
				tx1   = (p+0.5) * reciprocalX;
				
				glBegin(GL_POINTS);
					ty1  = 0.5  * reciprocalY;
					glMultiTexCoord2fARB ( GL_TEXTURE0_ARB, tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB ( GL_TEXTURE1_ARB, tx1, ty1 );
					ty1 += reciprocalY;
					glMultiTexCoord2fARB ( GL_TEXTURE2_ARB, tx1, ty1 );
					glVertex2f (p,(iter+2));

				glEnd();
			}
			solver->deactivate();

			iter++;

		} // for j

	} // for i

	// Readback the tracked features
	float          *buf = new float [ nFeats * 5 ];
	int             imw, imh;
	int				nSize = dm->getFeatureTableSize();

	imw = dm->getImgWidth();	imh = dm->getImgHeight();

	/* FINAL READBACK OF TRACKED FEATURES */
	glPixelStorei(GL_PACK_ALIGNMENT, 4*sizeof(GLubyte) ); 
	glReadBuffer(GL_COLOR_ATTACHMENT3_EXT);
	glReadPixels( 0,  maxIter * kltpyrlevels, nFeats, 1, GL_RGBA, GL_FLOAT, buf);

	// Copy features into Row 0 of Feature Table Texture Unit.
	int	c = 0;
	for (int i=0;i< (*ftList)->_nFeats ;i++)
	{
		if ( (*ftList)->_list[i]->valid)
		{

			int ret = (*ftList)->_list[i]->updatePos( kltConvergeThreshold, kltSSDThreshold, kltborder, buf[4*c], buf[4*c+1], buf[4*c+2], buf[4*c+3], imw, imh);

			if (ret == -1)
				((*ftList)->_ftCount)--;
		
			c++;
		}
		if (c >= nSize)
			break;
	}

	return;
}



/*! \fn void GpuVis::computeMinEigenValues()
\brief This function computes the cornerness values on the GPU. 
*/
void GpuVis::computeMinEigenValues(GpuKLT_FeatureList **ftList, int mindist, float eigenthreshold)
{
	static bool		firstTime = true;
	float           top, left, bot, right, offset;
	int				imw, imh;  
	TextureUnit    *srcTU;
	int             currFrame = dm->getCurrentFrameIndex();
	imw			  = dm->getImgWidth();
	imh			  = dm->getImgHeight();
	srcTU		  = dm->getPyramidBlock(   currFrame ,   0 , _MAIN_BUF);

	dm->_fbo2.Bind();
	glViewport(0, 0, imw, imh);
	glMatrixMode(GL_PROJECTION);    
	glLoadIdentity();                   
	glMatrixMode(GL_MODELVIEW);     
	glLoadIdentity();

	if (firstTime)
	{
		dm->_fbo2.AttachTexture(GL_COLOR_ATTACHMENT0_EXT, _texTarget, dm->getCornerness1TextureUnit()->_texID);
		dm->_fbo2.AttachTexture(GL_COLOR_ATTACHMENT1_EXT, _texTarget, dm->getCornerness2TextureUnit()->_texID);
		dm->_fbo2.AttachTexture(GL_DEPTH_ATTACHMENT_EXT, _texTarget, dm->depthTexture);
		firstTime = false;
	}

	cornerness_pass1_Kernel->activate(); 
	cornerness_pass1_Kernel->bindInput(srcTU->_texID);

	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	switch(architecture) {
		case GPU_ATI:
			left = 0.0; right = 1.0;
			top  = 0.0; bot   = 1.0;
			offset = 1.0 / (1.0 * dm->getImgWidth());
			break;
		case GPU_NVIDIA:
			left = 0.0; right = dm->getImgWidth();
			top  = 0.0; bot   = dm->getImgHeight();
			offset = 1.0 ;
			break;
	}

	glEnable(_texTarget);

	glBegin (GL_QUADS);
	    
	    // upper left      
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     left           ,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     left -   offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     left +   offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     left - 3*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     left - 2*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     left + 2*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     left + 3*offset,   bot  );
		glVertex2f   (-1     ,   1   );

		// upper right
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	 right           ,   bot  ); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	 right -   offset,	 bot  ); 
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	 right +   offset,	 bot  ); 
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,    right - 3*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,    right - 2*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,    right + 2*offset,   bot  );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,    right + 3*offset,   bot  );
        glVertex2f   (1   ,   1   );

        // lower right
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	 right            ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	 right -   offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	 right +   offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	 right - 3*offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	 right - 2*offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	 right + 2*offset ,   top );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	 right + 3*offset ,   top );
		glVertex2f   (1    ,   -1  );
	
		// lower left 
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	  left           ,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	  left -   offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	  left +   offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	  left - 3*offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	  left - 2*offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	  left + 2*offset,   top  );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	  left + 3*offset,   top  );
		glVertex2f   ( -1    ,   -1  );

    glEnd ();

	glDisable(_texTarget);

	cornerness_pass1_Kernel->deactivate();

	if (ftList !=NULL) 
	{
		glViewport(0, 0, imw, imh);
		glMatrixMode(GL_PROJECTION);    
		glLoadIdentity();                   
		gluOrtho2D(0, imw, 0, imh);     
		glMatrixMode(GL_MODELVIEW);     
		glLoadIdentity();

		glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
		
		// Clear the Cornerness Buffer (Depth + Color)
		glDepthFunc(GL_ALWAYS);          // Always pass Depth Test.
		glDepthMask(GL_TRUE);            // Allow writing into Depth Buffer
		glEnable (GL_DEPTH_TEST);
	
		cornerness_clearZ_Kernel->activate();
	
		glBegin(GL_QUADS);
			glVertex2f   (  0,  imh  );
			glVertex2f   (imw,  imh  );
			glVertex2f   (imw,    0  );	
			glVertex2f   (  0,    0  );
		glEnd();
	
		cornerness_clearZ_Kernel->deactivate();

		// Now Do Early Z-Culling by rendering mask for the existing features.
		glDepthFunc(GL_ALWAYS);          // Always pass Depth Test.
		glDepthMask(GL_TRUE);            // Allow writing into Depth Buffer
		glEnable (GL_DEPTH_TEST);

		// Set viewport for RENDERING MASK !

		cornerness_earlyZ_Kernel->activate();

		for (int indx = 0 ; indx < (*ftList)->_nFeats ; indx++)
		{
			 if ((*ftList)->_list[indx]->valid)  
			{
					int featx   = (int) (*ftList)->_list[indx]->x;
					int featy   = (int) (*ftList)->_list[indx]->y;
					glBegin(GL_QUADS);
						glVertex2f   (featx - mindist,  featy + mindist   );
						glVertex2f   (featx + mindist,  featy + mindist   );
						glVertex2f   (featx + mindist,  featy - mindist   );	
						glVertex2f   (featx - mindist,  featy - mindist   );
					glEnd();
			}
		}
		cornerness_earlyZ_Kernel->deactivate();

		// Do next pass - early Z culls out the fragments masked out in the previous step.

		glDepthFunc(GL_GREATER);        // Pass Fragment if depth != value in depth buffer
		glDepthMask(GL_FALSE);           // Do not write into Depth Buffer, only do depth test.
	}

	glViewport(0, 0, imw, imh);
	glMatrixMode(GL_PROJECTION);    
	glLoadIdentity();                   
	glMatrixMode(GL_MODELVIEW);     
	glLoadIdentity();

	srcTU = dm->getCornerness1TextureUnit();

	cornerness_pass2_Kernel->activate();
	cornerness_pass2_Kernel->bindInput(srcTU->_texID);

    cgGLSetParameter1f( eigenThresh_Param, eigenthreshold );

	glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);

	switch(architecture) {
		case GPU_ATI:
			left = 0.0; right = 1.0;
			top  = 0.0; bot   = 1.0;
			offset = 1.0 / (1.0 * dm->getImgHeight());
			break;
		case GPU_NVIDIA:
			left = 0.0; right = dm->getImgWidth();
			top  = 0.0; bot   = dm->getImgHeight();
			offset = 1.0 ;
			break;
	}

	glEnable(_texTarget);
	
	glBegin (GL_QUADS);
	    
	    // upper left      
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,     left,   bot            );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,     left,   bot -   offset );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,     left,   bot +   offset );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,     left,   bot - 3*offset );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,     left,   bot - 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,     left,   bot + 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,     left,   bot + 3*offset );
		glVertex2f   (-1     ,   1   );

		// upper right
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	 right,	  bot            ); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	 right,	  bot -   offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	 right,	  bot +   offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	 right,	  bot - 3*offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	 right,	  bot - 2*offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	 right,	  bot + 2*offset ); 
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	 right,	  bot + 3*offset ); 
        glVertex2f   (1    ,   1   );

        // lower right
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	 right,   top	   	     );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	 right,   top -   offset );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	 right,   top +   offset );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	 right,   top - 3*offset );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	 right,   top - 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	 right,   top + 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	 right,   top + 3*offset );
		glVertex2f   (1    ,   -1  );
	
		// lower left 
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	  left,   top	   	     );
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,	  left,   top -   offset );
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB,	  left,   top +   offset );
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB,	  left,   top - 3*offset );
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB,	  left,   top - 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE5_ARB,	  left,   top + 2*offset );
		glMultiTexCoord2fARB(GL_TEXTURE6_ARB,	  left,   top + 3*offset );
		glVertex2f   ( -1    ,   -1   );

    glEnd ();

	glDisable(_texTarget);
	
	if (ftList!=NULL)
	{
		glDepthMask(GL_TRUE);  // Reset Depth Buffer state.
		glDepthFunc(GL_LESS);  // Reset Depth Buffer state.
		glDisable(GL_DEPTH_TEST);
	}

	cornerness_pass2_Kernel->deactivate();

	return;
}



/*! \fn int GpuVis::readBackCornerNess(int boxleft, int boxtop, int boxwidth, int boxheight)
\brief This function reads back the cornerness values from the GPU and extracts the new feature locations on the CPU.
*/
int GpuVis::readBackCornerNess(int boxleft, int boxtop, int boxwidth, int boxheight)
{
	int            idx, frame;

	frame   = dm->getCurrentFrameIndex();
	
	int   imw = dm->getImgWidth();
	int   imh = dm->getImgHeight();

	// IMPORTANT!!! FBO IS ALREADY BOUND (Look where this routine gets called from)! AND The Texture to be read is already attached in COLOR_ATTACHMENT1 !!!

	glPixelStorei(GL_PACK_ALIGNMENT, 4*sizeof(GLubyte) );  
	glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);

	int btop    = (  boxtop > kltborder ) ? boxtop : kltborder;
	int bheight = ( (btop + boxheight) < (imh - kltborder)) ? boxheight : (imh- kltborder) - (btop);
	int bleft   = (  boxleft > kltborder ) ? boxleft : kltborder;
	int bwidth  = ( (bleft + boxwidth) < (imw - kltborder)) ? boxwidth : (imw - kltborder) - (bleft);

	glReadPixels( bleft, btop, bwidth, bheight, GL_RED, GL_FLOAT, cornerNessBuffer);

	pointcount = 0;
	for (int i=0; i< bheight; i++)
		for (int j=0; j < bwidth; j++) 
		{
			idx = i * bwidth + j ; 
			if (cornerNessBuffer[ idx ] > klteigenthreshold)
			{
				pointlist[3 * pointcount    ] = bleft + j;
				pointlist[3 * pointcount + 1] = btop  + i;
				pointlist[3 * pointcount + 2] = 10000 * cornerNessBuffer[idx];
				pointcount++;
			}
		}
	return pointcount;
}




/*!
\fn void	GpuVis::undistort(bool flagClear)
\brief Radial Undistort
*/
void	GpuVis::undistort(bool flagClear)
{	
	static bool		firstTimeUndist = true;
	TextureUnit * dstTU;
	float         tw, th;
	// RGB -> Grey + Undistort.
	int iw = dm->getImgWidth();
	int ih = dm->getImgHeight();

	if (architecture == GPU_ATI)
	{ tw = 1.0; th = 1.0; }
	else if (architecture == GPU_NVIDIA)
	{ tw = iw; th = ih; }

	gpgpu_setup(iw,ih);

    glGetIntegerv(GL_DRAW_BUFFER, &_currentDrawBuffer); 

	// Attach texture to framebuffer color buffer
	if (firstTimeUndist)
	{
		int curreFrameIndex = 0;
		int otherFrameIndex = 1;

		for (int lev = 0; lev < nLevels; lev++)
		{
		
			dm->_fbos[lev]->Bind();

			dstTU = dm->getPyramidBlock	(  curreFrameIndex, lev, _AUXI_BUF);
			dm->_fbos[lev]->AttachTexture(GL_COLOR_ATTACHMENT0_EXT, _texTarget, dstTU->_texID );

			dstTU = dm->getPyramidBlock	(  curreFrameIndex, lev, _MAIN_BUF);
			dm->_fbos[lev]->AttachTexture(GL_COLOR_ATTACHMENT1_EXT, _texTarget, dstTU->_texID );
		
			dstTU = dm->getPyramidBlock	(  otherFrameIndex, lev, _AUXI_BUF);
			dm->_fbos[lev]->AttachTexture(GL_COLOR_ATTACHMENT2_EXT, _texTarget, dstTU->_texID );
			
			dstTU = dm->getPyramidBlock	(  otherFrameIndex, lev, _MAIN_BUF);
			dm->_fbos[lev]->AttachTexture(GL_COLOR_ATTACHMENT3_EXT, _texTarget, dstTU->_texID );
		
		}		
		firstTimeUndist = false;
		dm->_fbos[0]->Bind();

	}

	int currFrameIndex = dm->getCurrentFrameIndex();

	if (currFrameIndex == 0)
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	else
		glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);

	undistort_Kernel->activate(); 
	undistort_Kernel->bindInput(dm->getInputTextureUnit()->_texID);
	undistort_Kernel->setRange(iw,ih);
	glEnable(_texTarget);
	cgGLSetTextureParameter(undistort_Kernel->fProgram->_FPtexture1, dm->getDistortionMapTextureUnit()->_texID);
	cgGLEnableTextureParameter(undistort_Kernel->fProgram->_FPtexture1);

	glBegin (GL_QUADS);
	    glMultiTexCoord2iARB(GL_TEXTURE0_ARB,     0.0,    th   ); 	   glVertex2f   (  0,    ih );
		glMultiTexCoord2iARB(GL_TEXTURE0_ARB,      tw,    th   ); 	   glVertex2f   ( iw,    ih );
	    glMultiTexCoord2iARB(GL_TEXTURE0_ARB,      tw,   0.0   );      glVertex2f   ( iw,    0  );
        glMultiTexCoord2iARB(GL_TEXTURE0_ARB,     0.0,   0.0   );      glVertex2f   (  0,    0  );
    glEnd ();
	
	undistort_Kernel->deactivate();
	
	glMatrixMode(GL_PROJECTION);    
	glLoadIdentity();                    
	glMatrixMode(GL_MODELVIEW);     
	glLoadIdentity();

}


/*!
\fn void	GpuVis::buildPyramid  ()
\brief  Builds Gaussian Pyramid.
   Input Image is in GreyTexture Render Texture Object
   Fill in the layers of scale space stack by doing ping pong between 2 surfaces
*/
void	GpuVis::buildPyramid			   ()
{
	int             currFrame, otherFrame;
	int				imw, imh, origw, origh, factor;
	float           xoffset,yoffset, t0, t1, t2, t3;
	TextureUnit    *srcTU, *dstTU;
	GLenum			attachment;


	origw = imw = dm->getImgWidth();
	origh = imh = dm->getImgHeight();
	currFrame   = dm->getCurrentFrameIndex();
	otherFrame  = 1 - currFrame;
	
	switch (architecture)
	{
		case GPU_ATI:
			xoffset     = 1.0 / (1.0*origw);
			yoffset     = 1.0 / (1.0*origh);
			t0 = 0.0; t1 = 0.0; t2 = 1.0; t3 = 1.0;
			break;
		case GPU_NVIDIA:
			xoffset     = 1.0 ;
			yoffset     = 1.0 ;
			t0 = 0.0; t1 = 0.0; t2 = imh; t3 = imw;
			break;
	}
	srcTU = dm->getPyramidBlock( currFrame , 0 , _AUXI_BUF);
	dstTU = dm->getPyramidBlock( otherFrame, 0 , _AUXI_BUF);

	g1->bindInput(srcTU->_texID);

	// already attached dm->_fbo.AttachTexture(GL_COLOR_ATTACHMENT0_EXT, _texTarget, dstTU->_texID );
	if (otherFrame == 0) 	
		attachment = GL_COLOR_ATTACHMENT0_EXT;
	else if (otherFrame == 1)
		attachment = GL_COLOR_ATTACHMENT2_EXT;
	glDrawBuffer(attachment);
	
	g1->setRange(imw,imh);
	g1->convolveRow(0, 0, imh, imw, t0, t1, t2, t3, xoffset);

	srcTU = dm->getPyramidBlock( otherFrame , 0 , _AUXI_BUF);
	dstTU = dm->getPyramidBlock( currFrame  , 0 , _MAIN_BUF);

	g1->bindInput(srcTU->_texID);
	// already atached dm->_fbo.AttachTexture(GL_COLOR_ATTACHMENT0_EXT, _texTarget, dstTU->_texID );

	if (currFrame == 0) 	
		attachment = GL_COLOR_ATTACHMENT1_EXT;
	else if (currFrame == 1)
		attachment = GL_COLOR_ATTACHMENT3_EXT;
	glDrawBuffer(attachment);

	g1->setRange(imw,imh);
	g1->convolveCol(0, 0, imh, imw, t0, t1, t2, t3, yoffset);

	if (nLevels > 0)
		dm->_fbos[1]->Bind();



	// For each level
	for (int j=1; j< nLevels; j++) 
	{	
		factor   = pow(2.0,j-1);
		if (architecture == GPU_ATI)
		{
			xoffset *= factor;
			yoffset *= factor;
		}	
		else 
		{
			t0 = 0.0; t1 = 0.0; t2 = imh; t3 = imw;
		}
		// Step 1 : Horiz Convolution

		srcTU = dm->getPyramidBlock( currFrame , j-1 , _MAIN_BUF);
		dstTU = dm->getPyramidBlock( otherFrame, j-1 , _AUXI_BUF);	
		g2->bindInput(srcTU->_texID);

		// already attached dm->_fbo.AttachTexture(GL_COLOR_ATTACHMENT0_EXT, _texTarget, dstTU->_texID );
		if (otherFrame == 0) 
			attachment = GL_COLOR_ATTACHMENT0_EXT;
		else if (otherFrame == 1)
			attachment = GL_COLOR_ATTACHMENT2_EXT;
		glDrawBuffer(attachment);
		
		g2->setRange(origw, origh);
		g2->convolveRow(0, 0, imh, imw, t0, t1, t2, t3, xoffset);

		// Step 2 : Verti Convolution
		srcTU = dm->getPyramidBlock( otherFrame , j-1 , _AUXI_BUF);
		dstTU = dm->getPyramidBlock( currFrame  , j-1 , _AUXI_BUF);
		g2->bindInput(srcTU->_texID);
		// already attached dm->_fbo.AttachTexture(GL_COLOR_ATTACHMENT0_EXT, _texTarget, dstTU->_texID );
		//glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		if (currFrame == 0) 
			attachment = GL_COLOR_ATTACHMENT0_EXT;
		else if (currFrame == 1)
			attachment = GL_COLOR_ATTACHMENT2_EXT;
		glDrawBuffer(attachment);

	    g2->convolveCol(0, 0, imh, imw, t0, t1, t2, t3, yoffset);
		
		// Step 3 : Subsample 
		imw = imw >> 1;		imh = imh >> 1;

		dm->_fbos[j]->Bind();

		srcTU = dm->getPyramidBlock( currFrame ,  j-1 , _AUXI_BUF);
		dstTU = dm->getPyramidBlock( currFrame  ,  j  , _MAIN_BUF);

		passThru_Kernel->activate(); 
		passThru_Kernel->bindInput(srcTU->_texID);
		
		// already attached dm->_fbo.AttachTexture(GL_COLOR_ATTACHMENT0_EXT, _texTarget, dstTU->_texID );
		if (currFrame == 0) 
			attachment = GL_COLOR_ATTACHMENT1_EXT;
		else if (currFrame == 1)
			attachment = GL_COLOR_ATTACHMENT3_EXT;
		glDrawBuffer(attachment);

		passThru_Kernel->setRange(origw, origh);
		passThru_Kernel->execute(  0, 0, imh, imw, t0 + 0.5*yoffset, t1 + 0.5*xoffset, t2-0.5*yoffset, t3 -0.5*xoffset);			  			     
		passThru_Kernel->deactivate();

	}	

	return;

}




/*!
\fn void	GpuVis::readBackPyramid			   (unsigned char **charbuf)
\brief  Readback the image pyramid.
*/
void	GpuVis::readBackPyramid			   (unsigned char **charbuf)
{
	int            frame;

	frame     = dm->getCurrentFrameIndex();
	int   imw = dm->getImgWidth();
	int   imh = dm->getImgHeight();
	glPixelStorei(GL_PACK_ALIGNMENT, 4*sizeof(GLubyte) );  

	for (int i=0;i<nLevels;i++)
	{
		dm->_fbos[i]->Bind(); 
		if (frame == 0)
			glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		else
			glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);

		glReadPixels(0,0,imw,imh, GL_RED, GL_UNSIGNED_BYTE, charbuf[i]);
		imw = imw >> 1;
		imh = imh >> 1;
	}

	return; 
};

// ============== DISPLAY ROUTINES =========================


/*! 
\fn  void GpuVis::displayVideoMemory(int chan)
\brief Display Routines : show channel \a chan of the Texture Memory Layout.
*/
void GpuVis::displayVideoMemory(int chan, int winW, int winH)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayPyramids(winW, winH, passChan_Kernel,red_Kernel, blue_Kernel);
	return;
}

/*! 
\fn  void GpuVis::displayCornerness(int chan)
\brief Display Routines : shows the Cornerness buffer.
*/
void GpuVis::displayCornerness(int chan)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayCornerness(passChan_Kernel, red_Kernel);
	return;
}

/*! 
\fn  void GpuVis::displayOrigFrame(int chan)
\brief Display Routines : shows the Original buffer.
*/
void GpuVis::displayOrigFrame(int chan)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayOrigFrame(passChan_Kernel);
	return;
}

/*! 
\fn  void GpuVis::displayUndistortedFrame(int chan, int winw, int winh, int trcount, int ftcount)
\brief Display Routines : shows the Cornerness buffer.
*/
void GpuVis::displayUndistortedFrame(int chan, int winw, int winh, int maxcount, int trcount, int ftcount, int adcount)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayUndistortedFrame(white_Kernel, passChan_Kernel, winw, winh, avgtime, maxcount, trcount, ftcount, adcount);
	return;
}


/*! 
\fn  	void GpuVis::displayDistortionMap  (int chan)
\brief Display Routines : shows the Original buffer.
*/
void GpuVis::displayDistortionMap  (int chan)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayDistortionMap(passChan_Kernel);
	return;
}


/*! 
\fn  void GpuVis::displayPyramidBlock	 (int aux,int level, int chan)
\brief Display Pyramid Block
*/
void GpuVis::displayPyramidBlock	 (int aux,int level, int chan)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayPyramidBlock(aux, dm->getCurrentFrameIndex() , level, passChan_Kernel);
	return;	

}


/*! 
\fn  void GpuVis::displayFtList(GpuKLT_FeatureList *list, int imgW, int imgH, int winW, int winH)
\brief Display Feature list.
*/
void GpuVis::displayFtList(GpuKLT_FeatureList *list, int imgW, int imgH, int winW, int winH)
{
	blue_Kernel->activate();
	list->drawTracks(_texTarget, imgW, imgH, winW, winH);
	blue_Kernel->deactivate();
	red_Kernel->activate();
	list->drawPoints(_texTarget, imgW, imgH, winW, winH);
	red_Kernel->deactivate();
	return;
}


/*! 
\fn  void GpuVis::displayFeatureTable     (int chan)
\brief Display Feature list Texture Unit
*/
void GpuVis::displayFeatureTable     (int chan)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayFeatureTable(passChan_Kernel);
	return;
}


/*! 
\fn void GpuVis::displayFloatingWindow   (int chan)
\brief Display Floating Window Texture Unit
*/
void GpuVis::displayFloatingWindow   (int chan)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayFloatingWindow(passChan_Kernel);
	return;
}

/*! 
\fn void GpuVis::displayPartialSum  (int chan)
\brief Display Partial Sum Texture Unit
*/
void GpuVis::displayPartialSum  (int chan)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayPartialSum(passChan_Kernel);
	return;
}

/*! 
\fn void GpuVis::displayMatrixElem   (int chan)
\brief Display Matrix Elem Texture Unit
*/
void GpuVis::displayMatrixElem   (int chan)
{
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayMatrixElem(passChan_Kernel);
	return;
}


/*! 
\fn void GpuVis::displayDebugTable		 ()
\brief Display Debug Table
*/
void GpuVis::displayDebugTable		 ()
{
	int chan = 0;
	cgGLSetParameter1f(passChan_Param, chan);
	dm->displayDebugTable(passChan_Kernel);
	return;

}
