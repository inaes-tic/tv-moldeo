/*******************************************************************************

                                moTextureBuffer.cpp

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

#include "moTextureBuffer.h"
#include "moResourceManager.h"
#include "FreeImage.h"

#include "moArray.cpp"
moDefineDynamicArray(moTextureFrames)
moDefineDynamicArray(moTextureBuffers)

//===========================================
//
//				moTextureBuffer
//
//===========================================

moTextureBuffer::moTextureBuffer() {

	//m_type = MO_TYPE_TEXTUREBUFFER;
	m_ImagesProcessed = 0;
	m_ActualImage = 0;
	m_pResourceManager = NULL;
	m_pDirectory = NULL;
	m_bLoadCompleted = false;
	m_pBufferLevels = new moTextureFrames* [100];
	for( int i=0; i<100; i++ ) {
	    m_pBufferLevels[i] = new moTextureFrames [10];
	    for( int j = 0; j<10; j++ ) {
            m_pBufferLevels[i][j].Init( 0 , NULL );
        }
	}

	LevelDiagram = NULL;
    ///size of max_luminance: 100
    ///how many levels: 100
    LevelDiagram = new MObyte [ 100 * 100 * 3];

}

moTextureBuffer::~moTextureBuffer() {
	Finish();
}

MOboolean  moTextureBuffer::Init() {

	if (!m_pResourceManager)
		return false;



	return m_bInitialized;
}

MOboolean
moTextureBuffer::Init( moText p_foldername, moText p_bufferformat, moResourceManager* p_pResourceManager ) {
	m_pResourceManager = p_pResourceManager;

	m_FolderName = p_foldername;
	m_BufferPath = m_pResourceManager->GetDataMan()->GetDataPath() + (moText)p_foldername;
	m_BufferFormat = p_bufferformat;


	m_pDirectory = m_pResourceManager->GetFileMan()->GetDirectory( m_BufferPath );

	if (!m_pDirectory) return false;

	//BuildEmpty( width, height);

	/*
	moShaderManager* SM = m_pResourceManager->GetShaderMan();
	moTextureManager* TM = m_pResourceManager->GetTextureMan();

	m_pShaderCopy = SM->GetShader(SM->GetShaderMOId(moText("shaders/Copy.cfg"),true) );
	*/
	m_Frames.Init( 0, NULL);

    max_luminance = 0;//100
    min_luminance = 100;//0

    max_contrast = 0;
    min_contrast = 0;

    for( int L=0; L<100*100*3; L++) {
        LevelDiagram[L] = 0;
    }

	m_bInitialized = true;

	return Init();
}

MOboolean  moTextureBuffer::Finish() {
	for(MOuint i=0; i<m_Frames.Count(); i++) {
		moTextureMemory* pTextureMemory = m_Frames[i];
		if (pTextureMemory)
			pTextureMemory->Finish();
		delete pTextureMemory;
	}
	m_Frames.Empty();

	for(int j=0 ; j<100; j++) {
        if (m_pBufferLevels[j]!=NULL) {
            for(int k=0 ; k<10; k++) {
                m_pBufferLevels[j][k].Empty();
            }
            delete [] m_pBufferLevels[j];
        }
    }
    m_pBufferLevels = NULL;

    if (LevelDiagram) {
            delete [] LevelDiagram;
            LevelDiagram = NULL;
    }

	m_pResourceManager = NULL;
	return true;
}

MOboolean
moTextureBuffer::LoadCompleted() {
	return m_bLoadCompleted;
}

