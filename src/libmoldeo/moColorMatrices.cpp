/*******************************************************************************

                                moColorMatrices.h

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

#include "moColorMatrices.h"

moColorMatrix::moColorMatrix()
{
    ZeroMatrix();
}

moColorMatrix::~moColorMatrix()
{
	Finish();
}

void moColorMatrix::Init()
{
}

void moColorMatrix::Finish()
{
}

void moColorMatrix::Copy(moColorMatrix& p_mat)
{
	int i, j;
	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
			m_Matrix[MatIdx(i, j)] = p_mat.m_Matrix[MatIdx(i, j)];
}

void moColorMatrix::Multiply(moColorMatrix& p_mat)
{
	int i, j;
	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
			m_MatrixChg[MatIdx(i, j)] = p_mat.m_Matrix[MatIdx(i, j)];

    ApplyChgMatrix();
}

void moColorMatrix::ZeroMatrix()
{
	for (int j = 0; j < 4; j++)
		for (int i = 0; i < 4; i++)
			m_Matrix[MatIdx(i, j)] = 0.0;
}

void moColorMatrix::IdentityMatrix()
{
	for (int j = 0; j < 4; j++)
		for (int i = 0; i < 4; i++)
			m_Matrix[MatIdx(i, j)] = i == j;
}

void moColorMatrix::SetXRotateMatrix(float rs, float rc)
{
    m_MatrixChg[MatIdx(0, 0)] = 1.0;
    m_MatrixChg[MatIdx(0, 1)] = 0.0;
    m_MatrixChg[MatIdx(0, 2)] = 0.0;
    m_MatrixChg[MatIdx(0, 3)] = 0.0;

    m_MatrixChg[MatIdx(1, 0)] = 0.0;
    m_MatrixChg[MatIdx(1, 1)] = rc;
    m_MatrixChg[MatIdx(1, 2)] = rs;
    m_MatrixChg[MatIdx(1, 3)] = 0.0;

    m_MatrixChg[MatIdx(2, 0)] = 0.0;
    m_MatrixChg[MatIdx(2, 1)] = -rs;
    m_MatrixChg[MatIdx(2, 2)] = rc;
    m_MatrixChg[MatIdx(2, 3)] = 0.0;

    m_MatrixChg[MatIdx(3, 0)] = 0.0;
    m_MatrixChg[MatIdx(3, 1)] = 0.0;
    m_MatrixChg[MatIdx(3, 2)] = 0.0;
    m_MatrixChg[MatIdx(3, 3)] = 1.0;
}

void moColorMatrix::SetYRotateMatrix(float rs, float rc)
{
    m_MatrixChg[MatIdx(0, 0)] = rc;
    m_MatrixChg[MatIdx(0, 1)] = 0.0;
    m_MatrixChg[MatIdx(0, 2)] = -rs;
    m_MatrixChg[MatIdx(0, 3)] = 0.0;

    m_MatrixChg[MatIdx(1, 0)] = 0.0;
    m_MatrixChg[MatIdx(1, 1)] = 1.0;
    m_MatrixChg[MatIdx(1, 2)] = 0.0;
    m_MatrixChg[MatIdx(1, 3)] = 0.0;

    m_MatrixChg[MatIdx(2, 0)] = rs;
    m_MatrixChg[MatIdx(2, 1)] = 0.0;
    m_MatrixChg[MatIdx(2, 2)] = rc;
    m_MatrixChg[MatIdx(2, 3)] = 0.0;

    m_MatrixChg[MatIdx(3, 0)] = 0.0;
    m_MatrixChg[MatIdx(3, 1)] = 0.0;
    m_MatrixChg[MatIdx(3, 2)] = 0.0;
    m_MatrixChg[MatIdx(3, 3)] = 1.0;
}

void moColorMatrix::SetZRotateMatrix(float rs, float rc)
{
    m_MatrixChg[MatIdx(0, 0)] = rc;
    m_MatrixChg[MatIdx(0, 1)] = rs;
    m_MatrixChg[MatIdx(0, 2)] = 0.0;
    m_MatrixChg[MatIdx(0, 3)] = 0.0;

    m_MatrixChg[MatIdx(1, 0)] = -rs;
    m_MatrixChg[MatIdx(1, 1)] = rc;
    m_MatrixChg[MatIdx(1, 2)] = 0.0;
    m_MatrixChg[MatIdx(1, 3)] = 0.0;

    m_MatrixChg[MatIdx(2, 0)] = 0.0;
    m_MatrixChg[MatIdx(2, 1)] = 0.0;
    m_MatrixChg[MatIdx(2, 2)] = 1.0;
    m_MatrixChg[MatIdx(2, 3)] = 0.0;

    m_MatrixChg[MatIdx(3, 0)] = 0.0;
    m_MatrixChg[MatIdx(3, 1)] = 0.0;
    m_MatrixChg[MatIdx(3, 2)] = 0.0;
    m_MatrixChg[MatIdx(3, 3)] = 1.0;
}

void moColorMatrix::SetZShearMatrix(float dx, float dy)
{
    m_MatrixChg[MatIdx(0, 0)] = 1.0;
    m_MatrixChg[MatIdx(0, 1)] = 0.0;
    m_MatrixChg[MatIdx(0, 2)] = dx;
    m_MatrixChg[MatIdx(0, 3)] = 0.0;

    m_MatrixChg[MatIdx(1, 0)] = 0.0;
    m_MatrixChg[MatIdx(1, 1)] = 1.0;
    m_MatrixChg[MatIdx(1, 2)] = dy;
    m_MatrixChg[MatIdx(1, 3)] = 0.0;

    m_MatrixChg[MatIdx(2, 0)] = 0.0;
    m_MatrixChg[MatIdx(2, 1)] = 0.0;
    m_MatrixChg[MatIdx(2, 2)] = 1.0;
    m_MatrixChg[MatIdx(2, 3)] = 0.0;

    m_MatrixChg[MatIdx(3, 0)] = 0.0;
    m_MatrixChg[MatIdx(3, 1)] = 0.0;
    m_MatrixChg[MatIdx(3, 2)] = 0.0;
    m_MatrixChg[MatIdx(3, 3)] = 1.0;
}

void moColorMatrix::XFormPoint(float &tx, float &ty, float &tz)
{
    float x = RLUM;
	float y = GLUM;
	float z = BLUM;

    tx = x * m_Matrix[MatIdx(0, 0)] + y * m_Matrix[MatIdx(1, 0)] + z * m_Matrix[MatIdx(2, 0)] + m_Matrix[MatIdx(3, 0)];
    ty = x * m_Matrix[MatIdx(0, 1)] + y * m_Matrix[MatIdx(1, 1)] + z * m_Matrix[MatIdx(2, 1)] + m_Matrix[MatIdx(3, 1)];
    tz = x * m_Matrix[MatIdx(0, 2)] + y * m_Matrix[MatIdx(1, 2)] + z * m_Matrix[MatIdx(2, 2)] + m_Matrix[MatIdx(3, 2)];
}

void moColorMatrix::ApplyChgMatrix()
{
	int i, j, k;

	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
		{
			m_MatrixTemp[MatIdx(i, j)] = 0.0;
			for (k = 0; k < 4; k++)
				m_MatrixTemp[MatIdx(i, j)] += m_Matrix[MatIdx(i, k)]* m_MatrixChg[MatIdx(k, j)];
		}

	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
			m_Matrix[MatIdx(i, j)] = m_MatrixTemp[MatIdx(i, j)];
}

moHueSatIntMatrix::moHueSatIntMatrix() : moColorMatrix()
{

	m_PreserveLuminance = true;
}

moHueSatIntMatrix::~moHueSatIntMatrix()
{
	Finish();
}

void moHueSatIntMatrix::Init(MOboolean p_PreserveLuminance,
							 MOfloat p_min_int, MOfloat p_max_int,
							 MOfloat p_min_sat, MOfloat p_max_sat,
							 MOfloat p_min_hue, MOfloat p_max_hue)
{
	moColorMatrix::Init();

	m_PreserveLuminance = p_PreserveLuminance;

	m_min_int = p_min_int;
	m_max_int = p_max_int;

	m_min_sat = p_min_sat;
	m_max_sat = p_max_sat;

	m_min_hue = p_min_hue;
	m_max_hue = p_max_hue;
}

void moHueSatIntMatrix::Finish()
{
	moColorMatrix::Finish();
}

void moHueSatIntMatrix::Update(MOfloat p_int, MOfloat p_sat, MOfloat p_hue)
{
	m_int = m_min_int + p_int * (m_max_int - m_min_int);
	m_sat = m_min_sat + p_sat * (m_max_sat - m_min_sat);
	m_hue = m_min_hue + p_hue * (m_max_hue - m_min_hue);

	m_int = momax(1e-3, m_int);

	IdentityMatrix();
	IntensityMatrix();
	SaturateMatrix();
	if (m_PreserveLuminance) HueRotateMatrix();
	else SimpleHueRotateMatrix();
}

void moHueSatIntMatrix::IntensityMatrix()
{
    m_MatrixChg[MatIdx(0, 0)] = m_int;
    m_MatrixChg[MatIdx(0, 1)] = 0.0;
    m_MatrixChg[MatIdx(0, 2)] = 0.0;
    m_MatrixChg[MatIdx(0, 3)] = 0.0;

    m_MatrixChg[MatIdx(1, 0)] = 0.0;
    m_MatrixChg[MatIdx(1, 1)] = m_int;
    m_MatrixChg[MatIdx(1, 2)] = 0.0;
    m_MatrixChg[MatIdx(1, 3)] = 0.0;

    m_MatrixChg[MatIdx(2, 0)] = 0.0;
    m_MatrixChg[MatIdx(2, 1)] = 0.0;
    m_MatrixChg[MatIdx(2, 2)] = m_int;
    m_MatrixChg[MatIdx(2, 3)] = 0.0;

    m_MatrixChg[MatIdx(3, 0)] = 0.0;
    m_MatrixChg[MatIdx(3, 1)] = 0.0;
    m_MatrixChg[MatIdx(3, 2)] = 0.0;
    m_MatrixChg[MatIdx(3, 3)] = 1.0;

	ApplyChgMatrix();
}

void moHueSatIntMatrix::SaturateMatrix()
{
    MOfloat a, b, c, d, e, f, g, h, i;
	MOfloat rs = (1.0 - m_sat);

    a = (rs * RLUM + m_sat);
    b = (rs * RLUM);
    c = (rs * RLUM);
    d = (rs * GLUM);
    e = (rs * GLUM + m_sat);
    f = (rs * GLUM);
    g = (rs * BLUM);
    h = (rs * BLUM);
    i = (rs * BLUM + m_sat);

    m_MatrixChg[MatIdx(0, 0)] = a;
    m_MatrixChg[MatIdx(0, 1)] = b;
    m_MatrixChg[MatIdx(0, 2)] = c;
    m_MatrixChg[MatIdx(0, 3)] = 0.0;

    m_MatrixChg[MatIdx(1, 0)] = d;
    m_MatrixChg[MatIdx(1, 1)] = e;
    m_MatrixChg[MatIdx(1, 2)] = f;
    m_MatrixChg[MatIdx(1, 3)] = 0.0;

    m_MatrixChg[MatIdx(2, 0)] = g;
    m_MatrixChg[MatIdx(2, 1)] = h;
    m_MatrixChg[MatIdx(2, 2)] = i;
    m_MatrixChg[MatIdx(2, 3)] = 0.0;

    m_MatrixChg[MatIdx(3, 0)] = 0.0;
    m_MatrixChg[MatIdx(3, 1)] = 0.0;
    m_MatrixChg[MatIdx(3, 2)] = 0.0;
    m_MatrixChg[MatIdx(3, 3)] = 1.0;

    ApplyChgMatrix();
}

// Hue rotation that preserves luminance.
void moHueSatIntMatrix::HueRotateMatrix()
{
	float mag;
    float lx, ly, lz;
    float xrs, xrc;
    float yrs, yrc;
    float zrs, zrc;
    float zsx, zsy;

    // rotate the grey vector into positive Z.
    mag = sqrt(2.0);
    xrs = 1.0 / mag;
    xrc = 1.0 / mag;
    SetXRotateMatrix(xrs, xrc);
	ApplyChgMatrix();

    mag = sqrt(3.0);
    yrs = -1.0 / mag;
    yrc = sqrt(2.0) / mag;
    SetYRotateMatrix(yrs, yrc);
	ApplyChgMatrix();

    // shear the space to make the luminance plane horizontal.
    XFormPoint(lx, ly, lz);
    zsx = lx / lz;
    zsy = ly / lz;
	SetZShearMatrix(zsx, zsy);
	ApplyChgMatrix();

    // rotate the hue.
    zrs = sin(m_hue);
    zrc = cos(m_hue);
    SetZRotateMatrix(zrs, zrc);
	ApplyChgMatrix();

    // unshear the space to put the luminance plane back.
    SetZShearMatrix(-zsx, -zsy);
	ApplyChgMatrix();

    // rotate the grey vector back into place.
    SetYRotateMatrix(-yrs, yrc);
	ApplyChgMatrix();
    SetXRotateMatrix(-xrs,xrc);
	ApplyChgMatrix();
}

void moHueSatIntMatrix::SimpleHueRotateMatrix()
{
    MOfloat mag;
    MOfloat xrs, xrc;
    MOfloat yrs, yrc;
    MOfloat zrs, zrc;

    // rotate the grey vector into positive Z.
    mag = sqrt(2.0);
    xrs = 1.0/mag;
    xrc = 1.0/mag;
	SetXRotateMatrix(xrs, xrc);
	ApplyChgMatrix();

    mag = sqrt(3.0);
    yrs = -1.0/mag;
    yrc = sqrt(2.0)/mag;
    SetYRotateMatrix(yrs, yrc);
	ApplyChgMatrix();

    // rotate the hue
    zrs = sin(m_hue);
    zrc = cos(m_hue);
    SetZRotateMatrix(zrs, zrc);
	ApplyChgMatrix();

    // rotate the grey vector back into place.
    SetYRotateMatrix(-yrs, yrc);
	ApplyChgMatrix();
    SetXRotateMatrix(-xrs, xrc);
	ApplyChgMatrix();
}

moBrightContMatrix::moBrightContMatrix() : moColorMatrix()
{
}

moBrightContMatrix::~moBrightContMatrix()
{
	Finish();
}

void moBrightContMatrix::Init(MOfloat p_min_bright, MOfloat p_max_bright,
			                  MOfloat p_min_cont, MOfloat p_max_cont)
{
	moColorMatrix::Init();

	m_min_bright = p_min_bright;
	m_max_bright = p_max_bright;

	m_min_cont = p_min_cont;
	m_max_cont = p_max_cont;
}

void moBrightContMatrix::Finish()
{
	moColorMatrix::Finish();
}

void moBrightContMatrix::Update(MOfloat p_bright, MOfloat p_cont)
{
	m_bright = m_min_bright + p_bright * (m_max_bright - m_min_bright);
	m_cont = m_min_cont + p_cont * (m_max_cont - m_min_cont);

	IdentityMatrix();
	BrightnessMatrix();
	ContrastMatrix();
}

void moBrightContMatrix::BrightnessMatrix()
{
    m_MatrixChg[MatIdx(0, 0)] = 1.0;
    m_MatrixChg[MatIdx(0, 1)] = 0.0;
    m_MatrixChg[MatIdx(0, 2)] = 0.0;
    m_MatrixChg[MatIdx(0, 3)] = 0.0;

    m_MatrixChg[MatIdx(1, 0)] = 0.0;
    m_MatrixChg[MatIdx(1, 1)] = 1.0;
    m_MatrixChg[MatIdx(1, 2)] = 0.0;
    m_MatrixChg[MatIdx(1, 3)] = 0.0;

    m_MatrixChg[MatIdx(2, 0)] = 0.0;
    m_MatrixChg[MatIdx(2, 1)] = 0.0;
    m_MatrixChg[MatIdx(2, 2)] = 1.0;
    m_MatrixChg[MatIdx(2, 3)] = 0.0;

    m_MatrixChg[MatIdx(3, 0)] = m_bright;
    m_MatrixChg[MatIdx(3, 1)] = m_bright;
    m_MatrixChg[MatIdx(3, 2)] = m_bright;
    m_MatrixChg[MatIdx(3, 3)] = 1.0;

    ApplyChgMatrix();
}

void moBrightContMatrix::ContrastMatrix()
{
    m_MatrixChg[MatIdx(0, 0)] = m_cont;
    m_MatrixChg[MatIdx(0, 1)] = 0.0;
    m_MatrixChg[MatIdx(0, 2)] = 0.0;
    m_MatrixChg[MatIdx(0, 3)] = 0.0;

    m_MatrixChg[MatIdx(1, 0)] = 0.0;
    m_MatrixChg[MatIdx(1, 1)] = m_cont;
    m_MatrixChg[MatIdx(1, 2)] = 0.0;
    m_MatrixChg[MatIdx(1, 3)] = 0.0;

    m_MatrixChg[MatIdx(2, 0)] = 0.0;
    m_MatrixChg[MatIdx(2, 1)] = 0.0;
    m_MatrixChg[MatIdx(2, 2)] = m_cont;
    m_MatrixChg[MatIdx(2, 3)] = 0.0;

    m_MatrixChg[MatIdx(3, 0)] = (1.0 - m_cont) * 0.5;
    m_MatrixChg[MatIdx(3, 1)] = (1.0 - m_cont) * 0.5;
    m_MatrixChg[MatIdx(3, 2)] = (1.0 - m_cont) * 0.5;
    m_MatrixChg[MatIdx(3, 3)] = 1.0;

    ApplyChgMatrix();
}

