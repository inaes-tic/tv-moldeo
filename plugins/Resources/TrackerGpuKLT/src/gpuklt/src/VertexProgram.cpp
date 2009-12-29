// --------------------------------------------------------------------------
/*! \file VertexProgram.cpp
* \brief VertexProgram Class File.
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
#include "VertexProgram.h"
#include <iostream>

/*!
*	\fn VertexProgram::VertexProgram( CGcontext      vContext,
                                       CGprofile vProfile,
                                       char     *VPsource,
									   bool      programInFile )
*	\brief Constructor
*	\param vContext : CG Context obtained on startup
*	\param vProfile : CG Profile obtained on startup
*	\param VPsource : Vertex Shader CG Code.
*	\param programInFile : If true, indicates program is to be read from file.
*/
VertexProgram::VertexProgram( CGcontext      vContext,
                                       CGprofile vProfile,
                                       char     *VPsource,
									   bool      programInFile )
{
  if (programInFile)
	  _VP = cgCreateProgramFromFile(vContext,
                               CG_SOURCE, VPsource,
                               vProfile, NULL,NULL);
  else
	  _VP = cgCreateProgram(vContext, CG_SOURCE, VPsource,
							   vProfile, NULL, NULL);

  cgGLLoadProgram(_VP);
  printf("Vertex program was loaded\n");
  _running   = true;
  _VPcontext = vContext;
  _VPprofile = vProfile;

  _VPmodelViewMatrix	= cgGetNamedParameter(_VP, "ModelViewProj");

}

/*!
*	\fn void VertexProgram::init(char *programSource)
*	\brief Creates the Vertex Program object
*	\param programSource : Cg Vertex Shader source code
*/
void VertexProgram::init(char *programSource)
{

	_VP = cgCreateProgramFromFile(_VPcontext,
                               CG_SOURCE,programSource,
                               _VPprofile, NULL,NULL);
	cgGLLoadProgram(_VP);

	_VPmodelViewMatrix	= cgGetNamedParameter(_VP, "ModelViewProj");
}

/*!
*	\fn void VertexProgram::activate()
*	\brief Activate the Vertex Shader
*/
void VertexProgram::activate()
{
  cgGLEnableProfile(_VPprofile);
  cgGLBindProgram(_VP);
  _running = true;
}


/*!
*	\fn void VertexProgram::setMatrices()
*	\brief Set The Modelview Matrix Of Our Shader To Our OpenGL Modelview Matrix
*/
void VertexProgram::setMatrices()
{
  cgGLSetStateMatrixParameter(_VPmodelViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
}

/*!
*	\fn void VertexProgram::deactivate()
*	\brief Deactivate the Vertex Shader
*/
void VertexProgram::deactivate()
{
  cgGLDisableProfile( _VPprofile );
  _running = false;
}

