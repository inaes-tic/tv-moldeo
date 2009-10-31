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
  Clases para definir una región de clipping en el esapcio de coordenadas de
  textura ST, y una grilla texturada.

*******************************************************************************/

#ifndef __MO_TEXTUREDGRID_H__
#define __MO_TEXTUREDGRID_H__

#include "moTypes.h"
#include "moAbstract.h"
#include "moConfig.h"

#define MO_MAX_TEXTURE_UNITS 4

/**
 * Esta clase define una subregión rectangular en el espacio ST de coordenadas de texturas.
 */
class LIBMOLDEO_API moTextureClip
{
public:
    /**
     * El constructor por defecto de la clase.
     */
    moTextureClip();
    /**
     * El destructor por defecto de la clase.
     */
	virtual ~moTextureClip();

    /**
     * Método de inicialización del clip.
     * @param s0 es la coordenada S de textura donde comienza la región de clipping.
     * @param s1 es la coordenada S de textura donde termina la región de clipping.
     * @param t0 es la coordenada T de textura donde comienza la región de clipping.
     * @param t1 es la coordenada T de textura donde termina la región de clipping.
     * @return true si la operación fue exitosa, false en caso contrario.
     */
    MOboolean Init(MOfloat s0, MOfloat s1, MOfloat t0, MOfloat t1);
    /**
     * Método de inicialización.
     * @param p_cfg puntero al objeto de configuración que contiene los parámetros de la grilla.
     * @param p_param_idx índice de los parámetros de la grilla en el objeto de configuración.
     * @return true si la operación fue exitosa, false en caso contrario.
     */
    MOboolean Init(moConfig* p_cfg, MOuint p_param_idx);

    /**
     * Método de finalización.
     * @return true si la operación fue exitosa, false en caso contrario.
     */
	virtual MOboolean Finish();

    /**
     * Sets the clipping region to cover the entire texture plane [0,1]x[0,1].
     */
    void SetEntireTexClip();

    /**
     * Devuelve el valor inicial de clipping en la coordenada S.
     * @return s0.
     */
    MOfloat GetS0() { return s0; }
    /**
     * Devuelve el valor final de clipping en la coordenada S.
     * @return s1.
     */
    MOfloat GetS1() { return s1; }

    /**
     * Devuelve el valor inicial de clipping en la coordenada T.
     * @return t0.
     */
    MOfloat GetT0() { return t0; }
    /**
     * Devuelve el valor final de clipping en la coordenada T.
     * @return t1.
     */
    MOfloat GetT1() { return t1; }

    /**
     * Mapea el valor de coordenada de textura s en [0, 1] a la
     * región de clipping [s0, s1].
     * @param s en [0, 1]
     * @return valor mapeado a [s0, s1]
     */
    MOfloat MapS(MOfloat s) { s0 + s / (s1 - s0); }
    /**
     * Mapea el valor de coordenada de textura t en [0, 1] a la
     * región de clipping [t0, t1].
     * @param t en [0, 1]
     * @return valor mapeado a [t0, t1]
     */
    MOfloat MapT(MOfloat t) { t0 + t / (t1 - t0); }

    /**
     * Copia los datos desde el clip p_src_clip
     * @param p_src_clip clip fuente para la copia.
     * @return referencia a este clip.
     */
    moTextureClip &operator = (const moTextureClip &p_src_clip);
protected:
    MOfloat s0, s1;
    MOfloat t0, t1;
};

/**
 * Esta clase define una grilla 2D donde mapear y distorsionar texturas. La grilla
 * está definida por el número de divisiones (número de puntos - 1) a lo largo de las direcciones X e Y.
 * En principio, el delta en el espacio de coordenadas de texturas entre un punto de la grilla y el
 * siguiente es igual a 1 / divisiones, pero este delta puede ser modificado para que adopte un valor
 * arbitrario dx, dy a lo largo de cada dirección. Asimismo, como se pueden aplicar texturas múltiples
 * sobre un a misma grilla, de pueden definir varios deltas (dx_i, dy_i) con i = 1... n, donde n es el
 * número de texturas (o capas) que se aplican sobre la grilla.
 */