MOboolean moTextureBuffer::UpdateImages( MOint maxfiles ) {

	//carga los frames desde los archivos
	moFile* pFile;

	MOint counter = 0;

	if (!m_pDirectory) return false;

	if (m_ActualImage>=(MOint)m_pDirectory->GetFiles().Count()) {
		m_bLoadCompleted = true;
		return true;
	}

	if (m_ActualImage==0)
		pFile = m_pDirectory->FindFirst();
	else
		pFile = m_pDirectory->Find(m_ActualImage);

	if (pFile)
	do {
		if ( pFile->GetType()==MO_FILETYPE_LOCAL && pFile->Exists()) {

			//LOAD AND UNCOMPRESS IMAGE
			FREE_IMAGE_FORMAT fif;
			fif = FreeImage_GetFileType( pFile->GetCompletePath(), 0);

			if( fif == FIF_UNKNOWN ) {
				// try to guess the file format from the file extension
				fif = FreeImage_GetFIFFromFilename(pFile->GetCompletePath());
			}

			if( (fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif) ) {
				//decodificamos el archivo
				FIBITMAP* pImage;
				pImage = FreeImage_Load( fif, pFile->GetCompletePath(), 0);

				//ONCE LOADED SAVE ON EVERY VIDEOBUFFER
				if (pImage) {

					//for(MOuint b = 0; b<m_VideoBuffers.Count(); b++) {

						//moVideoBuffer* pVideoBuffer = m_VideoBuffers[b];

						//if (pVideoBuffer)
                    //LoadImage(  , pImage, m_ActualImage );
                    MODebug2->Push( moText("moTextureBuffer::UpdateImages > Trying to load image:") +  (moText)pFile->GetCompletePath() );
                    if ( LoadImage( m_FolderName + moSlash + pFile->GetFileName() , pImage, m_ActualImage ) ) {
                        m_ImagesProcessed++;
                    }

					//}
					FreeImage_Unload(pImage);
					pImage = NULL;

				}
			}
		}

		m_ActualImage++;
		counter++;
		if (counter==maxfiles && maxfiles!=(-1))
			break;

	} while ( (pFile = m_pDirectory->FindNext()) );

	return true;
}


