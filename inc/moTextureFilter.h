/*******************************************************************************

                                moTextureFilter.h

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

#ifndef __MO_TEXTURE_FILTER_H
#define __MO_TEXTURE_FILTER_H

#include "moTypes.h"
#include "moAbstract.h"
#include "moConfig.h"
#include "moParam.h"

#include "moTexture.h"
#include "moTextureIndex.h"
#include "moShader.h"
#include "moShaderGLSL.h"
#include "moTextureFilterParam.h"

class moGLManager;
class moRenderManager;
class moShaderManager;
class moFBManager;

/**
 * Esta clase define un filtro 2D para aplicar en texturas. Un filtro es básicamente
 * un programa de shader (escrito en CG o GLSL) con un conjunto de predefinido de parámetros
 * uniformes y una grilla 2D donde las texturas fuente son mapeadas. Los puntos de la grilla 2D
 * pueden ser modificados en el estadio de vértices del filtro, permitiendo distorsiones arbitrarias
 * en la forma de la texturas.
 */
class LIBMOLDEO_API moTextureFilter : public moAbstract
{
public:
    /**
     * El constructor por defecto del filtro.
     */
	moTextureFilter();

    /**
     * El destructor por defecto del filtro.
     */
	virtual ~moTextureFilter() { Finish(); }

    /**
     * Método de inicialización de la grilla.
     * @param p_glman puntero al administrador de OpenGL.
     * @param p_renderman puntero al administrador de render.
     * @param p_src_tex referencia a la lista de texturas de orígen.
     * @param p_dest_tex referencia a la lista de texturas de destino.
     * @param p_shader puntero al programa de shader que define el filtro.
     * @param p_params puntero al objeto de parámetros a utilizar en este filtro.
     * @return true si la operación fue exitosa, false en caso contrario.
     */
	virtual MOboolean Init(moGLManager* p_glman, moFBManager* p_fbman, moRenderManager* p_renderman, moTextureArray &p_src_tex, moTextureArray &p_dest_tex, moShader *p_shader, moTextureFilterParam *p_params = NULL);
    /**
     * Método de finalización.
     * @return true si la operación fue exitosa, false en caso contrario.
     */
	virtual MOboolean Finish();

    /**
     * Devuelve el identificador Moldeo del filtro.
     * @return Moldeo ID del filtro.
     */
    MOuint GetMOId() { return m_moid; }
    /**
     * Utiliza p_moid como nuevo Moldeo ID del filtro.
     * @param p_moid nuevo Moldeo ID.
     */
    void SetMOId(MOuint p_moid) { m_moid = p_moid; }
    /**
     * Devuelve el nombre del filtro
     * @return nombre del filtro.
     */
    moText GetName() { return m_name; }
    /**
     * Utiliza p_name como nuevo nombre del filtro.
     * @param p_name nuevo nombre del filtro.
     */
    void SetName(moText p_name) { m_name = p_name; }

    /**
     * Aplica el filtro sobre las texturas de orígen y escribiendo el resutlado en las texturas de destino.
     * @param p_i cuadro a utilizar en las texturas de orígen (en el caso de que sean animadas).
     * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
     * @param p_params parámetros del filtro.
     */
	void Apply(MOuint p_i, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);
    /**
     * Aplica el filtro sobre las texturas de orígen y escribiendo el resutlado en las texturas de destino.
     * @param p_cycle fracción del ciclo entre 0.0 y 1.0 a utilizar en las texturas de orígen (en el caso de que sean animadas).
     * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
     * @param p_params parámetros del filtro.
     */
	void Apply(MOfloat p_cycle, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);
    /**
     * Aplica el filtro sobre las texturas de orígen y escribiendo el resutlado en las texturas de destino.
     * @param p_tempo tempo a utilizar en las texturas de orígen (en el caso de que sean animadas).
     * @param p_fade constante de "fading" utilizada para mezclar el resultado del filtro.
     * @param p_params parámetros del filtro.
     */
	void Apply(moTempo *p_tempo, MOfloat p_fade = 1.0, moTextureFilterParam *p_params = NULL);

    /**
     * Devuelve el puntero a la lista de texturas de orígen.
     * @return puntero a la lista de texturas de orígen.
     */
	moTextureIndex* GetSrcTex() { return &m_src_tex; }
    /**
     * Devuelve el puntero a la lista de texturas de destino.
     * @return puntero a la lista de texturas de destino.
     */
	moTextureIndex* GetDestTex() { return &m_dest_tex; }
    /**
     * Devuelve el puntero al shader que define este filtro.
     * @return puntero al shader.
     */
	moShader* GetShader() { return m_shader; }

protected:
	moShader* m_shader;
	moRenderManager* m_renderman;
	moGLManager* m_glman;
	moFBManager* m_fbman;

	MOuint m_dest_fbo;

    MOint m_moid;
    moText m_name;

	// Shader parameters.
	GLint m_src_tex_unit[MO_MAX_TEXTURE_UNITS];
	GLint m_src_tex_offset[MO_MAX_TEXTURE_UNITS];
	GLint m_tempo_angle;
	GLint m_fade_const;
	GLint m_dest_tex_size;

	moTextureFilterParam *m_DefParams;

	// Texture indices.
	moTextureIndex m_src_tex;
	moTextureIndex m_dest_tex;

	GLboolean m_blend_on;
	GLint m_poly_mode;
	GLenum m_draw_buffers[MO_MAX_COLOR_ATTACHMENTS_EXT];

	MOboolean m_use_screen_tex;
	MOboolean m_reattach_dest_tex;

	void SetupShader(MOint w, MOint h, moTempo *p_tempo, MOfloat p_fade, moTextureFilterParam *p_params);
	void SetGLConf(MOint w, MOint h);
	void RestoreGLConf();
	void BindDestFBO();
	void UnbindDestFBO();
	void BindDestTexToFBO();
	void SetDestTexInFBO();

	void BindSrcTex(MOuint p_i);
	void BindSrcTex(MOfloat p_cycle);
	void BindSrcTex(moTempo *p_tempo);
	void UnbindSrcTex();
	void RenderTexQuad(MOint w, MOint h);
};

moDeclareExportedDynamicArray( moTextureFilter*, moTextureFilterArray)

#endif
