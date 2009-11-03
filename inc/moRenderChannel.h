/*******************************************************************************

                                moRenderChannel.h

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
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

#ifndef __MO_RENDER_CHANNEL_H__
#define __MO_RENDER_CHANNEL_H__

#include "moTypes.h"

// Funcionalidad necesaria:
// * mecanismo para render stereo: las 4 texturas estan duplicadas, y se dibuja en cada una de ellas de manera alternada
// (cada efecto tiene que dibujar dos veces, una para la vista R y otra para la vista L). O sea que el rendermanager
// necesita una variable de canal (la idea de canal puede abstraerse del contexto de rendereado stereo y tener n canales).
// Cuando se esta renderando un canal, las referencias a las texturas MO_RENDER_TEX, MO_SCREEN_TEX, etc. deben regresar
// la textura del canal activo.
// * Clipping y filtering de cada textura final (L y R), a una (o unas) texturas de OUTPUT. Esto para deformacion y
// proyeccion sobre superficies no planas (planetarios digitales, etc). Tambien para postprocessing de la imagen final.


// Dos maneras para render stereo:


// 1) se cambia R o L cuando se dibuja cada efecto
// Loop de dibujo estereo:
//   por cada efecto
//    for mode = R to L do
//         beginDraw()
//            efecto dibuja cuadro R o L
//         endDraw()


// 2) Se hace todo el render de todos los efectos primero para un canal y luego para el otro.
// Loop de dibujo estereo:
//   for mode = R to L do
//     por cada efecto
//         beginDraw()
//            efecto dibuja cuadro R o L
//         endDraw()

class moTextureFilterManager;
class moTextureManager;
class moFBManager;

enum moRenderChannelModes
{
    RENDERCHANNEL_MONO = 0,
    RENDERCHANNEL_STEREO_LEFT = 1,
    RENDERCHANNEL_STEREO_RIGHT = 2,
    RENDERCHANNEL_MULTI_GENERIC = 3,
};

class LIBMOLDEO_API moResolution
{
public:
    MOint width;
    MOint height;
};

// Render textures:
// MO_RENDER_TEX el resultado del ultimo render
// MO_COPY_TEX el resultado del ultimo render
// MO_EFFECTS_TEX luego de todos los pre-efectos y efectos (pero no los post-efectos)
// MO_FINAL_TEX
class LIBMOLDEO_API moRenderChannel : public moAbstract
{
public:

    // Carga el config del channel, inicialize texturas de render y salida,
    // filtros asociados, contexto de rendering (fbo, stencil, etc).
    // multisampling si esta habilitado.
    void Init();

    // Binds the render texture tex (where tex is MO_RENDER_TEX, MO_COPY_TEX, etc) to the color attachement of the channel's fbo.
    void SetRenderTexture(MOint tex);

    // Applies the texture filters that generate the output textures from the render textures.
    void GenerateOutput();

    // Copies the render texture (MO_RENDER_TEX) to copy texture (MO_COPY_TEX).
    void SaveRender();

    moTexure* GetRenderTexture(int idx);
    moTexure* GetOutputTexture(int idx);
protected:
    moTextureFilterManager*
    moTextureManager*
    moFBManager* m_fbmanager;

    MOint m_fbo;

    moTextureArray m_render_textures;
    moTextureArray m_output_textures;

    moResolution m_render_res;
    moResolution m_output_res;


    // Stencil buffers, etc.
		GLenum m_target;
        GLint m_internal_format;
		MOuint m_width;
		MOuint m_height;

		MOboolean m_has_depth_buffer;
		MOboolean m_has_stencil_buffer;
		MOuint m_DepthStencilTex;

};

moDeclareExportedDynamicArray(moRenderChannel*, moRenderChannelArray)

#endif
