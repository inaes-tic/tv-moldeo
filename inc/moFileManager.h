/*******************************************************************************

                                moFileManager.h

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

#ifndef __MO_FILEMANAGER_H
#define __MO_FILEMANAGER_H

#include "moFile.h"
#include "moResourceManager.h"

/**
* Recurso para el acceso a archivos
*/

class LIBMOLDEO_API moFileManager : public moResource
{
	public:

		moFileManager();
		virtual ~moFileManager();

		virtual MOboolean Init();
		virtual MOboolean Finish();


		MOboolean		Load( moText p_FileName, MOboolean bWaitForDownload = false );
		moFile*			GetFile( moText p_FileName );

		MOboolean		Open ( moText p_Path,  MOboolean bWaitForDownload = false );
		moDirectory*	GetDirectory( moText p_Path );

        static bool       CreateDirectory( moDirectory Dir );
        static bool       CopyFile( moText FileSrc, moText FileDst );

		static moText          GetExePath();
		static moText          GetWorkPath();
		static bool            FileExists( moText filename );
        static bool            DirectoryExists( moText dirname );

	protected:

		moFileArray	m_Files;
		moDirectoryArray m_Directories;

};

#endif
