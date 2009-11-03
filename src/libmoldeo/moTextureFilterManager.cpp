/*******************************************************************************

                         moTextureFilterManager.h

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

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andres Colubri

*******************************************************************************/

#include "moTextureFilterManager.h"

moTextureFilterManager::moTextureFilterManager()
{
	SetType( MO_OBJECT_RESOURCE );
	SetResourceType( MO_RESOURCETYPE_TEXTUREFILTER );
	SetName("Texture Filter Manager");

	m_glmanager = NULL;
	m_fbmanager = NULL;
	m_rendermanager = NULL;
	m_shadermanager = NULL;
	m_texmanager = NULL;
}

moTextureFilterManager::~moTextureFilterManager()
{
	Finish();
}

MOboolean moTextureFilterManager::Init()
{
	if (m_pResourceManager){
		m_glmanager = m_pResourceManager->GetGLMan();
		m_fbmanager = m_pResourceManager->GetFBMan();
		m_rendermanager = m_pResourceManager->GetRenderMan();
		m_shadermanager = m_pResourceManager->GetShaderMan();
		m_texmanager = m_pResourceManager->GetTextureMan();
	} else return false;

	m_filters_array.Init(0, NULL);
}

MOboolean moTextureFilterManager::Finish()
{
	m_filters_array.Finish();

	m_glmanager = NULL;
	m_fbmanager = NULL;
	m_rendermanager = NULL;
	m_shadermanager = NULL;
	m_texmanager = NULL;

	return true;
}

MOint moTextureFilterManager::GetTextureFilterMOid(moTextArray p_src_tex_names, moTextArray p_dest_tex_names, moText p_shader_name, MOboolean p_create_filter)
{
	moTextureFilter* pfilter;
	moTextureIndex* srctex;
	moTextureIndex* destex;
	moShader* pshader;
	MOboolean found, diff;
	for (MOuint i = 0; i < m_filters_array.Count(); i++)
	{
		pfilter = m_filters_array[i];
		if (pfilter == NULL) continue;

        srctex = pfilter->GetSrcTex();
        destex = pfilter->GetDestTex();
        pshader = pfilter->GetShader();
        found = false;

        if (p_src_tex_names.Count() == srctex->Count())
        {
            diff = false;
            for (MOuint j = 0; j < p_src_tex_names.Count(); j++)
                if (p_src_tex_names[j] != srctex->GetTexture(j)->GetName())
                {
                    diff = true;
                    break;
                }
            if (diff) continue;
        }
        else continue;

        if (p_dest_tex_names.Count() == destex->Count())
        {
            diff = false;
            for (MOuint j = 0; j < p_dest_tex_names.Count(); j++)
                if (p_dest_tex_names[j] != destex->GetTexture(j)->GetName())
                {
                    diff = true;
                    break;
                }
            if (diff) continue;
        }
        else continue;

		if (pshader->GetName() != p_shader_name) found = true;

        if (found) return i;
	}
	if (p_create_filter) return AddTextureFilter(p_src_tex_names, p_dest_tex_names, p_shader_name);
	else return -1;
}

MOint moTextureFilterManager::GetTextureFilterMOid(moText p_name, MOboolean p_create_filter)
{
	moTextureFilter* pfilter;
	for (MOuint i = 0; i < m_filters_array.Count(); i++)
	{
		pfilter = m_filters_array[i];
		if (pfilter == NULL) continue;
        if (pfilter->GetName() == p_name) return i;
	}
	if (p_create_filter) return AddTextureFilter(p_name);
	else return -1;
}

MOint moTextureFilterManager::AddTextureFilter(moTextArray p_src_tex_names, moTextArray p_dest_tex_names, moText p_shader_name, moTextureFilterParam *p_params)
{
	moTextureArray src_tex, dest_tex;
	moShader *pshader;
	MOuint dest_width, dest_height;
	MOuint i;
	MOint error_code;
    moText name = "";
    moText sep = "";
    error_code = 0;

    src_tex.Empty();
    dest_tex.Empty();

    for (i = 0; i < p_src_tex_names.Count(); i++)
    {
        error_code = LoadSourceTexture(p_src_tex_names[i], src_tex, i == 0, dest_width, dest_height);
        if (0 < error_code)
        {
             PrintErrorMsg(error_code);
             return -1;
        }
        name += (moText)sep + (moText)p_src_tex_names[i];
        sep = moText("|");
    }

    error_code = LoadShader(p_shader_name, &pshader);
    if (0 < error_code)
    {
         PrintErrorMsg(error_code);
         return -1;
    }
    name += (moText)sep + (moText)p_shader_name;

    for (i = 0; i < p_dest_tex_names.Count(); i++)
    {
        error_code = LoadDestTexture(p_dest_tex_names[i], dest_tex, dest_width, dest_height);
        if (0 < error_code)
        {
             PrintErrorMsg(error_code);
             return -1;
        }
        name += (moText)sep + (moText)p_dest_tex_names[i];
    }

    return AddTextureFilter(name, src_tex, dest_tex, pshader, p_params);
}

MOint moTextureFilterManager::AddTextureFilter(moText p_name, moTextureArray &p_src_tex, moTextureArray &p_dest_tex, moShader *p_shader, moTextureFilterParam *p_params)
{
    moTextureFilter* pfilter = new moTextureFilter();
    if (pfilter != NULL)
    {
        pfilter->Init(m_glmanager, m_fbmanager, m_rendermanager, p_src_tex, p_dest_tex, p_shader);

        m_filters_array.Add(pfilter);
        pfilter->SetMOId(m_filters_array.Count() - 1);
        pfilter->SetName(p_name);
        return pfilter->GetMOId();
    }
    else return -1;
}

