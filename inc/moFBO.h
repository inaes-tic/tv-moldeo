/*******************************************************************************

                                moFBO.h

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

#ifndef __MO_FBO_H__
#define __MO_FBO_H__

#include "moTypes.h"

/**
 * Clase que encapsula un FrameBuffer Object (FBO) de OpenGL. Este tipo de objeto facilita enormemente
 * las operaciones de render a textura. Varias texturas pueden ser asociadas (attached) como búfers de color
 * a un FBO, de esta manera se puede hacer un render a varias texturas simultáneamente. A un FBO también pueden
 * ser asociados búfers de stencil y depth. De esta manera un FBO puede representar un contexto de render completo
 * donde se pueden llevar a cabo todas las operaciones de dibujo que se efectúan normalmente en el framebuffer de la
 * pantalla. Nota: los términos punto de asociación y búfer de color son utilizados como sinónimos en los comentarios
 * que siguen.
 */
class LIBMOLDEO_API moFBO : public moAbstract
{
    public:
        /**
         * Constructor por defecto.
         */
		moFBO();
        /**
         * Destructor por defecto.
         */
		virtual ~moFBO();

        /**
         * Inicializa el FBO utilizando p_gl como adminstrador de OpenGL.
         * @param p_is_screen_fbo true si este FBO representa el framebuffer de la pantalla.
         * @return true si la operación fue exitosa, false en caso contrario.
         */
		virtual MOboolean Init(MOboolean p_is_screen_fbo);
        /**
         * Finaliza el FBO.
         * @return true si la operación fue exitosa, false en caso contrario.
         */
		virtual MOboolean Finish();

        /**
         * Devuelve el identificador de OpenGL de este FBO.
         * @return identificador de OpenGL.
         */
		MOuint GetGLId() { return m_fbo; }

        /**
         * Devuelve el tipo de FBO encapsulado por este objeto.
         * @return true si este FBO representa el framebuffer de la pantalla, false en caso contrario.
         */
		MOboolean IsScreenFB() { return m_is_screen_fbo; }

        /**
         * Devuelve el estado del FBO.
         * @return estado del FBO.
         */
		MOuint CheckStatus();

        /**
         * Activa este FBO.
         */
		void Bind();

        /**
         * Setea las texturas pasadas en p_dest_tex en cada punto de attachement.
         * @param p_dest_tex es el indice de texturas que seran attacheadas a este FBO.
         */
        void SetDrawTexture(MOuint p_target, MOuint p_glid, MOuint p_attach_point = 0);
        void SetNumDrawTextures(MOuint p_num);


    private:
		MOuint m_fbo;

        MOboolean m_is_screen_fbo;

		MOuint m_num_color_attach_points;
        MOuint m_attach_points_array[MO_MAX_COLOR_ATTACHMENTS_EXT];
        MOuint m_tex_glid_array[MO_MAX_COLOR_ATTACHMENTS_EXT];

        void InitAttachPointsArray();




		GLenum m_target;
        GLint m_internal_format;
		MOuint m_width;
		MOuint m_height;

		MOboolean m_has_depth_buffer;
		MOboolean m_has_stencil_buffer;
		MOuint m_DepthStencilTex;

};

moDeclareExportedDynamicArray( moFBO*, moFBOArray)

#endif
