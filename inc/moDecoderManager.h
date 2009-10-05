/*******************************************************************************

                                moDecoderManager.h

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

*******************************************************************************/

#ifndef __MO_DECODERMANAGER_H__
#define __MO_DECODERMANAGER_H__

#include "moTypes.h"
#include "moText.h"
#include "moLock.h"
#include "moFBO.h"
#include "moTexture.h"
#include "moVideoGraph.h"
#include "moResourceManager.h"


typedef void moDecoderDevice;
typedef void moVideoMixer;
typedef void moVideoDecoder;


enum moDecoderType {

    MO_DECODERTYPE_VDPAU,
    MO_DECODERTYPE_CUDA,
    MO_DECODERTYPE_OPENCL

};


class moDecoderPlaylist {


  moDecoderPlaylist();
  virtual ~moDecoderPlaylist();



};


/**
 * Esta clase implementa un administrador para la API de VDPAU de reproducción de videos
 * que hace uso en LINUX del hardware específico de NVidia con la arquitectura de chips VP1/VP2
 * Esta librería permite reproducir archivos de tipo MPEG2, H264, WMV, VC1, total o parcialmente
 * acelerados por esta librería.

 */
class LIBMOLDEO_API moDecoderManager : public moResource
{
    public:
        /**
         * Constructor por defecto.
         */
		moDecoderManager();
        /**
         * Destructor por defecto.
         */
		virtual ~moDecoderManager();

        /**
         * Inicializador del administrador.
         */
		virtual MOboolean Init();
        /**
         * Finalizador del administrador.
         */
		virtual MOboolean Finish();

    moDecoderDevice*        GetDecoderDevice();
    moVideoDecoder*         GetVideoDecoder();
    moVideoMixer*           GetVideoMixer();

    void SetDecoderType( moDecoderType type );
    moDecoderType GetDecoderType();


    private:

		moGLManager*            m_glmanager;
		moDecoderType           m_DecoderType;
    moDecoderDevice*        m_pDecoderDevice;
    moVideoDecoder*         m_pVideoDecoder;
    moVideoMixer*           m_pVideoMixer;

    moVideoGraph*           m_pVideoGraph;

};

#include "moGLManager.h"

#endif
