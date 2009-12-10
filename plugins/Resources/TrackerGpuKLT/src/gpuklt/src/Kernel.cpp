// --------------------------------------------------------------------------
/*! \file Kernel.cpp
* \brief Kernel Class File. 
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
#include <windows.h>

#include "Kernel.h"

#include "..\include\GpuVis.h"

//extern Options opt;

/*!
\fn Kernel::Kernel(int arch, GLenum texTarget, CGcontext fContext, CGprofile fProfile , char * directory, char *FPsource, bool programInFile)
\brief Constructor 
*/
Kernel::Kernel(int arch, GLenum texTarget, CGcontext fContext, CGprofile fProfile , char * directory, char *FPsource, bool programInFile)
{
	char prog[256];
	sprintf(prog,"%s\\%s",directory,FPsource);
	printf("Loading Shader\t\t[%s]\n",prog);
	_texTarget = texTarget;
	fProgram = new FragmentProgram(arch, texTarget, fContext, fProfile, prog, 0, programInFile);
}

/*!
\fn void Kernel::activate() 
\brief Activate Routine
*/
void Kernel::activate() 
{
	swapNeeded = false;
	fProgram->activate();
}

/*!
\fn void Kernel::deactivate() 
\brief De-activate Routine
*/
void Kernel::deactivate()
{
	fProgram->deactivate();
}

/*!
\fn void Kernel::bindInput(GLuint texID)
\brief Bind input
*/
void Kernel::bindInput(GLuint texID)
{
	fProgram->bindTexture(texID);
}


/*!
\fn void Kernel::execute(float vtop, float vleft, float vbot, float vright, 
					     float  top, float  left, float  bot, float  right)
\brief Invoke GPGPU Computation
*/
void Kernel::exec(float vtop, float vleft, float vbot, float vright, 
				  float  top, float  left, float  bot, float  right)
{
	glEnable(_texTarget);
	glBegin (GL_QUADS);
	    glTexCoord2f (  left    ,    bot   ); /* lower left  */
		glVertex2f   ( vleft    ,   vbot   );
        glTexCoord2f ( right    ,    bot   ); /* lower right */
		glVertex2f   (vright    ,   vbot   );
		glTexCoord2f ( right    ,    top   ); /* upper right */
        glVertex2f   (vright    ,   vtop   );
        glTexCoord2f ( left     ,    top   ); /* upper left  */      
		glVertex2f   (vleft     ,   vtop   );
    glEnd ();

	glDisable(_texTarget);
}
	


/*!
\fn void Kernel::execute(float Op_top, float Op_left, float Op_bot, float Op_right, 
						 float Ip_top, float Ip_left, float Ip_bot, float Ip_right)
\brief Invoke GPGPU Computation
*/
void Kernel::execute(float Op_top, float Op_left, float Op_bot, float Op_right, 
					 float Ip_top, float Ip_left, float Ip_bot, float Ip_right)
{
	float vtop,vbot,vleft,vright;
	vtop   = (2.0 * Op_top  ) / ranH - 1 ;
	vbot   = (2.0 * Op_bot  ) / ranH - 1 ;
	vleft  = (2.0 * Op_left ) / ranW - 1 ;
	vright = (2.0 * Op_right) / ranW - 1 ;
	exec(vtop,vleft,vbot,vright,Ip_top,Ip_left,Ip_bot,Ip_right);
}