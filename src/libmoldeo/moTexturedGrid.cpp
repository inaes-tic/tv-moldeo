/*******************************************************************************

                                 moShader.h

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
  Andres Colubri

  Description:
  Clase para definr una grilla texturada.

*******************************************************************************/

#include "moTexturedGrid.h"

//===========================================
//
//				moTextureClip
//
//===========================================

moTextureClip::moTextureClip()
{
    s0 = 0.0;
    s1 = 1.0;

    t0 = 0.0;
    t1 = 1.0;
}

void moTextureClip::SetEntireTexClip()
{
}

moTextureClip &moTextureClip::operator = (const moTextureClip &p_src_clip)
{
    s0 = p_src_clip.s0;
    s1 = p_src_clip.s1;
    t0 = p_src_clip.t0;
    t1 = p_src_clip.t1;
	return *this;
}

//===========================================
//
//				moTexturedGrid
//
//===========================================

moTexturedGrid::moTexturedGrid()
{
	m_num_layers = 0;
	m_size_x = 0;
	m_size_y = 0;
}

moTexturedGrid::~moTexturedGrid()
{
	Finish();
}

MOboolean moTexturedGrid::Init(MOint p_size_x, MOint p_size_y, MOint p_num_layers, const MOfloat p_grid_dx[], const MOfloat p_grid_dy[])
{
	m_size_x = p_size_x;
	m_size_y = p_size_y;

	m_num_layers = p_num_layers;
	for (int i = 0; i < m_num_layers; i++)
	{
		m_grid_dx[i] = p_grid_dx[i];
		m_grid_dy[i] = p_grid_dy[i];
	}
	return true;
}

// The format of the grid parameters is the following:
// nx ny
// dx0 dy0
// dx1 dy1
// ...
// dxn dyn
// there nx and ny are the number of grid quads along x and y, and dxi, dyi
// are the grid spacings on each layer (the layer 0 represents the grid itself,
// the rest of the layers represent the texture coordinates).
MOboolean moTexturedGrid::Init(moConfig* p_cfg, MOuint p_param_idx)
{
	moParam* param;
	MOuint count = p_cfg->GetParam(p_param_idx).GetValue().GetSubValueCount();

	p_cfg->SetCurrentParamIndex(p_param_idx);
	p_cfg->FirstValue();
	for (MOuint i = 0; i < count; i++)
	{
		param = &p_cfg->GetParam();
		if (i == 0)
		{
			m_size_x = param->GetValue().GetSubValue(0).Int() + 1;
			m_size_y = param->GetValue().GetSubValue(1).Int() + 1;
		}
		else
		{
			// Reading layers.
			m_grid_dx[i - 1] = param->GetValue().GetSubValue(0).Float();
			m_grid_dy[i - 1] = param->GetValue().GetSubValue(1).Float();

		}
		p_cfg->NextValue();
	}

	// if there is only one parameter line, it should contain the
	// grid dimensions. The 0th layer is default to the grid spacing
	// corresponding to 1/(number of grid quads)
	if (count == 1)
	{
		m_num_layers = 1;
		m_grid_dx[0] = 1.0 / float(m_size_x - 1);
		m_grid_dy[0] = 1.0 / float(m_size_y - 1);
	}
	else m_num_layers = count - 1;
	return true;
}

void moTexturedGrid::Set1QuadGrid()
{
	m_num_layers = 1;
	m_size_x = 2;
	m_size_y = 2;
	m_grid_dx[0] = 1.0;
	m_grid_dy[0] = 1.0;
}

MOboolean moTexturedGrid::Finish() {
	return true;
}

void moTexturedGrid::GetPoint(MOint layer, MOint i, MOint j, MOfloat &x, MOfloat &y)
{
	if (layer < m_num_layers)
	{
		x = i * m_grid_dx[layer];
		y = j * m_grid_dy[layer];
	}
	else
	{
		// If there are not layers defined below the 0th, then the texture coordinates
		// are set identical to the grid coordinates.
		x = i * m_grid_dx[0];
		y = j * m_grid_dy[0];
	}
}

void moTexturedGrid::Draw(MOint w, MOint h, MOint l, const moTextureClip &clip)
{
	float x0, y0, x1, y1;

	for (int j = 0; j < m_size_y - 1; j++)
	{
		glBegin(GL_QUAD_STRIP);
			for (int i = 0; i < m_size_x; i++)
			{
				GetPoint(0, i, j, x0, y0);
				GetPoint(0, i, j + 1, x1, y1);

				x0 *= w; y0 *= h;
				x1 *= w; y1 *= h;

				SetTexCoord(i, j, l, clip);
				glVertex2f(x0, y0);

				SetTexCoord(i, j + 1, l, clip);
				glVertex2f(x1, y1);
			}
		glEnd();
	}
}

void moTexturedGrid::SetTexCoord(MOint i, MOint j, MOint l, const moTextureClip &clip)
{
	float s, t;
	for (int k = 1; k <= l; k++)
	{
        GetPoint(k, i, j, s, t);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB + k - 1, clip.s0 + s / (clip.s1 - clip.s0), clip.t0 + t / (clip.t1 - clip.t0));
    }
}

moTexturedGrid &moTexturedGrid::operator = (const moTexturedGrid &p_src_grid)
{
	Finish();
	Init(p_src_grid.m_size_x, p_src_grid.m_size_y, p_src_grid.m_num_layers, p_src_grid.m_grid_dx, p_src_grid.m_grid_dy);
	return *this;
}
