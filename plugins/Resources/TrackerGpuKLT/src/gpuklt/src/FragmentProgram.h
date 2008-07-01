
// --------------------------------------------------------------------------
/*! \file FragmentProgram.h
* \brief FragmentProgram Class Header File
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

#ifndef _FRAGMENT_PROGRAM_H
#define _FRAGMENT_PROGRAM_H

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include <Cg/cgGL.h>

/*! \class FragmentProgram
    \brief Represents a Cg Fragment Shader for GPGPU.

    Provides an abstraction for the functionality provided by the CG Fragment Shader
*/
/*********************************************************************************/
class FragmentProgram {
 protected:
   bool        _running;                           //!<  Flag indicates program state
   CGparameter _FPtexCoord, _FPtexture, _FPkernel1, _FPkernel2, _FPoffset; //!<  CG Parameter Handles used by different fragment shaders.
   CGprofile   _FPprofile;						   //!<  CG Fragment Program Profile
   CGcontext   _FPcontext;						   //!<  CG Program Context.
   GLuint      _inputTextureID;					   //!<  Current Input Texture Id

 public: // interface
   CGprogram   _FP;								   //!<  The Fragment Program.
   CGparameter _FPtexture1;						   //!<  Some more CG parameters handles
   CGparameter _FPtexture2;                        //!<  Some more CG Parameter handles
   GLenum      _texTarget;	

   //! Constructor 1
   FragmentProgram(CGcontext fContext, CGprofile fProfile) 
   { 
		_FPcontext = fContext;
		_FPprofile = fProfile;
   };

   //! Constructor 2
   FragmentProgram( int arch,
					GLenum texTarget,
					CGcontext fContext, 
                    CGprofile fProfile, 
                    char *FPname, 
                    GLuint texName,
					bool      programInFile );

   //! Destructor
  ~FragmentProgram() { };

   //! Activate the Shader.
   void activate();
   //! Bind Input Texture
   void bindTexture(GLuint texID);
   //! Bind the Shader.
   void bind();
   //! Deactivate the Shader
   void deactivate();
   //! Get Handle for parameters.
   void getKernelParams();
   void getOffsetParam();

   //! Set the parameters.
   void setParams(int n, float *kernel1, float * kernel2, float offset);
};

#endif

