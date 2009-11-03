/*******************************************************************************

                             moTextureFilterIndex.h

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
  Andr�s Colubri

*******************************************************************************/

#ifndef __MO_TEXTURE_FILTER_INDEX_H
#define __MO_TEXTURE_FILTER_INDEX_H

#include "moTypes.h"
#include "moAbstract.h"
#include "moConfig.h"

#include "moTextureFilter.h"

class moTextureFilterManager;

/**
 * Clase que encapsula una lista de punteros a objetos moTextureFilter. La utilidad de esta clase es que
 * permite definir un subconjunto de filtros de textura y accederlos de manera directa.
 */
class LIBMOLDEO_API moTextureFilterIndex : public moAbstract
{
public:
    /**
     * Constructor por defecto.
     */
	moTextureFilterIndex();
    /**
     * Destructor por defecto.
     */
	virtual ~moTextureFilterIndex();

    /**
     * M�todo de inicializaci�n.
     * @param p_param_idx �ndice en el objeto de configuraci�n donde se encuentran los filtros.
     * @param p_texfilterman puntero al administrador de filtros de textura.
     * @return resultado de la operaci�n: true si fue exitosa, false en caso contrario.
     */
    virtual MOboolean Init( moParam* p_param, moTextureFilterManager* p_texfilterman);

    /**
     * M�todo de inicializaci�n.
     * @param p_texfilterman puntero al administrador de filtros de textura.
     * @return resultado de la operaci�n: true si fue exitosa, false en caso contrario.
     */
	virtual MOboolean Init(moTextureFilterManager* p_texfilterman);

    /**
     * M�todo de inicializaci�n.
     * @param p_cfg puntero al objeto de configuraci�n de Moldeo donde se especifica la lista de filtros.
     * @param p_param_idx �ndice en el objeto de configuraci�n donde se encuentran los filtros.
     * @param p_texfilterman puntero al administrador de filtros de textura.
     * @return resultado de la operaci�n: true si fue exitosa, false en caso contrario.
     */
	virtual MOboolean Init(moConfig* p_cfg, MOuint p_param_idx, moTextureFilterManager* p_texfilterman);
    /**
     * M�todo de inicializaci�n.
     * @param p_filters_str lista de expresiones que definen los filtros a agregar.
     * @param p_texfilterman puntero al administrador de filtros de textura.
     * @return resultado de la operaci�n: true si fue exitosa, false en caso contrario.
     */
	virtual MOboolean Init(moTextArray* p_filters_str, moTextureFilterManager* p_texfilterman);
    /**
     * M�todo de finalizaci�n.
     * @return resultado de la operaci�n: true si fue exitosa, false en caso contrario.
     */
	virtual MOboolean Finish();

    /**
     * Agrega el filtro apuntado por p_filter.
     * @return �ndice del nuevo filtro.
     */
	MOint Add(moTextureFilter* p_filter);

    /**
     * Aplica el filtro p_idx sobre las texturas de or�gen y escribiendo el resutlado en las texturas de destino.
     * @param p_idx �ndice del filtro a aplicar.
     * @param p_i cuadro a utilizar en las texturas de or�gen (en el caso de que sean animadas).
     * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
     * @param p_params par�metros del filtro.
     */
	void Apply(MOuint p_idx, MOuint p_i, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);
    /**
     * Aplica el filtro p_idx sobre las texturas de or�gen y escribiendo el resutlado en las texturas de destino.
     * @param p_idx �ndice del filtro a aplicar.
     * @param p_cycle fracci�n del ciclo entre 0.0 y 1.0 a utilizar en las texturas de or�gen (en el caso de que sean animadas).
     * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
     * @param p_params par�metros del filtro.
     */
	void Apply(MOuint p_idx, MOfloat p_cycle, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);
    /**
     * Aplica el filtro p_idx sobre las texturas de or�gen y escribiendo el resutlado en las texturas de destino.
     * @param p_idx �ndice del filtro a aplicar.
     * @param p_tempo tempo a utilizar en las texturas de or�gen (en el caso de que sean animadas).
     * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
     * @param p_params par�metros del filtro.
     */
	void Apply(MOuint p_idx, moTempo *p_tempo, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);

    /**
     * Aplica todos los filtros sobre las texturas de or�gen y escribiendo el resutlado en las texturas de destino.
     * @param p_tempo tempo a utilizar en las texturas de or�gen (en el caso de que sean animadas).
     * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
     * @param p_params par�metros del filtro.
     */
	void Apply(moTempo *p_tempo, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);

    /**
     * Verifica el �ndice p_idx.
     * @param p_idx �ndice de filtro.
     * @return true si el �ndice es v�lido, false en caso contrario.
     */
	MOboolean ValidIndex(MOuint p_idx);

    /**
     * Devuelve el n�mero de filtros.
     * @return n�mero de filtros.
     */
	MOint Count() { return m_filters_array.Count(); }

    /**
     * Devuelve un puntero al filtro con �ndice p_idx.
     * @param p_idx �ndice de filtro.
     * @return puntero al filtro requerido.
     */
	moTextureFilter* Get(MOuint p_idx) { return m_filters_array[p_idx]; }

    /**
     * Devuelve un puntero al filtro con �ndice p_idx.
     * @param p_idx �ndice de filtro.
     * @return puntero al filtro requerido.
     */
    moTextureFilter* operator [](MOuint p_idx) { return m_filters_array[p_idx]; }
protected:
	moTextureFilterManager* m_texfilterman;

	moTextureFilterArray m_filters_array;

	MOuint LoadFilter(moValue* p_value);
	MOuint LoadFilters(moParam* p_param);
	MOuint LoadFilters(moConfig* p_cfg, MOuint p_param_idx);

	// Nota importante: los elementos del nombre de un filtro deben estar separados por '|' no por
	// espacios!!!
	MOuint LoadFilters(moTextArray* p_filters_str);
};

#endif