MOboolean
moTextureBuffer::LoadImage( moText p_ImageName, moBitmap* pImage, int indeximage  ) {

    MOboolean res = false;
    FIBITMAP* _pImage = (FIBITMAP*)pImage;
	FIBITMAP* pImageResult = NULL;
	FIBITMAP* pImageCropped = NULL;
	FIBITMAP* pImageScaled = NULL;
    /*
	if ( m_width!=FreeImage_GetWidth(_pImage) || m_height!=FreeImage_GetHeight(_pImage) ) {
		//CROP MODE
		pImageCropped = FreeImage_Copy( _pImage, m_XSource , m_YSource , m_XSource + m_SourceWidth , m_YSource+m_SourceHeight );
		pImageResult = pImageCropped;

	} else
	*/
	pImageResult = _pImage;
	FreeImage_AdjustContrast( pImageResult, 10 );
	FreeImage_AdjustBrightness( pImageResult, 60 );

	//RESCALE: NOTE NECESARRY HERE
	//quizas podamos definir un máximo para el tamaño tanto ancho como alto
	//o como proporción
	//forzar proporcion y esas cosas....
	/*
	if ( m_width != m_SourceWidth || m_height != m_SourceHeight ) {

		//FILTER_BOX Box, pulse, Fourier window, 1st order (constant) B-Spline
		//FILTER_BILINEAR Bilinear filter
		//FILTER_BSPLINE 4th order (cubic) B-Spline
		//FILTER_BICUBIC Mitchell and Netravali's two-param cubic filter
		//FILTER_CATMULLROM Catmull-Rom spline, Overhauser spline
		//FILTER_LANCZOS3
		pImageScaled = FreeImage_Rescale( pImageResult, m_width, m_height, FILTER_BICUBIC );
		if (pImageScaled) {
			FreeImage_Unload(pImageResult);
			pImageResult = pImageScaled;
		}
	}
	*/

    moTextureMemory* pTextureMemory = NULL;

    int idx = m_pResourceManager->GetTextureMan()->AddTexture( MO_TYPE_TEXTUREMEMORY, p_ImageName );

	if (idx>-1) {
	    pTextureMemory = (moTextureMemory*) m_pResourceManager->GetTextureMan()->GetTexture(idx);
        if (pTextureMemory) {
            if (pTextureMemory->BuildFromBitmap( pImageResult, m_BufferFormat )) {
                MODebug2->Push( moText("moTextureBuffer::LoadImage success : ") + (moText)pTextureMemory->GetName()
                                + moText(" width:") + IntToStr(pTextureMemory->GetWidth()) +
                                + moText(" height:") + IntToStr(pTextureMemory->GetHeight()) );
                m_Frames.Add(pTextureMemory);

                int contrastlevel = pTextureMemory->GetContrast() / 2000;
                int luminancelevel = (int)((float)pTextureMemory->GetLuminance() / (float)2.55);

                contrastlevel = 0;

                if (luminancelevel>=100) {
                        MODebug2->Error( moText("moTextureBuffer::LoadImage Error: luminance out of bound:")+ IntToStr(pTextureMemory->GetLuminance()) );
                } else {
                        if (max_luminance<luminancelevel) {
                            max_luminance = luminancelevel;
                        } else if (min_luminance>luminancelevel) {
                            min_luminance = luminancelevel;
                        }
                }
                if (contrastlevel>=10) {
                        MODebug2->Error( moText("moTextureBuffer::LoadImage Error: contrast out of bound:")+ IntToStr(pTextureMemory->GetContrast()) );
                }
                if (0<=luminancelevel && luminancelevel<100 && 0<=contrastlevel && contrastlevel<10) {
                    m_pBufferLevels[luminancelevel][contrastlevel].Add( pTextureMemory );
                    int icc = momin( m_pBufferLevels[luminancelevel][contrastlevel].Count(), 99 );
                    LevelDiagram[ (luminancelevel + icc*100)*3 ] = 250;
                    LevelDiagram[ (luminancelevel+1 + icc*100)*3 ] = 250;
                    LevelDiagram[ (luminancelevel+2 + icc*100)*3 ] = 250;
                }

                res = true;
            } else {
                res = false;
                MODebug2->Error( moText("moTextureBuffer::LoadImage Error loading image:")+(moText)pTextureMemory->GetName());
            }
        }
	}

	if (pImageResult!=_pImage)
		FreeImage_Unload(pImageResult);

	//m_nFrames = m_Frames.Count();
	//m_fFramesPerSecond = 25.0;

	return (res);
}
/*
MOboolean moTextureBuffer::LoadFromVideo(  moText p_moviefile ) {
	//
	return true;
}
*/

moTextureFrames&  moTextureBuffer::GetBufferLevels( int L, int C ) {

    if (0<=L && L<100 && 0<=C && C<10) {
        return m_pBufferLevels[L][C];
    } else {
        //devolvemos el oscuro
        return m_pBufferLevels[0][0];
    }

}


/*
*/
int moTextureBuffer::GetFrame( MOuint p_i ) {

	if ( p_i<m_Frames.Count()) {

		moTextureMemory* pTextureMemory = m_Frames[p_i];

		//GLId = pTextureMemory->GetReference();
		pTextureMemory->GetReference();
		return pTextureMemory->GetGLId();

    } else {
        MODebug2->Error(moText("moTextureBuffer::GetFrame Error: p_i out of range: p_i:") + IntToStr(p_i) + moText(" count:") + IntToStr(m_Frames.Count()) );
        return -1;
    }

}


void moTextureBuffer::ReleaseFrame( MOuint p_i ) {

	if ( p_i<m_Frames.Count()) {

		moTextureMemory* pTextureMemory = m_Frames[p_i];

		//GLId = pTextureMemory->GetReference();
		pTextureMemory->ReleaseReference();
    }
}

moTextureMemory* moTextureBuffer::GetTexture( MOuint p_i ) {
    if ( p_i<m_Frames.Count()) {

		moTextureMemory* pTextureMemory = m_Frames[p_i];
		return pTextureMemory;
    }
}
