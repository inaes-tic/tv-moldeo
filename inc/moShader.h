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
  Fabricio Costa
  Andres Colubri

  Description:
  Base class for GLSL and Cg shaders.

*******************************************************************************/

#ifndef __MO_SHADER_H__
#define __MO_SHADER_H__

#include "moTypes.h"
#include "moAbstract.h"
#include "moConfig.h"
#include "moArray.h"
#include "moTextureUtils.h"

typedef enum { MO_SHADER_GLSL, MO_SHADER_CG } moShaderLanguage;

/**
 * Clase base para encapsular shaders de OpenGL. Un programa de shaders tiene tres etapas:
 * el shader de vértices, donde las coordenadas 3D de los vértices son proyectadas a coordenadas homogéneas
 * de pantalla, el shader de geometría, donde nuevos vértices y conectividades entre los mismos pueden ser
 * definidos, y el shader de fragmentos, donde las primitivas geométricas, ya rasterizadas, son dibujadas
 * en la panalla u otro búfer de escritura.
 * Un programa de shader no necesita tener estas tres etapas definidas. Puede consistir sólo en un shader
 * de fragmentos, con lo cual los estadios de vértices y geometría utilizan la funcionalidad por defecto
 * ofrecida por OpenGL en el pipeline gráfico por defecto.
 * Por abuso de terminología, la palabra shader denota a veces el programa de shader completo (incluyendo
 * los estadios de vértices, geometría y fragmentos), mientras que otras veces denota solamente a un estadio
 * particular (shader de vértices, shader de fragmentos, etc.).
 */
class LIBMOLDEO_API moShader : public moAbstract
{
public:
    /**
     * El constructor por defecto de la clase.
     */
    moShader();
    /**
     * El destructor por defecto de la clase.
     */
    virtual ~moShader();

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
     * Crea el shader de vértices a partir del código fuente en vert_source.
     * @param vert_source código fuente del shader de vértices.
     */
    virtual void CreateVertShader(moText vert_source) = 0;
    /**
     * Crea el shader de fragmentos a partir del código fuente en frag_source.
     * @param frag_source código fuente del shader de vértices.
     */
    virtual void CreateFragShader(moText frag_source) = 0;
    /**
     * Crea los shaders de vértices y fragmentos a partir del código fuente en vert_source y frag_source.
     * @param vert_source código fuente del shader de vértices.
     * @param frag_source código fuente del shader de vértices.
     */
    virtual void CreateShader(moText vert_source, moText frag_source) = 0;
    /**
     * Crea el shader de vértices a partir del código fuente en el archivo vert_filename.
     * @param vert_filename nombre del archivo con el código fuente del shader de vértices.
     */
    virtual void LoadVertShader(moText vert_filename) = 0;
    /**
     * Crea el shader de fragmentos a partir del código fuente en el archivo frag_filename.
     * @param frag_filename nombre del archivo con el código fuente del shader de fragmentos.
     */
    virtual void LoadFragShader(moText frag_filename) = 0;
    /**
     * Crea los shaders de vértices y fragmentos a partir del código fuente en los archivos vert_filename y
     * frag_filename.
     * @param vert_filename nombre del archivo con el código fuente del shader de vértices.
     * @param frag_filename nombre del archivo con el código fuente del shader de fragmentos.
     */
    virtual void LoadShader(moText vert_filename, moText frag_filename) = 0;

    /**
     * Activa el shader.
     */
    void StartShader();
    /**
     * Desactiva el shader.
     */
    void StopShader();
    /**
     * Devuelve el estado del shader.
     * @return true si el shader está activo, false en caso contrario.
     */
    MOboolean ShaderActive();

    /**
     * Devuelve el código del último error ocurrido en el shader de vértices.
     * @return código de error.
     */
    MOint VertErrorCode();
    /**
     * Devuelve el código del último error ocurrido en el shader de fragmentos.
     * @return código de error.
     */
    MOint FragErrorCode();

    /**
     * Devuelve el tipo de shader: MO_SHADER_GLSL o MO_SHADER_CG.
     * @return tipo de shader.
     */
	MOuint GetType() { return m_type; }
    /**
     * Establece el tipo de shader.
     * @param p_type nuevo tipo de shader.
     */
	void SetType(MOuint p_type) { m_type = p_type; }
    /**
     * Devuelve el nombre del shader.
     * @return nombre del shader.
     */
	moText GetName() { return m_name; }
    /**
     * Establece el nombre del shader.
     * @param p_name nuevo nombre del shader.
     */
	void SetName(moText p_name) { m_name = p_name; }

    /**
     * Devuelve el objeto de grilla 2D utilizado por este shader.
     * @return objeto de grilla 2D.
     */
	moTexturedGrid GetGrid() { return m_grid; }
    /**
     * Establece el objeto de grilla 2D a utilizar por este shader.
     * @param m_grid nuevo objeto de grilla 2D.
     */
	void SetGrid(moTexturedGrid p_grid) { m_grid = p_grid; }

    /**
     * Devuelve el objeto clipping de textura utilizado por este shader.
     * @return objeto de clipping de textura.
     */
	moTextureClip GetClip() { return m_clip; }
    /**
     * Establece el objeto de clipping de textura a utilizar por este shader.
     * @param m_grid nuevo objeto de grilla 2D.
     */
	void SetClip(moTextureClip p_clip) { m_clip = p_clip; }

    /**
     * Dibuja la grilla asociada a este shader con ancho w y alto w, aplicando las capas hasta la número l.
     * @param w ancho con el que se dibuja la grilla.
     * @param h alto con el que se dibuja la grilla.
     * @param l número de capas de textura a aplicar.
     */
	void DrawGrid(MOint w, MOint h, MOint l) { m_grid.Draw(w, h, l, m_clip); }
protected:
	MOuint m_type;
	moText m_name;

	moTexturedGrid m_grid;
	moTextureClip m_clip;

    MOboolean m_Active;
    MOint m_VertErrorCode;
    MOint m_FragErrorCode;

    virtual void compileVertShader(moText vert_source) = 0;
    virtual void compileFragShader(moText frag_source) = 0;

    moText LoadShaderSource(moText p_fn);
};

moDeclareExportedDynamicArray( moShader*, moShaderArray)

#endif
