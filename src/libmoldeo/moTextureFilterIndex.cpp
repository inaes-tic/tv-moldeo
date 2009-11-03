/*******************************************************************************

                             moTextureFilterIndex.cpp

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

#include "moTextureFilterIndex.h"
#include "moResourceManager.h"

moTextureFilterIndex::moTextureFilterIndex()
{
	m_filters_array.Init(0, NULL);

}
moTextureFilterIndex::~moTextureFilterIndex()
{
	Finish();
}

MOboolean moTextureFilterIndex::Init(moTextureFilterManager* p_texfilterman)
{
	m_texfilterman = p_texfilterman;

	return true;
}

MOboolean moTextureFilterIndex::Init( moParam* p_param, moTextureFilterManager* p_texfilterman)
{
	m_texfilterman = p_texfilterman;

	MOuint res = LoadFilters(p_param);

	return 0 < res;
}

MOboolean moTextureFilterIndex::Init(moConfig* p_cfg, MOuint p_param_idx, moTextureFilterManager* p_texfilterman)
{
	m_texfilterman = p_texfilterman;

	MOuint res = LoadFilters(p_cfg, p_param_idx);

	return 0 < res;
}

MOboolean moTextureFilterIndex::Init(moTextArray* p_filters_str, moTextureFilterManager* p_texfilterman)
{
	m_texfilterman = p_texfilterman;

	MOuint res = LoadFilters(p_filters_str);

	return 0 < res;
}

MOboolean moTextureFilterIndex::Finish()
{
	m_filters_array.Finish();
	return true;
}

MOint moTextureFilterIndex::Add(moTextureFilter* p_filter)
{
	MOint idx;
	m_filters_array.Add(p_filter);
	idx = m_filters_array.Count() - 1;
	return idx;
}

void moTextureFilterIndex::Apply(MOuint p_idx, MOuint p_i, MOfloat p_fade, moTextureFilterParam *p_params)
{
	if (ValidIndex(p_idx)) m_filters_array[p_idx]->Apply(p_i, p_fade, p_params);
}

void moTextureFilterIndex::Apply(MOuint p_idx, MOfloat p_cycle, MOfloat p_fade, moTextureFilterParam *p_params)
{
	if (ValidIndex(p_idx)) m_filters_array[p_idx]->Apply(p_cycle, p_fade, p_params);
}

void moTextureFilterIndex::Apply(MOuint p_idx, moTempo *p_tempo, MOfloat p_fade, moTextureFilterParam *p_params)
{
	if (ValidIndex(p_idx)) m_filters_array[p_idx]->Apply(p_tempo, p_fade, p_params);
}

void moTextureFilterIndex::Apply(moTempo *p_tempo, MOfloat p_fade, moTextureFilterParam *p_params)
{
	for (MOuint i = 0; i < m_filters_array.Count(); i++) m_filters_array[i]->Apply(p_tempo, p_fade, p_params);
}

MOboolean moTextureFilterIndex::ValidIndex(MOuint p_idx)
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

MOuint moTextureFilterIndex::LoadFilter(moValue* p_value) {

    int j;
	MOuint nFilterParts;
	moTextureFilter* pfilter;
    moText name = "";
    moText sep = "";

    nFilterParts = p_value->GetSubValueCount();
    for (j = 0; j < nFilterParts; j++)
    {
        if ( p_value->GetSubValue(j).GetData()->Type()==MO_DATA_TEXT ||
        p_value->GetSubValue(j).GetData()->Type()==MO_DATA_IMAGESAMPLE ||
        p_value->GetSubValue(j).GetData()->Type()==MO_DATA_IMAGESAMPLE_FILTERED ) {
            name += (moText)sep + (moText)p_value->GetSubValue(j).Text();
            sep = moText("|");
        }
    }

    MOint idx = m_texfilterman->GetTextureFilterMOid(name, true);
    if (-1 < idx)
    {
        pfilter = m_texfilterman->GetTextureFilter(idx);
        if (pfilter != NULL) Add(pfilter);

    }
	return m_filters_array.Count();
}

MOuint moTextureFilterIndex::LoadFilters(moParam* p_param)
{
	MOuint i, j, idx;
	MOuint nFilters, nFilterParts;
	moTextureFilter* pfilter;
	moText part;
    moText name = "";
    moText sep = "";

	moParam* param = p_param;

	nFilters = p_param->GetValuesCount();
	p_param->FirstValue();
	for (i = 0; i < nFilters; i++)
	{
		param = p_param;
		nFilterParts = param->GetValue().GetSubValueCount();

        name = "";
        sep = "";
		for (j = 0; j < nFilterParts; j++)
		{
			part = param->GetValue().GetSubValue(j).Text();
            name += (moText)sep + (moText)part;
            sep = moText("|");
		}

        idx = m_texfilterman->GetTextureFilterMOid(name, true);
        if (-1 < idx)
        {
            pfilter = m_texfilterman->GetTextureFilter(idx);
            if (pfilter != NULL) Add(pfilter);
        }

		p_param->NextValue();
	}

	return m_filters_array.Count();
}

MOuint moTextureFilterIndex::LoadFilters(moConfig* p_cfg, MOuint p_param_idx)
{
	MOuint i, j, idx;
	MOuint nFilters, nFilterParts;
	moTextureFilter* pfilter;
	moText part;
    moText name = "";
    moText sep = "";
	moParam* param;

	nFilters = p_cfg->GetParam(p_param_idx).GetValuesCount();

	p_cfg->SetCurrentParamIndex(p_param_idx);
	p_cfg->FirstValue();
	for (i = 0; i < nFilters; i++)
	{
		param = &p_cfg->GetParam();
		nFilterParts = param->GetValue().GetSubValueCount();

        name = "";
        sep = "";
		for (j = 0; j < nFilterParts; j++)
		{
			part = param->GetValue().GetSubValue(j).Text();
            name += (moText)sep + (moText)part;
            sep = moText("|");
		}

        idx = m_texfilterman->GetTextureFilterMOid(name, true);
        if (-1 < idx)
        {
            pfilter = m_texfilterman->GetTextureFilter(idx);
            if (pfilter != NULL) Add(pfilter);
        }

		p_cfg->NextValue();
	}

	return m_filters_array.Count();
}

MOuint moTextureFilterIndex::LoadFilters(moTextArray* p_filters_str)
{
	MOuint i, idx;
    moText name;
	moTextureFilter* pfilter;
	MOuint nFilters;

	nFilters = p_filters_str->Count();
	for (i = 0; i < nFilters; i++)
	{
		name = p_filters_str->Get(i);
		idx = m_texfilterman->GetTextureFilterMOid(name, true);
        if (-1 < idx)
        {
            pfilter = m_texfilterman->GetTextureFilter(idx);
            if (pfilter != NULL) Add(pfilter);
        }
	}

	return m_filters_array.Count();
}
