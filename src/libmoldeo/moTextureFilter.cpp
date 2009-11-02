/*******************************************************************************

                                moTextureFilter.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                     *
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

#include "moTextureFilter.h"
#include "moResourceManager.h"

#include "moArray.cpp"
moDefineDynamicArray(moTextureFilterArray)

//===========================================
//
//				moTextureFilter
//
//===========================================

MOboolean moTextureFilter::Init(moGLManager* p_glman, moFBManager* p_fbman, moRenderManager* p_renderman, moTextureArray &p_src_tex, moTextureArray &p_dest_tex, moShader *p_shader, moTextureFilterParam *p_params)
{
	MOuint i, fboidx;
	moText uname;

	m_glman = p_glman;
	m_renderman = p_renderman;
	m_fbman = p_fbman;

	m_use_screen_tex = false;
	for (i = 0; i < p_src_tex.Count(); i++)
	{
		m_src_tex.Add(p_src_tex[i]);
		if (p_src_tex[i]->GetName() == moText("screen_texture")) m_use_screen_tex = true;
	}
	for (i = 0; i < p_dest_tex.Count(); i++) {
	    m_dest_tex.Add(p_dest_tex[i]);
	}

	m_shader = p_shader;
	moShaderGLSL* pglsl = (moShaderGLSL*)m_shader;
	pglsl->PrintVertShaderLog();
	pglsl->PrintFragShaderLog();

	// Gettting uniform parameters.
	for (i = 0; i < m_src_tex.Count(); i++)
	{
		uname = moText("src_tex_unit") + (moText)IntToStr(i);
		m_src_tex_unit[i] = pglsl->GetUniformID(uname);
		uname = moText("src_tex_offset") + IntToStr(i);
		m_src_tex_offset[i] = pglsl->GetUniformID(uname);
	}

	uname = moText("tempo_angle");
	m_tempo_angle = pglsl->GetUniformID(uname);

	uname = moText("dest_tex_size");
	m_dest_tex_size = pglsl->GetUniformID(uname);

    uname = moText("fade_const");
    m_fade_const = pglsl->GetUniformID(uname);

    if (p_params == NULL) m_DefParams = new moTextureFilterParam();
    else m_DefParams = p_params;

    m_DefParams->getParamIDs(pglsl);

    m_dest_fbo = m_fbman->AddFBO();

	return true;
}

MOboolean moTextureFilter::Finish()
{
    if (m_DefParams != NULL)
    {
        delete m_DefParams;
        m_DefParams = NULL;
    }
	return true;
}

void moTextureFilter::Apply(MOuint p_i, MOfloat p_fade, moTextureFilterParam *p_params)
{
	MOint w = m_dest_tex[0]->GetWidth();
	MOint h = m_dest_tex[0]->GetHeight();
	SetGLConf(w, h);
	moShaderGLSL* pglsl = (moShaderGLSL*)m_shader;

	if (m_use_screen_tex) m_renderman->SaveScreen();

    SetDestTexInFBO();
	BindDestFBO();

	pglsl->StartShader();
	SetupShader(w, h, NULL, p_fade, p_params);

	BindSrcTex(p_i);
	pglsl->DrawGrid(w, h, m_src_tex.Count());
	UnbindSrcTex();

	pglsl->StopShader();

	UnbindDestFBO();

	RestoreGLConf();
}

void moTextureFilter::Apply(MOfloat p_cycle, MOfloat p_fade, moTextureFilterParam *p_params)
{
	MOint w = m_dest_tex[0]->GetWidth();
	MOint h = m_dest_tex[0]->GetHeight();
	SetGLConf(w, h);
	moShaderGLSL* pglsl = (moShaderGLSL*)m_shader;

	if (m_use_screen_tex) m_renderman->SaveScreen();

    SetDestTexInFBO();
	BindDestFBO();

	pglsl->StartShader();
	SetupShader(w, h, NULL, p_fade, p_params);

	BindSrcTex(p_cycle);
	pglsl->DrawGrid(w, h, m_src_tex.Count());
	UnbindSrcTex();

	pglsl->StopShader();

	UnbindDestFBO();

	RestoreGLConf();
}

void moTextureFilter::Apply(moTempo *p_tempo, MOfloat p_fade, moTextureFilterParam *p_params)
{
	MOint w = m_dest_tex[0]->GetWidth();
	MOint h = m_dest_tex[0]->GetHeight();
	SetGLConf(w, h);
	moShaderGLSL* pglsl = (moShaderGLSL*)m_shader;

	if (m_use_screen_tex) m_renderman->SaveScreen();

    SetDestTexInFBO();
	BindDestFBO();

	pglsl->StartShader();
	SetupShader(w, h, p_tempo, p_fade, p_params);

	BindSrcTex(p_tempo);
	pglsl->DrawGrid(w, h, m_src_tex.Count());
	UnbindSrcTex();

	pglsl->StopShader();

	UnbindDestFBO();

	RestoreGLConf();
}

void moTextureFilter::SetupShader(MOint w, MOint h, moTempo *p_tempo, MOfloat p_fade, moTextureFilterParam *p_params)
{
	for (MOuint i = 0; i < m_src_tex.Count(); i++)
	{
		if (-1 < m_src_tex_unit[i])
			glUniform1iARB(m_src_tex_unit[i], i);

		if (-1 < m_src_tex_offset[i])
			if (m_glman->RectTexture(m_src_tex[i]->GetTexTarget())) glUniform2fARB(m_src_tex_offset[i], 1.0, 1.0);
			else glUniform2fARB(m_src_tex_offset[i], 1.0 / float(m_src_tex[i]->GetWidth()), 1.0 / float(m_src_tex[i]->GetHeight()));
	}

	if (-1 < m_tempo_angle)
		if (p_tempo != NULL)
		{
			float a = p_tempo->ang;
			float f = fmod(float(a), float(2.0 * moMathf::PI)) / (2.0 * moMathf::PI);
			glUniform2fARB(m_tempo_angle, a, f);
		}
		else glUniform2fARB(m_tempo_angle, 0.0, 0.0);

	if (-1 < m_dest_tex_size) glUniform2fARB(m_dest_tex_size, w, h);

	if (-1 < m_fade_const) glUniform1fARB(m_fade_const, p_fade);

    if (p_params != NULL)
    {
        m_DefParams->CopyDefParamIDs(p_params);
        p_params->setParamValues();
    }
    else m_DefParams->setParamValues();
}

void moTextureFilter::SetGLConf(MOint w, MOint h)
{
	m_blend_on = glIsEnabled(GL_BLEND);
	glGetIntegerv(GL_POLYGON_MODE, &m_poly_mode);

	glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT, GL_FILL);

	m_glman->SaveView();
	m_glman->SetOrthographicView(w, h);
}

void moTextureFilter::RestoreGLConf()
{
	m_glman->RestoreView();

	if (m_blend_on) glEnable(GL_BLEND);
	glPolygonMode(GL_FRONT, m_poly_mode);
}

void moTextureFilter::BindDestFBO()
{
    m_glman->PushFBO();
    m_glman->SetFBO(m_dest_fbo);
}

void moTextureFilter::UnbindDestFBO()
{
    m_glman->PopFBO();
}

void moTextureFilter::SetDestTexInFBO()
{
    if (-1 < m_dest_fbo)
    {
        moTexture* ptex;
        moFBO* pfbo = m_fbman->GetFBO(m_dest_fbo);
        pfbo->SetNumDrawTextures(m_dest_tex.Count());
        for (int i = 0; i < m_dest_tex.Count(); i++)
        {
            ptex = m_dest_tex[i];
            pfbo->SetDrawTexture(ptex->GetTexTarget(), ptex->GetGLId(), i);
        }
    }
}

void moTextureFilter::BindSrcTex(MOuint p_i)
{
	for (MOuint i = 0; i < m_src_tex.Count(); i++)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + i);
		glBindTexture(m_src_tex[i]->GetTexTarget(), m_src_tex.GetGLId(i, (GLint)p_i));
	}
}

void moTextureFilter::BindSrcTex(MOfloat p_cycle)
{
	for (MOuint i = 0; i < m_src_tex.Count(); i++)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + i);
		glBindTexture(m_src_tex[i]->GetTexTarget(), m_src_tex.GetGLId(i, p_cycle));
	}
}

void moTextureFilter::BindSrcTex(moTempo *p_tempo)
{
	for (MOuint i = 0; i < m_src_tex.Count(); i++)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + i);
		glBindTexture(m_src_tex[i]->GetTexTarget(), m_src_tex.GetGLId(i, p_tempo));
	}
}

void moTextureFilter::UnbindSrcTex()
{
	for (MOuint i = m_src_tex.Count(); 0 < i; i--)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + i - 1);
		glBindTexture(m_src_tex[i-1]->GetTexTarget(), 0);
	}
}

