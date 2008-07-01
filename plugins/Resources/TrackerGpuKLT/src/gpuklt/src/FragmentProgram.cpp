
// --------------------------------------------------------------------------
/*! \file FragmentProgram.cpp
* \brief FragmentProgram Class File. 
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
#include <windows.h>

#include "FragmentProgram.h"
#include "utils.h"


/*!
*	\fn FragmentProgram::FragmentProgram(  int       arch,   
									   GLenum    texTarget, 
									   CGcontext      fContext, 
                                       CGprofile fProfile, 
                                       char     *FPsource, 
                                       GLuint    textureID,
									   bool      programInFile )
*	\brief Constructor
*	\param fContext : CG Context obtained on startup
*	\param fProfile : CG Profile obtained on startup
*	\param FPsource : Fragment Shader CG Code.
*   \param textureID: texture ID associated with this fragment program during initialization
*	\param programInFile : If true, indicates program is to be read from file.
*/ 
FragmentProgram::FragmentProgram(      int       arch,   
									   GLenum    texTarget, 
									   CGcontext fContext, 
                                       CGprofile fProfile, 
                                       char     *FPsource, 
                                       GLuint    textureID,
									   bool      programInFile )
{
		
	if (arch == GPU_ATI)
	{
		if (programInFile) 
			_FP = cgCreateProgramFromFile(fContext,
                               CG_SOURCE, FPsource,
                               fProfile, NULL,NULL);
		else 
			_FP = cgCreateProgram(fContext, CG_SOURCE, FPsource,
							   fProfile, NULL, NULL);
	}
	else if (arch == GPU_NVIDIA)
	{
		// XYZ TRIED PASSING COMPILER OPTIONS - but did not work !
		// const char* args[] = {  "-nofastprecision", "-nofastmath", 0 };  
		if (programInFile) 
			_FP = cgCreateProgramFromFile(fContext,
                               CG_SOURCE, FPsource,
                               fProfile, NULL, NULL); //XYZ "main", args);
		else 
			_FP = cgCreateProgram(fContext, CG_SOURCE, FPsource,
							   fProfile, NULL, NULL); //XYZ "main", args);

	}
  cgGLLoadProgram(_FP);

  printf("%s\n\n", cgGetErrorString(cgGetError()));

  _FPtexture  = cgGetNamedParameter (_FP, "texture");
  _FPtexture1 = cgGetNamedParameter (_FP, "texture1");
  _FPtexture2 = cgGetNamedParameter (_FP, "texture2");
  if (_FPtexture == 0) {
		printf("FragmentProgram ERROR : Texture param acquisition failed!");
		exit (1);
  }
 
  _texTarget	  = texTarget;
  _inputTextureID = textureID; 
  _running		  = true;
  _FPcontext	  = fContext;
  _FPprofile	  = fProfile;
  _FPkernel1	  = NULL;
  _FPkernel2	  = NULL;

}



/*! 
*	\fn void FragmentProgram::getKernelParams()
*	\brief Fetch the Cg Parameter Handles for the Kernel Array.
*/
void FragmentProgram::getKernelParams()
{
	_FPkernel1     = cgGetNamedParameter(_FP, "kernel1");
	if (!_FPkernel1 ) {
		printf("Fragment Program ERROR : Kernel array param acquisition failed!");
		exit(1);
	}
	_FPkernel2     = cgGetNamedParameter(_FP, "kernel2");
	if (!_FPkernel2 ) {
		printf("Fragment Program ERROR : Kernel array param acquisition failed!");
		exit(1);
	}
	
}
  

void FragmentProgram::getOffsetParam()
{
	_FPoffset     = cgGetNamedParameter(_FP, "offset");
	if (!_FPoffset ) {
		printf("Fragment Program ERROR : Offset param acquisition failed!");
		exit(1);
	}
}
  


/*! 
*	\fn void FragmentProgram::setParams(int n, float *kernel1, float * kernel2, float offset)
*	\param n : size of kernel footprint
*	\param kernel1, kernel2 : array of values
*   \param offset : not used any more ..
*/
void FragmentProgram::setParams(int n, float *kernel1, float * kernel2, float offset)
{
	cgGLSetParameterArray1f(_FPkernel1,0,n,kernel1);
	cgGLSetParameterArray1f(_FPkernel2,0,n,kernel2);
}

/*! 
*	\fn void FragmentProgram::activate()
*	\brief Activate the Fragment Shader
*/
void FragmentProgram::activate()
{

  cgGLEnableProfile(_FPprofile);
  cgGLBindProgram(_FP);
  _running = true;

}

/*! 
*	\fn void FragmentProgram::bind() 
*	\brief Bind the Fragment Shader Program
*/
void FragmentProgram::bind() 
{ 
	cgGLBindProgram(_FP); 
}

/*! 
*	\fn FragmentProgram::bindTexture(GLuint texID)
*	\brief Bind Texture Input
*	\param texID : texture ID
*/
void FragmentProgram::bindTexture(GLuint texID)
{
	glEnable(_texTarget);
	cgGLSetTextureParameter(_FPtexture, texID);
	cgGLEnableTextureParameter(_FPtexture);
}

/*! 
*	\fn void FragmentProgram::deactivate()
*	\brief Deactivate the Fragment Shader
*/
void FragmentProgram::deactivate()
{
  cgGLDisableProfile( _FPprofile );
  cgGLDisableTextureParameter(_FPtexture);
  _running = false;
}

