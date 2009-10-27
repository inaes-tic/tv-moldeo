/*******************************************************************************

                                moFBO.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
  *                                                                          *
  *   This code is distributed in the hope that it will be useful, but       *
  *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
  *   General Public License for more details.                               *
  *                                                                          *
  *   A copy of the GNU General Public License is available on the World     *
  *   Wide Web at <http://www.gnu.org/copyleft/gpl.html>. You can also       *
  *   obtain it by writing to the Free Software Foundation,                  *
  *   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.         *
  *                                                                          *
  ****************************************************************************

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andrés Colubri

*******************************************************************************/

#include "moFBO.h"

#include "moArray.cpp"
moDefineDynamicArray(moFBOArray)

//===========================================
//
//				moFBO
//
//===========================================

moFBO::moFBO()
{
	m_gl = NULL;
	m_fbo = 0;
	m_is_screen_fbo = false;


	m_num_color_attach_points = 0;
    m_has_depth_buffer = m_has_stencil_buffer = false;
	m_width = 0;
	m_height = 0;
}

moFBO::~moFBO()
{
	Finish();
}

MOboolean  moFBO::Init(moGLManager* p_gl, MOboolean p_is_screen_fbo)
{
	m_gl = p_gl;
	m_is_screen_fbo = p_is_screen_fbo;

	if (m_is_screen_fbo) m_fbo = 0;
	else glGenFramebuffersEXT(1, &m_fbo);


	m_num_color_attach_points = 0;
    m_has_depth_buffer = m_has_stencil_buffer = false;
	m_width = 0;
	m_height = 0;
    InitAttachPointsArray();


	return (m_gl!=NULL);
}

MOboolean moFBO::Finish()
{
	if (m_has_depth_buffer && m_has_stencil_buffer)
	{
		glDeleteTextures(1, &m_DepthStencilTex);
		m_has_depth_buffer = false;
		m_has_stencil_buffer = false;
	}

    if (0 < m_fbo)
	{
		glDeleteFramebuffersEXT(1, &m_fbo);
		m_fbo = 0;
	}
	return true;
}

MOuint moFBO::CheckStatus()
{
	if (MODebug != NULL) MODebug->Push("Framebuffer status: ");
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch (status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			if (MODebug != NULL) MODebug->Push("GL_FRAMEBUFFER_COMPLETE_EXT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			if (MODebug != NULL) MODebug->Push("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			if (MODebug != NULL) MODebug->Push("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
			break;
//		case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
//			if (MODebug != NULL) MODebug->Push("GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT");
//			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			if (MODebug != NULL) MODebug->Push("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			if (MODebug != NULL) MODebug->Push("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			if (MODebug != NULL) MODebug->Push("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			if (MODebug != NULL) MODebug->Push("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT");
   			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			if (MODebug != NULL) MODebug->Push("GL_FRAMEBUFFER_UNSUPPORTED_EXT");
			break;
		default:
			if (MODebug != NULL) MODebug->Push("Unknown error");
			break;
	}
	return status;
}


void moFBO::Bind()
{
    CheckStatus();

    glDrawBuffers(m_num_color_attach_points, m_attach_points_array);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
}

void moFBO::SetDrawTexture(MOuint p_target, MOuint p_glid, MOuint p_attach_point)
{
    m_tex_glid_array[p_attach_point] = p_glid;
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, m_attach_points_array[p_attach_point], p_target, p_glid, 0);
}

void moFBO::SetNumDrawTextures(MOuint p_num)
{
    m_num_color_attach_points = p_num;
}

void moFBO::InitAttachPointsArray()
{
	for (MOuint i = 0; i < MO_MAX_COLOR_ATTACHMENTS_EXT; i++)
		m_attach_points_array[i] = GL_COLOR_ATTACHMENT0_EXT + i;
}
