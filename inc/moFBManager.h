/*******************************************************************************

                                moFBManager.h

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

#ifndef __MO_FBMANAGER_H__
#define __MO_FBMANAGER_H__

#include "moTypes.h"
#include "moText.h"
#include "moLock.h"
#include "moFBO.h"
#include "moTexture.h"
#include "moResourceManager.h"

/**
 * Esta clase implementa un administrador de framebuffers. Existen dos tipos de framebuffers:
 * el framebuffer de la pantalla, que es el utilizado por defecto por OpenGL para todas las operaciones
 * de render que terminan siendo visualizadas en la pantalla, y los FBO (FrameBuffer Objects) que permiten
 * contextos de render fuera de pantalla.
 * Cada vez que un framebuffer es activado (bind), el que estaba activo hasta ese momento es guardado de manera
 * tal que pueda ser restaurado cuando al llamar a la función de desactivación (unbind).

 */
class LIBMOLDEO_API moFBManager : public moResource
{
    public:
        /**
         * Constructor por defecto.
         */
		moFBManager();
        /**
         * Destructor por defecto.
         */
		virtual ~moFBManager();

        /**
         * Inicializador del administrador.
         */
		virtual MOboolean Init();
        /**
         * Finalizador del administrador.
         */
		virtual MOboolean Finish();

        /**
         * Agrega un nuevo FBO.
         * @return índice del nuevo FBO.
         */
        MOint AddFBO(MOboolean p_screen_fbo = false);
        /**
         * Elimina el FBO con índice p_fbo.
         * @param p_fbo índice del FBO a eliminar.
         * @return true en caso de que el FBO se haya podido eliminar, false en caso contrario.
         */
		MOboolean DeleteFBO(MOuint p_fbo);
        /**
         * Devuelve el número de FBOs.
         * @return número de FBOs.
         */
		MOuint GetFBOCount() { return m_fbo_array.Count(); }
        /**
         * Devuelve un puntero al FBO con índice p_fbo.
         * @param p_fbo índice del FBO.
         * @return puntero al FBO.
         */
		moFBO* GetFBO(MOuint p_fbo) { return m_fbo_array.Get(p_fbo); }

		MOboolean BindFBO(MOuint p_fbo);
    private:
		moGLManager* m_glmanager;

		moFBOArray m_fbo_array;
};

#include "moGLManager.h"

#endif
