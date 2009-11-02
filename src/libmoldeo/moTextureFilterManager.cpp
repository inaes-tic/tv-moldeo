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
}

MOboolean moTextureFilterManager::Init()
{
	if (m_pResourceManager){
		m_glmanager = m_pResourceManager->GetGLMan();
		m_fbmanager = m_pResourceManager->GetFBMan();
		m_rendermanager = m_pResourceManager->GetRenderMan();
	} else return false;

	m_filters_array.Init(0, NULL);
}

MOboolean moTextureFilterManager::Finish()
{
	m_filters_array.Finish();

	m_glmanager = NULL;
	m_fbmanager = NULL;
	m_rendermanager = NULL;

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
		if (pshader == NULL) continue;

        srctex = pfilter->GetSrcTex();
        destex = pfilter->GetDestTex();
        pshader = pfilter->GetShader();
        found = false;

        if (p_src_tex_names.Count() == srctex->Count())
        {
            diff = false;
            for (MOuint j = 0; j < p_src_tex_names.Count(); j++)
                if (stricmp(p_src_tex_names[j], srctex->GetTexture(j)->GetName()))
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
                if (stricmp(p_dest_tex_names[j], destex->GetTexture(j)->GetName()))
                {
                    diff = true;
                    break;
                }
            if (diff) continue;
        }
        else continue;

		if (!stricmp(pshader->GetName(), p_shader_name)) found = true;

        if (found) return i;
	}
	if (p_create_filter) return AddTextureFilter(p_src_tex_names, p_dest_tex_names, p_shader_name);
	else return -1;
}

MOint moTextureFilterManager::AddTextureFilter(moTextArray p_src_tex_names, moTextArray p_dest_tex_names, moText p_shader_name)
{
    return 0;
}

MOint moTextureFilterManager::AddTextureFilter(moTextureArray &p_src_tex, moTextureArray &p_dest_tex, moShader *p_shader, moTextureFilterParam *p_params)
{
    moTextureFilter* pfilter = new moTextureFilter();
    if (pfilter != NULL)
    {
        pfilter->Init(m_glmanager, m_fbmanager, m_rendermanager, p_src_tex, p_dest_tex, p_shader);

        m_filters_array.Add(pfilter);
        return m_filters_array.Count() - 1;
    }
    else return -1;
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
