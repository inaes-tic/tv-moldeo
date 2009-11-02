/*******************************************************************************

                                moTextureBuffer.h

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

#ifndef __MO_TEXTURE_BUFFER_H__
#define __MO_TEXTURE_BUFFER_H__

#include "moTexture.h"
#include "moFile.h"

typedef moTextureMemory* moTextureFramePtr;
moDeclareExportedDynamicArray(moTextureFramePtr, moTextureFrames)

/**
 * Clase que permite cargar una sucesión de imágenes de mismo formato con compresión incluída dentro
 * de un buffer de memoria y a la vez permite reproducir cada cuadro de forma independiente, descomprimiendo de memoria
 * y asignando una textura
 */
class LIBMOLDEO_API moTextureBuffer : public moAbstract {

	public:
		moTextureBuffer();
		virtual ~moTextureBuffer();

		virtual MOboolean  Init();
		virtual MOboolean  Init( moText p_foldername, moText p_bufferformat, moResourceManager* p_pResourceManager );
		virtual MOboolean  Finish();

		virtual int GetFrame( MOuint p_i );

		virtual void ReleaseFrame( MOuint p_i );

		virtual moTextureMemory* GetTexture( MOuint p_i );

		MOboolean UpdateImages( int maxfiles = -1 );

        MOboolean	LoadCompleted();
        int GetImagesProcessed() { return  m_ImagesProcessed; }

		MOboolean LoadImage( moText p_ImageName, moBitmap* pImage, int indeximage );

		moText GetBufferPath() { return m_BufferPath; }
		moText	GetBufferFormat() { return m_BufferFormat; }

		moText GetName() { return m_FolderName; }

        moTextureFrames& GetBufferLevels( int L, int C );

        int max_luminance;
        int min_luminance;
        int max_contrast;
        int min_contrast;

        //size of max_luminance
        MObyte*   LevelDiagram;

	private:

		MOint m_ImagesProcessed;
		MOboolean	m_bLoadCompleted;
		MOint m_ActualImage;


        moText  m_FolderName;
		moText	m_BufferPath;
		moText  m_BufferFormat;

		moResourceManager*	m_pResourceManager;

		moTextureFrames     m_Frames;
		moDirectory*	m_pDirectory;

		///max level contrast = 10
		///max level luminance = 10
		moTextureFrames**  m_pBufferLevels;
};

typedef moTextureBuffer* moTextureBufferPtr;

moDeclareExportedDynamicArray(moTextureBufferPtr, moTextureBuffers)

#endif