MOint moTextureFilterManager::AddTextureFilter(moText p_name)
{
	MOuint i, j;
	MOint error_code;

	MOboolean reading_src_tex;

	moTextureFilter *pfilter;
	moTextureArray src_tex, dest_tex;
	moShader *pshader;

	MOuint dest_width, dest_height;
	moText name, extension, left, tmp;

    reading_src_tex = true;

    error_code = 0;
    src_tex.Empty();
    dest_tex.Empty();
    tmp =  p_name;
    j = 0;
    while (tmp != moText(""))
    {
        name = tmp.Scan(moText("|"));
        name = name.Trim();

        extension = name;
        extension.Right(3);

        if (extension == moText("cfg"))
        {
			reading_src_tex = false;
			error_code = LoadShader(name, &pshader);
		}
		else if (reading_src_tex)
		{
			// Loading source textures...
			error_code = LoadSourceTexture(name, src_tex, j == 0, dest_width, dest_height);
		}
		else
		{
			// Creating destination textures...
			left = name;
			left.Left(3);
			if (left == moText("res:")) error_code = LoadDestTexResolution(name, dest_width, dest_height);
			else error_code = LoadDestTexture(name, dest_tex, dest_width, dest_height);
		}
        j++;
    }

	if (error_code == 0)
	{
	    return AddTextureFilter(p_name, src_tex, dest_tex, pshader);
	}
	else
	{
		PrintErrorMsg(error_code);
        return -1;
	}
}

void moTextureFilterManager::Apply(MOuint p_idx, MOuint p_i, MOfloat p_fade, moTextureFilterParam *p_params)
{
	if (ValidIndex(p_idx)) m_filters_array[p_idx]->Apply(p_i, p_fade, p_params);
}

void moTextureFilterManager::Apply(MOuint p_idx, MOfloat p_cycle, MOfloat p_fade, moTextureFilterParam *p_params)
{
	if (ValidIndex(p_idx)) m_filters_array[p_idx]->Apply(p_cycle, p_fade, p_params);
}

void moTextureFilterManager::Apply(MOuint p_idx, moTempo *p_tempo, MOfloat p_fade, moTextureFilterParam *p_params)
{
	if (ValidIndex(p_idx)) m_filters_array[p_idx]->Apply(p_tempo, p_fade, p_params);
}

MOboolean moTextureFilterManager::DeleteTextureFilter(MOint p_idx)
{
	if (p_idx < (MOint)m_filters_array.Count())
	{
		m_filters_array.Remove(p_idx);
		return true;
	}
	else return false;
}

MOboolean moTextureFilterManager::ValidIndex(MOuint p_idx)
{
	if (p_idx < m_filters_array.Count()) return true;
	else
	{
		moText text;
		text =  moText("Error(moTextureIndex): the index: ");
		text += IntToStr(p_idx);
		text +=  moText(" does not exists.");
		MODebug2->Error(text);
		return false;
	}
}

MOint moTextureFilterManager::LoadShader(moText& name, moShader **pshader)
{
	MOint idx = m_shadermanager->GetShaderMOId(name, true);
	if (-1 < idx)
	{
		*pshader = m_shadermanager->GetShader(idx);
		return 0;
	}
	return 1;
}

MOint moTextureFilterManager::LoadSourceTexture(moText& name, moTextureArray& src_tex, MOboolean first_tex, MOuint& dest_width, MOuint& dest_height)
{
	MOint idx = m_texmanager->GetTextureMOId(name, true);
	if (-1 < idx)
	{
		moTexture* ptex = m_texmanager->GetTexture(idx);
		src_tex.Add(ptex);
		if (first_tex)
		{
			// By default, the resolution of the destination textures is the same
			// as the source textures.
			dest_width = ptex->GetWidth();
			dest_height = ptex->GetHeight();
		}
		return 0;
	}
	return 2;
}

MOint moTextureFilterManager::LoadDestTexResolution( const moText& name, MOuint& dest_width, MOuint& dest_height)
{
	moText tmp_width, tmp_height;
	MOint l;

	tmp_height = name;
	tmp_height.Scan(moText("x"));
	l = tmp_height.Length();
	tmp_height.Mid(1, l - 1);

	tmp_width = name;
	tmp_width.Mid(4, tmp_width.Length() - l - 4);

	dest_width = atoi(tmp_width);
	dest_height = atoi(tmp_height);

	return 0;
}

MOint moTextureFilterManager::LoadDestTexture( const moText& name, moTextureArray& dest_tex, MOuint dest_width, MOuint dest_height)
{
	moTexture* ptex;
	MOint idx = m_texmanager->GetTextureMOId(name, false);
	if (-1 < idx)
	{
		// The destination texture already exists...
		ptex = m_texmanager->GetTexture(idx);
		dest_tex.Add(ptex);
		return 0;
	}
	else
	{
		idx = m_texmanager->AddTexture(name, dest_width, dest_height);
		if (-1 < idx)
		{
			ptex = m_texmanager->GetTexture(idx);
			dest_tex.Add(ptex);
			return 0;
		}
		else return 4;
	}
}

void moTextureFilterManager::PrintErrorMsg(MOint error_code)
{
    // Print some error message...
    if (error_code == 1)
        MODebug->Push("Error in creating filter: cannot load shader");
    else if (error_code == 2)
        MODebug->Push("Error in creating filter: cannot load source texture");
    else if (error_code == 3)
        MODebug->Push("Error in creating filter: cannot read resolution of destination texture");
    else if (error_code == 4)
        MODebug->Push("Error in creating filter: cannot create destination texture");
    else
        MODebug->Push("Unknown error in creating filter.");
}
