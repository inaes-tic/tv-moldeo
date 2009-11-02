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


#ifndef __MO_TEXTUREFILTER_MANAGER_H
#define __MO_TEXTUREFILTER_MANAGER_H

#include "moTextureFilter.h"
#include "moResourceManager.h"

/**
 * Clase que implementa un administrador de filtros de textura.
 */
class LIBMOLDEO_API moTextureFilterManager : public moResource
{
    public:

        /**
         * Constructor genérico de la clase.
         */
		moTextureFilterManager();

        /**
         * Destructor genérico de la clase.
         */
		~moTextureFilterManager() {}

        /**
         * Método de inicialización.
         * @return true si la operación fue exitosa, false en caso contrario.
         */
		virtual MOboolean Init();

        /**
         * Método de finalización.
         * @return true si la operación fue exitosa, false en caso contrario.
         */
		virtual MOboolean Finish();

        /**
         * Devuelve el filtro de textura con los nombres de texturas de entrada, salida y shader especificados. Si el
         * filtro no es encontrado entonces un nuevo filtro es creado (los nombres son interpretados como nombres de
         * archivos para cargar las texturas y shaders que no esten presentes todavia en los administradores
         * correspondientes) de acuerdo al valor de p_create_filter. El orden de los elementos en los arrays de nombres
         * de texturas es importante: arrays con los mismos nombres pero en distinto orden son considerados distintos.
         * @param p_src_tex_names nombres de las texturas fuente o de entrada.
         * @param p_dest_tex_names nombres de des texturas destino o de salida.
         * @param p_shader_name nombre del shader.
         * @param p_create_filter si es true entonces un nuevo filtro es creado si no existe uno con los parametros especificados.
         * @return índice del filtro buscado.
         */
	    MOint GetTextureFilterMOid(moTextArray p_src_tex_names, moTextArray p_dest_tex_names, moText p_shader_name, MOboolean p_create_filter);

        MOint AddTextureFilter(moTextArray p_src_tex_names, moTextArray p_dest_tex_names, moText p_shader_name);

        /**
         * Agrega un filtro de textura conformado por un grupo de texturas de entrada, un shader y un grupo de
         * texturas de salida.
         * @param p_src_tex array de texturas fuente o de entrada.
         * @param p_dest_tex array de texturas destino o de salida.
         * @param p_shader puntero al shader que opera sobre estas texturas.
         * @param p_params parametros del filtro (opcional).
         * @return índice del nuevo filtro.
         */
		MOint AddTextureFilter(moTextureArray &p_src_tex, moTextureArray &p_dest_tex, moShader *p_shader, moTextureFilterParam *p_params = NULL);


        /**
         * Aplica el filtro p_idx sobre sus texturas de orígen y escribiendo el resultado en sus texturas de destino.
         * @param p_idx índice del filtro a aplicar.
         * @param p_i cuadro a utilizar en las texturas de orígen (en el caso de que sean animadas).
         * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
         * @param p_params parámetros del filtro.
         */
	    void Apply(MOuint p_idx, MOuint p_i, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);
        /**
         * Aplica el filtro p_idx sobre sus texturas de orígen y escribiendo el resultado en sus texturas de destino.
         * @param p_idx índice del filtro a aplicar.
         * @param p_cycle fracción del ciclo entre 0.0 y 1.0 a utilizar en las texturas de orígen (en el caso de que sean animadas).
         * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
         * @param p_params parámetros del filtro.
         */
	    void Apply(MOuint p_idx, MOfloat p_cycle, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);
        /**
         * Aplica el filtro p_idx sobre sus texturas de orígen y escribiendo el resultado en sus texturas de destino.
         * @param p_idx índice del filtro a aplicar.
         * @param p_tempo tempo a utilizar en las texturas de orígen (en el caso de que sean animadas).
         * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
         * @param p_params parámetros del filtro.
         */
	    void Apply(MOuint p_idx, moTempo *p_tempo, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);

       /**
        * Elimina el filtro de textura con índice p_idx.
        * @param p_idx índice del filtro a eliminar.
        * @return true si la operación fue exitosa, false en caso contrario.
        */
	    MOboolean DeleteTextureFilter(MOint p_idx);

        /**
         * Devuelve el número de filtros de textura.
         * @return número filtros.
         */
	    MOuint GetTextureFilterCount() { return m_filters_array.Count(); }

        /**
         * Verifica el índice p_idx.
         * @param p_idx índice de filtro.
         * @return true si el índice es válido, false en caso contrario.
         */
	    MOboolean ValidIndex(MOuint p_idx);

        /**
         * Devuelve un puntero al filtro de textura con índice p_idx.
         * @param p_idx índice del filtro.
         * @return puntero al filtro solicitado.
         */
        moTextureFilter* GetTextureFilter(MOuint p_idx) { return m_filters_array[p_idx]; }
    private:
	    moGLManager*     m_glmanager;
	    moFBManager*     m_fbmanager;
	    moRenderManager* m_rendermanager;

        moTextureFilterArray m_filters_array;
};

#endif
