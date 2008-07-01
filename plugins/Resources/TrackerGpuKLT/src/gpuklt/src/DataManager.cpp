
// --------------------------------------------------------------------------
/*! \file DataManager.cpp
* \brief DataManager Class File. 
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

#include <iostream>
#include <assert.h>
#include <math.h>
#include <vector>
#include <windows.h>

#include "DataManager.h"

#include "utils.h"

#include <GL/glew.h>

TextureUnit::TextureUnit(int w, int h, GLenum texTarget, GLenum internalFormat, GLenum texFormat, GLenum datatype, GLenum filterType)
{
	_w                 = w;
	_h                 = h;
	_texTarget         = texTarget;
	_texInternalFormat = internalFormat;
	_texFormat		   = texFormat;
	_texDataType       = datatype;
	_texFilterType     = filterType;

	CheckErrorsGL("DM::initTextureUnits 00");

	// Create Input Image Texture
	glGenTextures(1,&_texID);
	glBindTexture   (_texTarget,_texID);
	
	CheckErrorsGL("DM::initTextureUnits 01");

	glPixelStorei   (GL_UNPACK_ALIGNMENT , 1);
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB);  
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB); 

	CheckErrorsGL("DM::initTextureUnits 02");

	glTexParameteri (_texTarget, GL_TEXTURE_MAG_FILTER, _texFilterType);
	glTexParameteri (_texTarget, GL_TEXTURE_MIN_FILTER, _texFilterType);

	CheckErrorsGL("DM::initTextureUnits 03");

	CheckErrorsGL("DM::initTextureUnits 04");

	glTexImage2D    (_texTarget, 0, _texInternalFormat,_w,_h, 0, _texFormat, datatype, 0);	

	CheckErrorsGL("DM::initTextureUnits 05");
}



void TextureUnit::resetTextureUCHAR(unsigned char *data)
{
	static bool firstTime = true;
	glBindTexture   (_texTarget, _texID);
	glPixelStorei   (GL_UNPACK_ALIGNMENT , 1);
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB); 
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB);  
	glTexParameteri (_texTarget, GL_TEXTURE_MAG_FILTER, _texFilterType);
	glTexParameteri (_texTarget, GL_TEXTURE_MIN_FILTER, _texFilterType);

	if (firstTime)
		glTexImage2D    (_texTarget, 0, _texInternalFormat,_w,_h, 0, _texFormat, _texDataType, data);
	else
		glTexSubImage2D (_texTarget, 0, 0, 0, _w,_h , _texFormat, _texDataType, data);
	
	firstTime = false;
	return;
}



void TextureUnit::resetTextureFLOAT(float *data)
{
	glBindTexture   (_texTarget, _texID);
	glPixelStorei   (GL_UNPACK_ALIGNMENT , 1);
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB);
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB);
	glTexParameteri (_texTarget, GL_TEXTURE_MAG_FILTER, _texFilterType);
	glTexParameteri (_texTarget, GL_TEXTURE_MIN_FILTER, _texFilterType);
	glTexImage2D    (_texTarget, 0, _texInternalFormat,_w,_h, 0, _texFormat, _texDataType, data);
}


FrameUnit::FrameUnit (GLenum arch, int w, int h, int n, GLenum texTarget, GLenum internalFormat)
{
	int wid, ht;
	GLenum interpolator;
	wid   = w; ht = h;
	_nlev = n;
	
	if (arch == GPU_ATI)
		interpolator = GL_NEAREST;
	else if (arch == GPU_NVIDIA)
		interpolator = GL_NEAREST;

	for (int i=0;i<_nlev;i++)
	{
		mainBuf[i] = new TextureUnit(wid, ht, texTarget, internalFormat, GL_RGBA, GL_FLOAT, interpolator);
		auxiBuf[i] = new TextureUnit(wid, ht, texTarget, internalFormat, GL_RGBA, GL_FLOAT, interpolator);
		
		wid = wid >> 1;
		ht  = ht  >> 1;
	}

}




/*!
\fn DataManager::DataManager(int w, int h, int nframes, int nlevels, int kltsubsampling, int kltpyrlevels, int kltnumfeats, int kltiterations )
\brief  Constructor : tell it how much storage to allocate
This routine sets various attributes passed in and also initializes the texture units to be used with FBO's.
\param w,h          : size of orig image
\param nframes      : how many consecutive frames to store on graphics card.
\param nlevels      : num pyramid levels.
\param kltsubsampling: Subsampling in the KLT Pyramid.
\param kltpyrlevels : num pyramid levels in KLT Pyramid
\param kltnumfeats  : num features for GPU KLT Tracking.
\param kltiterations: num iterations within track( ) for each pyramid level.
*/
DataManager::DataManager(int arch, int w, int h, int nframes, int nlevels, int kltsubsampling, int kltpyrlevels, int kltnumfeats, int kltiterations )
				
					:_image_w(w),
					 _image_h(h),
					 _nFrames(nframes),
					 _nLevels(nlevels),
					 _kltSubSampling(kltsubsampling),
					 _kltPyramidLevels(kltpyrlevels),
					 _nkltIterations(kltiterations),
					 _maxFeatureTableSize(kltnumfeats),
					 //_fbo(), // create a new frame buffer object
					 _fbo1(),
					 _fbo2(),
					 _rb()   // optional: create a new render buffer object
{

	_arch			= arch;
	_boundary		= _TEXTURE_BORDER_WIDTH;
	_currFrameIndex = -1;
	_prevFrameIndex = -1;

	// Verify the supported texture sizes !?
	_maxTexWidth	= _MAX_TEXTURE_WIDTH;
	_maxTexHeight	= _MAX_TEXTURE_HEIGHT;
	while ( !querySupportedTextureSize(GL_RGBA, GL_FLOAT, _maxTexWidth, _maxTexHeight))
	{
		_maxTexWidth  /= 2;
		_maxTexHeight /= 2;
	}


	// Initialize the Frame Buffer Objects
	for (int i=0;i<nlevels;i++)
	{
		_fbos[i] = new FramebufferObject();
	}

	// As of now, we will also have only 2 video frame-slots in GPU memory.
	assert(_nFrames==2);

	// This initializes all the storage in GPU memory.
	initTextureUnits(_image_w, _image_h); 

}	




