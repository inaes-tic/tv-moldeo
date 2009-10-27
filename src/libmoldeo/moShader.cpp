/*******************************************************************************

                               moShader.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *   (at your option) any later version.                                    *
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

  Copyright (C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andres Colubri

  Description:
  Base class for GLSL and Cg shaders.

*******************************************************************************/

#include "moShader.h"

#include "moArray.cpp"
moDefineDynamicArray(moShaderArray)

//===========================================
//
//				moShader
//
//===========================================

moShader::moShader()
{
    m_Active = 0;
    m_VertErrorCode = 0;
    m_FragErrorCode = 0;

	m_name = moText("");
}

moShader::~moShader()
{
    Finish();
}

MOboolean moShader::Init()
{
	return true;
}

MOboolean moShader::Finish()
{
    m_VertErrorCode = 0;
    m_FragErrorCode = 0;

    if (ShaderActive()) StopShader();
	return true;
}

void moShader::StartShader()
{
    m_Active = 1;
}

void moShader::StopShader()
{
    m_Active = 0;
}

MOboolean moShader::ShaderActive()
{
    return m_Active;
}

MOint moShader::VertErrorCode()
{
    return m_VertErrorCode;
}

MOint moShader::FragErrorCode()
{
    return m_FragErrorCode;
}

moText moShader::LoadShaderSource(moText p_fn)
{
	FILE *src_file = fopen(p_fn, "rt");
	moText src_text;

	if (src_file)
	{
		char *content = NULL;
		int count = 0;

		fseek(src_file, 0, SEEK_END);
		count = ftell(src_file);
		fseek(src_file, 0, SEEK_SET);
        if (count > 0)
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char), count, src_file);
			content[count] = '\0';
		}
		fclose(src_file);

		src_text = moText(content);
	}
	else
	{
		moText errstr = "Shader source file ";
		errstr += p_fn + moText(" not found") ;
		if (MODebug != NULL) MODebug->Push(errstr);
		src_text = moText("");
	}

	return src_text;
}
