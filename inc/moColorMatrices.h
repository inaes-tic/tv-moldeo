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

#ifndef __MO_COLOR_MATRICES_H
#define __MO_COLOR_MATRICES_H

#include "moTypes.h"
#include "moAbstract.h"
#include "moConfig.h"
#include "moParam.h"

#define RLUM    ((float) (0.3086))
#define GLUM    ((float) (0.6094))
#define BLUM    ((float) (0.0820))

// Base class to implement color transformation matrices.
class LIBMOLDEO_API moColorMatrix
{
public:
	moColorMatrix();
	virtual ~moColorMatrix();

	virtual void Init();
	virtual void Finish();

	void Copy(moColorMatrix& p_mat);
	void Multiply(moColorMatrix& p_mat);
	void SetZero() { ZeroMatrix(); }
	void SetIndentity() { IdentityMatrix(); }

	MOfloat* GetMatrixPointer() { return m_Matrix; }
protected:
	MOfloat m_Matrix[16];
	MOfloat m_MatrixChg[16];
	MOfloat m_MatrixTemp[16];

	int MatIdx(int i, int j) { return j * 4 + i; }

	void ZeroMatrix();
	void IdentityMatrix();

	void SetXRotateMatrix(float rs, float rc);
	void SetYRotateMatrix(float rs, float rc);
	void SetZRotateMatrix(float rs, float rc);
    void SetZShearMatrix(float dx, float dy);
    void XFormPoint(float &tx, float &ty, float &tz);

	void ApplyChgMatrix();
};

// Class that generates a Hue/Staturation/Intensity matrix
// Based on the Hue/Saturation/Intensity Filter for VirtualDub
// written by Donald A. Graft
// Some useful explanations on Matrix Operations for Image Processing
// can be found here:
// http://www.graficaobscura.com/matrix/index.html
class LIBMOLDEO_API moHueSatIntMatrix : public moColorMatrix
{
public:
	moHueSatIntMatrix();
	~moHueSatIntMatrix();

	void Init(MOboolean p_PreserveLuminance = true,
			MOfloat p_min_int = 0.0, MOfloat p_max_int = 2.0,
			MOfloat p_min_sat = 0.0, MOfloat p_max_sat = 2.0,
			MOfloat p_min_hue = 0.0, MOfloat p_max_hue = 2 * moMathf::PI);
	void Finish();

	// Updates the matrix using the provided intensity, saturation and
	// hue values (all in [0, 1]).
	void Update(MOfloat p_int, MOfloat p_sat, MOfloat p_hue);
protected:
	MOboolean m_PreserveLuminance;
	MOfloat m_int, m_sat, m_hue;
	MOfloat m_min_int, m_max_int;
	MOfloat m_min_sat, m_max_sat;
	MOfloat m_min_hue, m_max_hue;

	void IntensityMatrix();
	void SaturateMatrix();
	void HueRotateMatrix();
	void SimpleHueRotateMatrix();
};

// Class that generates a Brightness/Contrast matrix.
class LIBMOLDEO_API moBrightContMatrix : public moColorMatrix
{
public:
	moBrightContMatrix();
	~moBrightContMatrix();

	void Init(MOfloat p_min_bright = 0.0, MOfloat p_max_bright = 2.0,
			  MOfloat p_min_cont = 0.0, MOfloat p_max_cont = 2.0);
	void Finish();

	void Update(MOfloat p_bright, MOfloat p_cont);
protected:
	MOfloat m_bright, m_cont;
	MOfloat m_min_bright, m_max_bright;
	MOfloat m_min_cont, m_max_cont;

	void BrightnessMatrix();
	void ContrastMatrix();
};

#endif
