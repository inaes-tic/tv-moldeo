
// --------------------------------------------------------------------------
/*! \file VertexProgram.h
* \brief VertexProgram Class Header File
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


#ifndef _VERTEX_PROGRAM_H
#define _VERTEX_PROGRAM_H

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include <Cg/cgGL.h>

/*! \class VertexProgram
    \brief Represents a Cg Fragment Shader for GPGPU.

    Provides an abstraction for the functionality provided by the CG Vertex Shader
*/
class VertexProgram {
 protected:
   bool        _running;							//!<  Flag indicates program state
   CGprofile   _VPprofile;							//!<  Cg Vertex Shader Profile
   CGcontext   _VPcontext;							//!<  Cg Shader Context
   CGparameter _VPposition, _VPmodelViewMatrix;		//!<  Cg Parameters
 public:
   CGprogram   _VP;									//!<  The Vertex Program 

   //! Constructor;
   VertexProgram(CGcontext fContext, CGprofile fProfile) 
   { 
		_VPcontext = fContext;
		_VPprofile = fProfile;
   };
   //! Destructor
  ~VertexProgram() { };
  
   //! Constructor
   VertexProgram( CGcontext vContext, 
                  CGprofile vProfile, 
                  char     *VPname,
			      bool      programInFile );

   //! Intialize Vertex Shader from Program Source
   void init(char *programSource);
   //! Activate the Vertex Shader
   void activate();
   //! Setup the Matrices to be passed to Vertex Shader
   void setMatrices();
   //! Deactivate the Vertex Shader
   void deactivate();
};

#endif

