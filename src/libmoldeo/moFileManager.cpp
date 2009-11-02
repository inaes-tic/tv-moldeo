/*******************************************************************************

                              moFileManager.cpp

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

#include "moFileManager.h"

#include <boost/filesystem.hpp>
namespace bfs=boost::filesystem;

//===========================================
//
//				moFileManager
//
//===========================================

moFileManager::moFileManager() {

	SetType( MO_OBJECT_RESOURCE );
	SetResourceType( MO_RESOURCETYPE_FILE );

	SetName("File Manager");

}

moFileManager::~moFileManager() {

}

MOboolean moFileManager::Init() {
	return true;
}


MOboolean moFileManager::Finish() {
	return true;
}


MOboolean
moFileManager::Load( moText p_FileName, MOboolean bWaitForDownload ) {

	for(MOuint i = 0; i< m_Files.Count(); i++ ) {
		if ( m_Files[i]->GetCompletePath() == p_FileName ) {
			return true;
		}
	}

	moFile* pFile =  new moFile( p_FileName );

	if ( pFile!=NULL ) {
		if (pFile->GetType()==MO_FILETYPE_LOCAL && !pFile->Exists())
			return false;
		if (bWaitForDownload && pFile->IsRemote()) {
			while(pFile->GetStatus()!=MO_FILESTATUS_READY) {
				pFile->Update();
			}
		}
		m_Files.Add(pFile);
		return true;
	}
	return false;
}

moFile*
moFileManager::GetFile( moText p_FileName ) {

	if ( Load(p_FileName) ) {
		for(MOuint i = 0; i< m_Files.Count(); i++ ) {
			if ( m_Files[i]->GetCompletePath() == p_FileName ) {
				return m_Files[i];
			}
		}
	}
	return NULL;
}


MOboolean
moFileManager::Open( moText p_Path, MOboolean bWaitForDownload ) {

	for(MOuint i = 0; i< m_Directories.Count(); i++ ) {
		if ( m_Directories[i]->GetCompletePath() == p_Path ) {
			return true;
		}
	}

	moDirectory* pDir =  new moDirectory( p_Path, this );

	if ( pDir!=NULL ) {
		if (pDir->GetType()==MO_FILETYPE_LOCAL && !pDir->Exists())
			return false;
		if (bWaitForDownload && pDir->IsRemote()) {
			while(pDir->GetStatus()!=MO_FILESTATUS_READY) {
				pDir->Update();
			}
		}
		m_Directories.Add(pDir);
		return true;
	}
	return false;
}

moDirectory*
moFileManager::GetDirectory( moText p_Path ) {
	if ( Open(p_Path) ) {
		for(MOuint i = 0; i< m_Directories.Count(); i++ ) {
			if ( m_Directories[i]->GetCompletePath() == p_Path ) {
				return m_Directories[i];
			}
		}
	}
	return NULL;
}

moText
moFileManager::GetExePath() {

    return moText("");

}

moText
moFileManager::GetWorkPath() {
    char lbuf[1024];

    getcwd( lbuf , sizeof( lbuf ) );

    moText workpath = lbuf;

    return workpath;

}

bool
moFileManager::CreateDirectory( moDirectory Dir ) {

    std::string dirname;

    dirname = Dir.GetCompletePath();

    bfs::create_directory( dirname );

    return Dir.Exists();
}


bool
moFileManager::FileExists( moText filename ) {
        moFile file( filename );
        return file.Exists();
}

bool
moFileManager::DirectoryExists( moText dirname ) {
        moDirectory   directory( dirname );
        return directory.Exists();
}



bool
moFileManager::CopyFile( moText FileSrc, moText FileDst ) {

/*
	ifstream initialFile(FileSrc, ios::in|ios::binary);
	ofstream outputFile(FileDst, ios::out|ios::binary);
	//defines the size of the buffer
	initialFile.seekg(0, ios::end);
	long fileSize = initialFile.tellg();
	//Requests the buffer of the predefined size


	//As long as both the input and output files are open...
	if(initialFile.is_open() && outputFile.is_open())
	{
		short * buffer = new short[fileSize/2];
		//Determine the file's size
		//Then starts from the beginning
		initialFile.seekg(0, ios::beg);
		//Then read enough of the file to fill the buffer
		initialFile.read((char*)buffer, fileSize);
		//And then write out all that was read
		outputFile.write((char*)buffer, fileSize);
		delete[] buffer;
	}
	//If there were any problems with the copying process, let the user know
	else if(!outputFile.is_open())
	{
		MODebug2->Error( moText("I couldn't open ") + (moText)FileDst + moText(" for copying!"));
		return 0;
	}
	else if(!initialFile.is_open())
	{
		MODebug2->Error( moText("I couldn't open ") + (moText)FileSrc + moText(" for copying!"));
		return 0;
	}

	initialFile.close();
	outputFile.close();
*/
    std::string name, new_name;

    name = FileSrc;
    new_name = FileDst;

    try {
        bfs::copy_file(name, new_name);
    }
    catch( const std::exception & ex ) {
        MODebug2->Error( ex.what() );
    }


	return 1;

}