class LIBMOLDEO_API moTexturedGrid : public moAbstract
{
public:
    /**
     * El constructor por defecto de la clase.
     */
	moTexturedGrid();
    /**
     * El destructor por defecto de la clase.
     */
	virtual ~moTexturedGrid();

    /**
     * Método de inicialización de la grilla.
     * @param p_size_x número de puntos en la dirección X.
     * @param p_size_y número de puntos en la dirección Y.
     * @param p_num_layers número de capas.
     * @param p_grid_dx deltas para las texturas en la dirección de X.
     * @param p_grid_dy deltas para las texturas  en la dirección de Y.
     * @return true si la operación fue exitosa, false en caso contrario.
     */
	virtual MOboolean Init(MOint p_size_x, MOint p_size_y, MOint p_num_layers, const MOfloat p_grid_dx[], const MOfloat p_grid_dy[]);
    /**
     * Método de inicialización.
     * @param p_cfg puntero al objeto de configuración que contiene los parámetros de la grilla.
     * @param p_param_idx índice de los parámetros de la grilla en el objeto de configuración.
     * @return true si la operación fue exitosa, false en caso contrario.
     */
	virtual MOboolean Init(moConfig* p_cfg, MOuint p_param_idx);
    /**
     * Método de finalización.
     * @return true si la operación fue exitosa, false en caso contrario.
     */
	virtual MOboolean Finish();

    /**
     * Crea una grilla de 1x1 con una sola capa de textura.
     */
	void Set1QuadGrid();

    /**
     * Dibuja la grilla con ancho w y alto w, aplicando las capas hasta la número l y haciendo
     * el clipping the coordenadas de texturas indicado en clip.
     * @param w ancho con el que se dibuja la grilla.
     * @param h alto con el que se dibuja la grilla.
     * @param l número de capas de textura a aplicar.
     * @param clip que define la región del espacio ST a utilizar.
     */
	void Draw(MOint w, MOint h, MOint l, moTextureClip &clip);

    /**
     * Devuelve el número de puntos en la dirección X.
     * @return número de puntos.
     */
	MOint GetWidth() { return m_size_x; }
    /**
     * Devuelve el número de puntos en la dirección Y.
     * @return número de puntos.
     */
	MOint GetHeight() { return m_size_y; }
    /**
     * Devuelve el punto (i, j) en la capa i. La capa 0 corresponde a las coordenadas de la
     * grilla propiamente dicha. Las coordenadas de las capas subsiguientes son los deltas de
     * de las texturas.
     * @param layer capa de donde se requiere el punto.
     * @param i índice del punto en la dirección X.
     * @param j índice del punto en la dirección Y.
     * @param x en esta variable pasada por referencia se devuelve la coordenada X del punto.
     * @param y en esta variable pasada por referencia se devuelve la coordenada Y del punto.
     */
	void GetPoint(MOint layer, MOint i, MOint j, MOfloat &x, MOfloat &y);
    /**
     * Establece las coordenadas de texturas hasta la capa l, para el punto (i, j) de la grilla,
     * haciendo el clipping especificado.
     * @param i índice del punto de la grilla a lo largo de la dirección X.
     * @param j índice del punto de la grilla a lo largo de la dirección Y.
     * @param l número de capas de textura a establecer las coordenadas.
     * @param clip que define la región del espacio ST a utilizar.
     */
	void SetTexCoord(MOint i, MOint j, MOint l, moTextureClip &clip);

    /**
     * Copia los datos desde la grilla p_src_grid.
     * @param p_src_grid grilla fuente para la copia.
     * @return referencia a esta grilla.
     */
	moTexturedGrid &operator = (const moTexturedGrid &p_src_grid);
protected:
	// Number of points in the grid, along each direction.
	MOint m_size_x;
	MOint m_size_y;

	// The layer 0 stores the actual grid, the other layers store the texture coordinates.
	MOint m_num_layers;
	MOfloat m_grid_dx[MO_MAX_TEXTURE_UNITS + 1];
	MOfloat m_grid_dy[MO_MAX_TEXTURE_UNITS + 1];
};

#endif
