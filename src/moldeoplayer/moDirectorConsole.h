/*******************************************************************************

                                moDirectorConsole.h

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
  Andr�s Colubri

  Description:
	based on moConsole.h, but using wxGLCanvas context, not SDL context

*******************************************************************************/

#ifndef _MO_DIRECTORCONSOLE_
#define _MO_DIRECTORCONSOLE_

#include "moDirectorTypes.h"
#include "moGLCanvas.h"
#include "moIDirectorActions.h"
#include "moDirectorIODeviceManager.h"
#include "moDirectorCore.h"

#define TICKS_ID 333

class moDirectorConsole : public wxEvtHandler, public moConsole, public moIDirectorActions {

public:

	moDirectorConsole();
	virtual ~moDirectorConsole();
	MOboolean Finish();

	void Draw();

	void OnTimer(wxTimerEvent &event);

	void FullReset() {}
	void ResetEffect( MOint p_n ) {}
	void ResetPreEffect( MOint p_n ) {}
	void ResetPostEffect( MOint p_n ) {}
	void ResetMasterEffect( MOint p_n ) {}
	void ResetIODevice( MOint p_n ) {}


	void SetDirectorCore( moDirectorCore* p_pDirectorCore );

	moMoldeoObject* GetObject( moMobDescriptor p_MobDesc );

//================================================================
//Project
//================================================================

    moDirectorStatus ConsoleLoop();
    moDirectorStatus SetView( int x, int y, int w, int h );


	moDirectorStatus NewProject( moProjectDescriptor p_projectdescriptor );
	moDirectorStatus OpenProject( moProjectDescriptor p_projectdescriptor );
	moDirectorStatus CloseProject();
	moDirectorStatus SaveProject();
	moDirectorStatus SaveAsProject( moText p_configname, moText p_configpath );

	moDirectorStatus SetProject( moProjectDescriptor p_projectdescriptor );
	moProjectDescriptor GetProject();
	moDirectorStatus ReloadProject();
	moMobDescriptors GetMobDescriptors();

//================================================================
//Resource
//================================================================

    moResourceDescriptor GetResourceDescriptor( moResourceDescriptor p_ResourceDescriptor );
    moResourceDescriptors GetResourceDescriptors(  moResourceType p_ResourceType  );

//================================================================
//MOB Moldeo Object
//================================================================

	moDirectorStatus OpenMob( moMobDescriptor p_MobDesc );
	moDirectorStatus CloseMob( moMobDescriptor p_MobDesc );
	moDirectorStatus ReloadMob( moMobDescriptor p_MobDesc );
	moDirectorStatus NewMob( moMobDescriptor p_MobDesc );
	moDirectorStatus EditMob( moMobDescriptor p_MobDesc );
	moDirectorStatus SaveMob( moMobDescriptor p_MobDesc );
	moDirectorStatus DeleteMob( moMobDescriptor p_MobDesc );
	moMobDescriptor GetMob( moMobDescriptor p_MobDesc );
	moDirectorStatus SetMob( moMobDescriptor p_MobDesc );

	moDirectorStatus AddMobToProject( moMobDescriptor p_MobDesc );
	moDirectorStatus RemoveMobToProject( moMobDescriptor p_MobDesc );
	moDirectorStatus MoveMobInProject( moMobDescriptor p_MobDesc );

    moParameterDescriptors  GetParameterDescriptors( moMobDescriptor p_MobDesc);

//================================================================
//Parameter
//================================================================
    moDirectorStatus    SetParameter( moParameterDescriptor  p_ParameterDesc );
    moValueDescriptors  GetValueDescriptors( moParameterDescriptor p_ParamDesc );

//================================================================
//Values
//================================================================
    moDirectorStatus InsertValue( moValueDescriptor p_ValueDesc );
    moDirectorStatus DeleteValue( moValueDescriptor p_ValueDesc );
    moDirectorStatus SaveValue( moValueDescriptor p_ValueDesc );
    moDirectorStatus SetValue( moValueDescriptor p_ValueDesc );
    moValueDescriptor GetValue( moValueDescriptor p_ValueDesc );

	MOulong m_timerticks;
protected:
	MOulong GetTicks();
	void GLSwapBuffers();


protected:

	moDirectorCore*	m_pDirectorCore;
	moProjectDescriptor     m_ProjectDescriptor;

	wxTimer		m_timer;

	DECLARE_EVENT_TABLE()

};

#endif