/*!
\fn void DataManager::initTextureUnits(int w, int h) 
\brief Read in Input Image and create the appropriate texture object
\param w : Input Image Width.
\param h : Input Image Height
*/
void DataManager::initTextureUnits(int w, int h) 
{

	switch(_arch)
	{
	case GPU_ATI:
		_texTarget = GL_TEXTURE_2D;
		_texInternalFormat = GL_RGBA_FLOAT16_ATI;
		break;
	case GPU_NVIDIA:
		_texTarget = GL_TEXTURE_RECTANGLE_NV;
		_texInternalFormat = GL_FLOAT_RGBA32_NV;
		break;
	}
	CheckErrorsGL("DM::initTextureUnits 1");

	//////////////// Initialize Input Texture ////////////////
	_inputImageTU    = new TextureUnit(w, h, _texTarget, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR);

	CheckErrorsGL("DM::initTextureUnits 2");


	//////////////// Initialize Distortion Texture Map ///////
	_distortionMapTU = new TextureUnit(_image_w, _image_h, _texTarget, _texInternalFormat, GL_RGBA, GL_FLOAT, GL_NEAREST);
	
	CheckErrorsGL("DM::initTextureUnits 3");

	//////////////// Generate a Distortion Map ////////////////
	float * distortionmap = new float [ _image_w * _image_h * 4];	
	float WMax = 1.0/(1.0* _image_w);
	float HMax = 1.0/(1.0* _image_h);
	int i=0;
	for (int k=0; k< _image_h; k++)
	{
		for (int j=0; j < _image_w; j++)
		{
			float del_x        = WMax * (j - _image_w/2);
			float del_y        = HMax * (k - _image_h/2);
			float r            = sqrt( del_x * del_x + del_y * del_y);
			float rdist        = r * ( 1 + 0.05 * r*r); //0.095 
			if (r==0.0)
			{	del_x = 0.0; del_y = 0.0; }
			else			   
			{ del_x			   = rdist * ( del_x / r ) - del_x ;
			  del_y			   = rdist * ( del_y / r ) - del_y ;
			}
			distortionmap[i++] = - del_x + 0.5 ; 
			distortionmap[i++] = - del_y + 0.5 ;
			distortionmap[i++] = 0.0;
			distortionmap[i++] = 0.0;
		}
	}

	//////////////// Update Distortion Texture Map ///////
	_distortionMapTU->resetTextureFLOAT(distortionmap);

	CheckErrorsGL("DM::initTextureUnits 4");

	//////////////// Initialize Pyramid Storage ///////

	_pyramid[0] = new FrameUnit (_arch, _image_w, _image_h, _nLevels, _texTarget, _texInternalFormat);
	_pyramid[1] = new FrameUnit (_arch, _image_w, _image_h, _nLevels, _texTarget, _texInternalFormat);

	CheckErrorsGL("DM::initTextureUnits 5");

	//////////////// Initialize KLT Data Struct Storage ///////       

	// For FBO Efficiency, ALL TEXTURES UNITS ARE SAME SIZE, (only then FBO's can have multiple attachments)
	int N = _maxFeatureTableSize;

	_kltCornernessTU1      = new TextureUnit(_image_w, _image_h, _texTarget, _texInternalFormat, GL_RGBA, GL_FLOAT, GL_NEAREST);
	_kltCornernessTU2      = new TextureUnit(_image_w, _image_h, _texTarget, _texInternalFormat, GL_RGBA, GL_FLOAT, GL_NEAREST);
    _kltFeaturesTU		   = new TextureUnit(    N   , 100  , _texTarget, _texInternalFormat, GL_RGBA, GL_FLOAT, GL_NEAREST);               
    _kltFloatingWindowTU   = new TextureUnit(    N   , 100  , _texTarget, _texInternalFormat, GL_RGBA, GL_FLOAT, GL_NEAREST);          
    _kltPartialSumTU       = new TextureUnit(    N   , 100  , _texTarget, _texInternalFormat, GL_RGBA, GL_FLOAT, GL_NEAREST);       
    _kltMatrixElemTU	   = new TextureUnit(    N   , 100  , _texTarget, _texInternalFormat, GL_RGBA, GL_FLOAT, GL_NEAREST);

	debugTable			   = new TextureUnit(  100, 7*20  , _texTarget, _texInternalFormat, GL_RGBA, GL_FLOAT, GL_NEAREST);

	_xSize				   = N;
	_ySize				   = 100;
	switch(_arch)
	{
		case GPU_ATI:
			_xOffset			   = 1.0 / (1.0 * N);
			_yOffset			   = 1.0 / 100.0;
			break;
		case GPU_NVIDIA:
			_xOffset			   = 1.0 ;
			_yOffset			   = 1.0 ;
			break;
	}

	CheckErrorsGL("DM::initTextureUnits 6");

	// Buffer Data used to upload feature coordinate list to GPU at the beginning of call to trackFeatures()
	data          = new float [ _xSize * _ySize * 4 ];


	//Create the shadow map texture
	glGenTextures	(1, &depthTexture);
	glBindTexture	(_texTarget, depthTexture);
	glTexParameteri	(_texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri	(_texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri	(_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri	(_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D	(_texTarget, 0, GL_DEPTH_COMPONENT, _image_w, _image_h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	return ;
}


/*!
  \fn void   DataManager::resetInputTexture(unsigned char *imagedata)
  \brief This routine resets the input texture from the Image object
  \param imagedata: 8-bit Image data (Next Frame in Image Sequence)
*/
void   DataManager::resetInputTexture(unsigned char *imagedata)
{
	_inputImageTU->resetTextureUCHAR(imagedata);
}


/*!
  \fn void  DataManager::uploadFeatureTable(GpuKLT_FeatureList *ftTable)
  \brief This routine uploads the Feature Table into GPU memory.
  \param ftTable: Feature list which contains data to be uploaded into GPU memory.
*/
void  DataManager::uploadFeatureTable(GpuKLT_FeatureList *ftTable)
{
	
	int       i, c;
	static bool firstTime = true;
	// Fill it up with 1's (indicating INVALID FEATURE RECORDS)
	for (i=0;i< (_xSize * _ySize * 4); i++)
		data[i] = 1.0;

	// Copy features into Row 0 of Feature Table Texture Unit.
	c = 0;
	for (i=0;i< ftTable->_nFeats ;i++)
	{
		if (ftTable->_list[i]->valid)
		{
			float fsx = ftTable->_list[i]->normx;
			float fsy = ftTable->_list[i]->normy;
			data[c++] = fsx ;		data[c++] = fsy ;
			data[c++] = fsx ;		data[c++] = fsy ;
		}
		if (c >= (_maxFeatureTableSize * 4))
			break;
	}

	// Bind the appropriate texture unit and upload 'data' into it.
	glBindTexture   (_texTarget, _kltFeaturesTU->_texID);
	glPixelStorei   (GL_UNPACK_ALIGNMENT , 1);
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB); 
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB); 
	glTexParameteri (_texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (_texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	if (firstTime)
		glTexImage2D    (_texTarget, 0, _texInternalFormat,_xSize, _ySize, 0, GL_RGBA, GL_FLOAT, data);
	else
		glTexSubImage2D (_texTarget, 0, 0, 0, _xSize, _nkltIterations * _kltPyramidLevels , GL_RGBA, GL_FLOAT, data);

	if (firstTime)
	{
		// TEST THE FLOATING WINDOW texture unit WITH A 7x7 CHECKER BOARD PATTERN.
		float *data1  = new float [  _xSize * _ySize * 4 ];
		int    stride = (_maxFeatureTableSize / 10);	
		int    xFeat, yFeat, i;

		for (i=0;i< (_xSize * _ySize * 4); i++)
			data1[i] = 1.0;

		for (i=0;i< _maxFeatureTableSize; i++)
		{
			int nx = (i % stride);
			int ny = (i / stride);
			xFeat = 7 * nx;
			yFeat = 7 * ny;
			if ((nx%2) && !(ny%2)) continue;
			if (!(nx%2) && (ny%2)) continue;

			for (int a=0; a<7; a++)	{
				for (int b=0; b<7; b++)	{
					c = (yFeat + b) * 4*_xSize + 4*(xFeat + a);
					data1[c++] = 0.0;	data1[c++] = 0.0;	data1[c++] = 0.0;	data1[c++] = 0.0;
				} // a
			} // b
		} // i

		glBindTexture   (_texTarget, _kltFloatingWindowTU->_texID);
		glPixelStorei   (GL_UNPACK_ALIGNMENT , 1);
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB); 
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB); 
		glTexParameteri (_texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (_texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D    (_texTarget, 0, _texInternalFormat, _xSize, _ySize, 0, GL_RGBA, GL_FLOAT, data1);
	
		firstTime = false;
		delete data1;

	} // firsttime
	
	return;
}





/*! 
\fn void DataManager::displayDistortionMap(Kernel *kernel) 
\brief Display the input frame
\param kernel : Use this fragment program object to display the texture memory
*/
void DataManager::displayDistortionMap(Kernel *kernel) 
{
	int w    = _image_w ;  	
	int h    = _image_h ;
	kernel->activate();
	kernel->bindInput(_distortionMapTU->_texID);
	kernel->setRange(w,(int)(1.333*h));
	if (_arch ==GPU_ATI)
		kernel->execute(h,0,0,w,0.0, 0.0, 1.0, 1.0);
	else if (_arch == GPU_NVIDIA)
		kernel->execute(h,0,0,w,0.0, 0.0, h, w);
	kernel->deactivate();
}

/*! 
\fn void DataManager::displayFeatureTable(Kernel *kernel) 
\brief Display the input frame
\param kernel : Use this fragment program object to display Feature Table
*/
void DataManager::displayFeatureTable(Kernel *kernel) 
{
	kernel->activate();
	kernel->bindInput(_kltFeaturesTU->_texID);
	int hh = _nkltIterations * _kltPyramidLevels + 1;
	kernel->setRange(_xSize, hh);
	if (_arch == GPU_ATI)
		kernel->execute(hh, 0, 0 , _xSize, 0.0, 0.0, (1.0*hh)/(1.0*_ySize), 1.0);	
	else if (_arch == GPU_NVIDIA)
		kernel->execute(hh, 0, 0 , _xSize, 0.0, 0.0, hh, _xSize);	
		
	kernel->deactivate();
}

/*! 
\fn void DataManager::displayFloatingWindow(Kernel *kernel) 
\brief Display the input frame
\param kernel : Use this fragment program object to display the Floating Window Texture Unit
*/
void DataManager::displayFloatingWindow(Kernel *kernel) 
{
	kernel->activate();
	kernel->bindInput(_kltFloatingWindowTU->_texID);
	kernel->setRange(0.7*_xSize, 0.7*_ySize);
	if (_arch == GPU_ATI)
		kernel->execute(0.7*_ySize, 0, -0.3*_ySize, _xSize, 0.0, 0.0, 1.0, 1.0);
	else if (_arch == GPU_NVIDIA)
		kernel->execute(0.7*_ySize, 0, -0.3*_ySize, _xSize, 0.0, 0.0, _ySize, _xSize);
	kernel->deactivate();
}


/*! 
\fn void DataManager::displayPartialSum   (Kernel *kernel)
\brief Display the input frame
\param kernel : Use this fragment program object to display the Partial Sum Texture Unit
*/
void DataManager::displayPartialSum   (Kernel *kernel)
{

	kernel->activate();
	kernel->bindInput(_kltPartialSumTU->_texID);
	int hh = 14;
	kernel->setRange(_xSize, hh);
	if (_arch == GPU_ATI)
		kernel->execute(hh, 0, 0 , _xSize, 0.0, 0.0, (1.0*hh)/(1.0*_ySize), 1.0);
	else if (_arch == GPU_NVIDIA)
		kernel->execute(hh, 0, 0 , _xSize, 0.0, 0.0, hh, _xSize);
	kernel->deactivate();
}


/*! 
\fn void DataManager::displayMatrixElem   (Kernel *kernel)
\brief Display the input frame
\param kernel : Use this fragment program object to display the Matrix Element Texture Unit
*/
void DataManager::displayMatrixElem   (Kernel *kernel)
{
	kernel->activate();
	kernel->bindInput(_kltMatrixElemTU->_texID);
	int hh = 3;
	kernel->setRange(_xSize, hh);
	if (_arch == GPU_ATI)
		kernel->execute(hh, 0, 0 , _xSize, 0.0, 0.0, (1.0*hh)/(1.0*_ySize), 1.0);
	else if (_arch == GPU_NVIDIA)
		kernel->execute(hh, 0, 0 , _xSize, 0.0, 0.0, hh, _xSize);
	kernel->deactivate();
}


/*! 
\fn void DataManager::displayPyramids(int winW, int winH, Kernel *kernel, Kernel *red, Kernel *blue)
\brief Display the Pyramid Data Structure on the GPU.
\param winW, winH : Window dimensions.
\param kernel,red,blue : Use these fragment programs to display the texture memory
*/
void DataManager::displayPyramids(int winW, int winH, Kernel *kernel, Kernel *red, Kernel *blue)
{

	int block_w = (_image_w + 2 * _boundary);
	int block_h = (_image_h + 2 * _boundary);
	int tex_w =  block_w * 3;
	int tex_h =  block_h * 2;

	int wtop, wbot, wleft, wright;

	int framePos[2][2];

	framePos[0][0] = 0; framePos[0][1] = -tex_h;
	framePos[1][0] = 0; framePos[1][1] = 0;

	int   auxix, auxiy, currx, curry, currb, currw, currh, factor;

	kernel->activate();			
	kernel->setRange(tex_w,tex_h*2);
	
	// Set up records for two successive frames.
	for (int i=0;i<_nFrames;i++) 
	{
		currx   = framePos[i][0];
		curry   = framePos[i][1];
		auxix   = tex_w - block_w;
		auxiy   = curry;

		for (int j=0; j<_nLevels ; j++) 
		{	
				factor  = pow(2.0,j);
				currb   = _boundary / factor ;
				currw   = _image_w / factor;
				currh   = _image_h / factor;

				wtop  = tex_h - (curry + currb);
				wleft = currx + currb;
				wbot  = wtop  - currh;
				wright= wleft + currw;

				kernel->bindInput( getPyramidBlock( i, j, _MAIN_BUF)->_texID );
				if (_arch == GPU_ATI)
					kernel->execute(wtop, wleft, wbot, wright,0.0,0.0,1.0, 1.0);
				else if (_arch == GPU_NVIDIA)
					kernel->execute(wtop, wleft, wbot, wright, 0.0, 0.0, currh, currw);

				wtop  = tex_h - (auxiy + currb);
				wleft = auxix + currb;
				wbot  = wtop  - currh;
				wright= wleft + currw;
					
				kernel->bindInput( getPyramidBlock( i, j, _AUXI_BUF)->_texID );
				if (_arch == GPU_ATI)
					kernel->execute(wtop, wleft, wbot, wright,0.0,0.0,1.0, 1.0);
				else if (_arch == GPU_NVIDIA)
					kernel->execute(wtop, wleft, wbot, wright, 0.0, 0.0, currh, currw);

				currx+= (currw   + 2*currb/2);
				auxix-= (currw/2 + 2*currb/2);
				auxiy+= (currh/2 + 2*currb/2);
		}
	}

	kernel->deactivate();

	red->activate();
	glDisable(_texTarget);
    glLineWidth(1.0);
	glBegin(GL_LINE_LOOP);
		glVertex2f(-0.998,0.998);	glVertex2f(0.998,0.998); glVertex2f(0.998,0.002);    glVertex2f(-0.998,0.002);
	glEnd();	
	char framestring[16];
	sprintf(framestring,"Frame 0");
	DrawText (10, 10 + winH  - 0.5*winH , framestring, 1, 1, 1);
	red->deactivate();
	blue->activate();
	glLineWidth(1.0);
	glBegin(GL_LINE_LOOP);
		glVertex2f(-0.998,-0.002);	glVertex2f(0.998,-0.002); glVertex2f(0.998,-0.998);    glVertex2f(-0.998,-0.998);
	glEnd();
	sprintf(framestring,"Frame 1");
	DrawText (10, 10 , framestring, 1, 1, 1);
	blue->deactivate();

}

/*! 
\fn void DataManager::displayPyramidBlock (int aux, int frame, int level, Kernel *kernel)
\brief Display Each Pyramid Block
\param aux: 0 or 1
\param frame: 0 or 1 
\param kernel: Use thisfragment program to display the texture memory
*/
void DataManager::displayPyramidBlock (int aux, int frame, int level, Kernel *kernel)
{
	
	int w    = _image_w ;  	
	int h    = _image_h ;
	int factor = pow(2.0,level);
	TextureUnit *tu;

	w /= factor;
	h /= factor;
	if (aux ==0)
		tu = _pyramid[frame]->mainBuf[level];
	else 
		tu = _pyramid[frame]->auxiBuf[level];

	kernel->activate();
	kernel->bindInput(tu->_texID);
	kernel->setRange(1.5*_image_w,2*_image_h);

	if (_arch == GPU_ATI)
		kernel->execute(_image_h,0,0,_image_w,0.0, 0.0, 1.0, 1.0);
	else if (_arch == GPU_NVIDIA)
		kernel->execute(_image_h,0,0,_image_w,0.0, 0.0, h, w);
 
	if (level > 0)
	{
		if (_arch == GPU_ATI)
			kernel->execute(h,_image_w,0,_image_w + w,0.0, 0.0, 1.0, 1.0);
		else if (_arch == GPU_NVIDIA)
			kernel->execute(h,_image_w,0,_image_w + w,0.0, 0.0, h, w);
	}

	kernel->deactivate();
	return;
}


/*! 
\fn void DataManager::displayCornerness(Kernel *kernel, Kernel *colorkernel)
\brief Display the Cornerness Texture Unit
\param kernel, colorkernel : Use this fragment program object to display the texture memory
*/
void DataManager::displayCornerness(Kernel *kernel, Kernel *colorkernel) 
{
	int w = _image_w;
	int h = _image_h;
	
	kernel->activate();
	kernel->bindInput(getCornerness2TextureUnit()->_texID );
	kernel->setRange(w,(int)(1.333*h));
 
	if (_arch == GPU_ATI)
		kernel->execute(h,0,0,w,0.0,0.0,1.0,1.0);
	else if (_arch == GPU_NVIDIA)
		kernel->execute(h,0,0,w,0.0,0.0,h,w);
 
	kernel->deactivate();
	
	glDisable(_texTarget);
	colorkernel->activate();
	glColor3f(0.0,0.0,0.0); glLineWidth(1.0);
	glBegin(GL_LINE_LOOP);
		glVertex2f(-0.998,-0.998);	glVertex2f(0.998,-0.998 ); glVertex2f(0.99802, 0.5);    glVertex2f(-0.998,0.5);
	glEnd();
	colorkernel->deactivate();
}

/*! 
\fn void  DataManager::displayOrigFrame(Kernel *kernel) 
\brief Display the input frame
\param kernel : Use this fragment program object to display the texture memory
*/
void DataManager::displayOrigFrame(Kernel *kernel) 
{
	int w = _image_w;
	int h = _image_h;
	kernel->activate();
	kernel->bindInput(_inputImageTU->_texID);
	kernel->setRange(w,(int)(1.333*h));
 
	if (_arch == GPU_ATI)
		kernel->execute(h,0,0,w,0.0,0.0,1.0,1.0);
	else if (_arch == GPU_NVIDIA)
		kernel->execute(h,0,0,w,0.0,0.0,h,w);
 
	kernel->deactivate();
}

/*! 
\fn void DataManager::displayDebugTable(Kernel *kernel)
\brief Display the debug table
\param kernel : Use this fragment program object to display the texture memory
*/
void DataManager::displayDebugTable(Kernel *kernel)
{	
	int w = _image_w;
	int h = _image_h;

	kernel->activate();
	kernel->bindInput(debugTable->_texID);
	kernel->setRange(w,(int)(1.333*h));
	if (_arch == GPU_ATI)
		kernel->execute(1.2*h, 0.1*w, -0.3*h,w*.75,0.0,0.0,1.0,1.0);
	else if (_arch == GPU_NVIDIA)
		kernel->execute(1.2*h, 0.1*w, -0.3*h,w*.75,0.0,0.0,h/6,w/12);

	kernel->deactivate();

}



/*! 
\fn void DataManager::displayUndistortedFrame(Kernel *k1, Kernel *kernel, int winw, int winh, float time, int trcount, int ftcount, int ftadded)
\brief Display the undistorted frame
\param k1, kernel: Use this fragment program object to display the texture memory
\param winw, winh: window dimensions 
\param time: timing per frame
\param ftcount: how many feature to be tracked
\param trcount: how many features were tracked
\param ftadded: how many features were re-injected
*/
void DataManager::displayUndistortedFrame(Kernel *k1, Kernel *kernel, int winw, int winh, float time, int maxcount, int trcount, int ftcount, int ftadded)
{

	int w = _image_w;
	int h = _image_h;

	char timestring[1024];

	sprintf(timestring,"%d x %d video, Time: %8.3lf msec,  Features: [MAX %5d]  (Tracked %5d of %5d)  (Added %5d)", _image_w, _image_h, time, maxcount, trcount, ftcount, ftadded);

	k1->activate();
	DrawText (0.05*winw, 0.9 * winh, timestring, 1.0, 1.0, 1.0);
	k1->deactivate();

	kernel->activate();

	kernel->bindInput(getPyramidBlock( getCurrentFrameIndex() , 0, _MAIN_BUF)->_texID);

	kernel->setRange(w,(int)(1.333*h));
	 
	if (_arch == GPU_ATI)
		kernel->execute(h,0,0,w,0.0,0.0,1.0,1.0);
	else if (_arch == GPU_NVIDIA)
		kernel->execute(h,0,0,w,0.0,0.0,h,w);
 
	kernel->deactivate();

	glDisable(_texTarget);
	glColor3f(1.0,0.0,0.0); glLineWidth(1.0);

	glBegin(GL_LINE_LOOP);
		glVertex2f(-0.998,-0.998);	glVertex2f(0.998,-0.998 ); glVertex2f(0.99802, 0.5);    glVertex2f(-0.998,0.5);
	glEnd();


}



/*! \fn void DataManager::toggleFrames()
\brief Toggle between Frame 0 and 1 for successive frames. Frame n overwrites frame (n-2)
*/
void DataManager::toggleFrames()
{
	if (_currFrameIndex == -1)
	{
		_currFrameIndex = 0;
	}
	else
	{
		_prevFrameIndex = _currFrameIndex;
		_currFrameIndex = 1 - _currFrameIndex;
	}
}


/*! \fn TextureUnit *	DataManager::getPyramidBlock(int frameIndex, int level,int which_plane)
\brief Return the particular Texture unit asked for
\param frameIndex: 0 or 1
\param level: 0, 1, 2 .. maxlevel-1
\param which_plane: main storage or auxiliary storage
*/
TextureUnit *	DataManager::getPyramidBlock(int frameIndex, int level,int which_plane)
{
	if (which_plane == _MAIN_BUF)
	{
		return _pyramid[ frameIndex ]->mainBuf[ level ];
	}
	else if (which_plane == _AUXI_BUF)
	{
		return _pyramid[ frameIndex ]->auxiBuf[ level ];
	}
	else
		return NULL;
}
