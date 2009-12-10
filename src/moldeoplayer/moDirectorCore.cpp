#include "moDirectorCore.h"


moDirectorCore::moDirectorCore() {
	m_pDirectorConsole = NULL;
	m_pDirectorIODeviceManager = NULL;
	m_pResourceManager = NULL;
}

moDirectorCore::~moDirectorCore() {
	if(m_pDirectorConsole!=NULL) {
		delete m_pDirectorConsole;
		m_pDirectorConsole = NULL;
	}
	if(m_pDirectorIODeviceManager!=NULL) {
		delete m_pDirectorIODeviceManager;
		m_pDirectorIODeviceManager = NULL;
	}
	if(m_pResourceManager!=NULL) {
		delete m_pResourceManager;
		m_pResourceManager = NULL;
	}
	m_pUserInterface = NULL;
}

bool
moDirectorCore::Init() {

    moText workpath = moFileManager::GetWorkPath();

	//conffilename.
	if ( m_Config.LoadConfig( "director.gmo" ) != MO_CONFIG_OK ) {
	    if ( !moFileManager::FileExists( "director.gmo" ) ) {
            GetDefinition();
            if (!m_Config.CreateDefault( "director.gmo" ))
                return false;
           	m_Config.GetParam(moText("installationpath")).GetValue(0).GetSubValue(0).SetText( m_ApplicationDescriptor.GetInstallationFullPath() );
           	if (m_Config.SaveConfig()!=MO_CONFIG_OK)
                return false;
	    } else return false;
	}

    moDefineParamIndex( CORE_INSTALLATIONPATH, moText("installationpath") );
    moDefineParamIndex( CORE_CONFIGURATIONPATH, moText("configurationpath") );
    moDefineParamIndex( CORE_PLUGINSPATH, moText("pluginspath") );
    moDefineParamIndex( CORE_LASTPROJECTS, moText("lastprojects") );
    moDefineParamIndex( CORE_OPENFILES, moText("openfiles") );
    moDefineParamIndex( CORE_AUTOSAVE, moText("autosave") );
    moDefineParamIndex( CORE_EXECUTIONS, moText("executions") );
    moDefineParamIndex( CORE_SPLASHSCREEN, moText("splashscreen") );
    moDefineParamIndex( CORE_3DLOGO, moText("3dlogo") );

	return true;
}

void
moDirectorCore::SetUserInterface( moDirectorFrame* p_pUserInterface ) {

	m_pUserInterface = p_pUserInterface;

	if ( m_pDirectorConsole == NULL ) {
		m_pDirectorConsole = new moDirectorConsole();
		m_pDirectorConsole!=NULL ? Log(moText("Director console created.")) : LogError(moText("Director console couldn't be created.")) ;
	}

	if ( m_pDirectorIODeviceManager == NULL ) {
		m_pDirectorIODeviceManager = new moDirectorIODeviceManager();
		m_pDirectorIODeviceManager!=NULL ? Log(moText("Director IODeviceManager created.")) : LogError(moText("Director IODeviceManager couldn't be created.")) ;
	}

	if ( m_pResourceManager == NULL ) {
		m_pResourceManager = new moResourceManager();
		m_pResourceManager!=NULL ? Log(moText("Director Resource Manager created.")) : LogError(moText("Director Resource Manager couldn't be created.")) ;
	}

	if ( m_pUserInterface && m_pResourceManager && m_pDirectorConsole) {

		m_pUserInterface->SetDirectorCore( this );
		m_pUserInterface->SetIODeviceManager(m_pDirectorIODeviceManager);

		m_pDirectorConsole->SetResourceManager( m_pResourceManager );
		m_pDirectorConsole->SetIODeviceManager( m_pDirectorIODeviceManager );
		m_pDirectorConsole->SetDirectorCore( this );
		Log("Resources initialized.");
	} else {
		LogError("Couldn't initialize resources modules correctly, maybe more memory is needed.");
		ErrorMessage("The program will close");
		exit(1);
	}
}

void
moDirectorCore::SetPaths( moText p_installationpath ) {

    m_ApplicationDescriptor = moApplicationDescriptor( p_installationpath, p_installationpath + moText("\\plugins") );

}

//=============================================================================================================================



//=========================================================================================================================
//NEW IDIRECTORACTIONS
//=========================================================================================================================

	moResourceManager*	moDirectorCore::GetResourceManager() {

		return m_pResourceManager;

	}

	moDirectorStatus moDirectorCore::Preview() { return MO_DIRECTOR_STATUS_OK; }

	moDirectorStatus moDirectorCore::ConsoleLoop() {
	    return m_pDirectorConsole->ConsoleLoop();
    }

	void moDirectorCore::ViewSwapBuffers() {
	    if (m_pUserInterface)
            m_pUserInterface->ViewSwapBuffers();
    }

    moDirectorStatus
    moDirectorCore::SetView( int x, int y, int w, int h ) {
        return m_pDirectorConsole->SetView( x, y, w, h );
    }

    MO_HANDLE moDirectorCore::GetHandle() {
        return m_pUserInterface->GetHandle();
    }

    MO_DISPLAY moDirectorCore::GetDisplay() {
        return m_pUserInterface->GetDisplay();
    }

	moDirectorStatus moDirectorCore::FullScreen( bool force) {
        return m_pUserInterface->FullScreen(force);
	}

	moDirectorStatus moDirectorCore::Play() {
        return m_pDirectorConsole->Play();
	}

	moDirectorStatus moDirectorCore::Pause() {
        return m_pDirectorConsole->Pause();
	}

	moDirectorStatus moDirectorCore::FocusOutput() {
        return m_pUserInterface->FocusOutput();
	}

	moDirectorStatus moDirectorCore::SaveSession() {
        return m_pDirectorConsole->SaveSession();
	}
	moDirectorStatus moDirectorCore::SaveAll() {
	    return m_pDirectorConsole->SaveAll();
    }
	moDirectorStatus moDirectorCore::CloseAll() {
        return m_pUserInterface->CloseAll();
    }

	moDirectorStatus moDirectorCore::CloseApp() {
        m_pUserInterface = NULL;
        return MO_DIRECTOR_STATUS_OK;
    }


	moApplicationDescriptor moDirectorCore::GetApplicationDescriptor() {

        //actualizar lista de plugins:
        moText pluginsfullpath = m_ApplicationDescriptor.GetPluginsFullPath();

        moPluginDefinitions& rPluginDefs( m_ApplicationDescriptor.GetPluginDefinitions());

        Log( moText("Checking plugins") );
        //recorrer los plugins en busca de: dll o so
        //lo interesante sería que la definición de los plugins se complete a su vez
        //...

        //pluginsfullpath + moText("\\preeffects");
        moDirectory DirEffects;

        //PREEFFECTS
        DirEffects.Open( pluginsfullpath + moText("/preeffects"), moText("/*.dll") );
        if (DirEffects.Exists()) {

            moFile* pFile = NULL;

            pFile = DirEffects.FindFirst();
            moText FileNameEnd;

            if (pFile!=NULL)
                Log( moText("File founded") );

            while(pFile!=NULL) {
                FileNameEnd = pFile->GetFileName();
                FileNameEnd.Right(1);
                if ( pFile->GetExtension()==moText("dll") && FileNameEnd!=moText("d") ) {
                    rPluginDefs.Add( moPluginDefinition( pFile->GetFileName(), pFile->GetCompletePath(), MO_OBJECT_PREEFFECT ) );
                }
                Log( pFile->GetFileName() );
                pFile = DirEffects.FindNext();
            }
        } else LogError( moText("PreEffects directory doesn´t exists") );


        //EFFECTS
        DirEffects.Open( pluginsfullpath + moText("/effects"), moText("/*.dll") );
        if (DirEffects.Exists()) {

            moFile* pFile = NULL;

            pFile = DirEffects.FindFirst();
            moText FileNameEnd;

            if (pFile!=NULL)
                Log( moText("File founded") );

            while(pFile!=NULL) {
                FileNameEnd = pFile->GetFileName();
                FileNameEnd.Right(1);
                if ( pFile->GetExtension()==moText("dll") && FileNameEnd!=moText("d") ) {
                    rPluginDefs.Add( moPluginDefinition( pFile->GetFileName(), pFile->GetCompletePath(), MO_OBJECT_EFFECT ) );
                }
                Log( pFile->GetFileName() );
                pFile = DirEffects.FindNext();
            }
        } else LogError( moText("Effects directory doesn´t exists") );


        //POSTEFFECTS
        DirEffects.Open( pluginsfullpath + moText("/posteffects"), moText("/*.dll") );
        if (DirEffects.Exists()) {

            moFile* pFile = NULL;

            pFile = DirEffects.FindFirst();
            moText FileNameEnd;

            if (pFile!=NULL)
                Log( moText("File founded") );

            while(pFile!=NULL) {
                FileNameEnd = pFile->GetFileName();
                FileNameEnd.Right(1);
                if ( pFile->GetExtension()==moText("dll") && FileNameEnd!=moText("d") ) {
                    rPluginDefs.Add( moPluginDefinition( pFile->GetFileName(), pFile->GetCompletePath(), MO_OBJECT_POSTEFFECT ) );
                }
                Log( pFile->GetFileName() );
                pFile = DirEffects.FindNext();
            }
        } else LogError( moText("Effects directory doesn´t exists") );

        //IODEVICES
        DirEffects.Open( pluginsfullpath + moText("/iodevices"), moText("/*.dll") );
        if (DirEffects.Exists()) {

            moFile* pFile = NULL;

            pFile = DirEffects.FindFirst();
            moText FileNameEnd;

            if (pFile!=NULL)
                Log( moText("File founded") );

            while(pFile!=NULL) {
                FileNameEnd = pFile->GetFileName();
                FileNameEnd.Right(1);
                if ( pFile->GetExtension()==moText("dll") && FileNameEnd!=moText("d") ) {
                    rPluginDefs.Add( moPluginDefinition( pFile->GetFileName(), pFile->GetCompletePath(), MO_OBJECT_IODEVICE ) );
                }
                Log( pFile->GetFileName() );
                pFile = DirEffects.FindNext();
            }
        } else LogError( moText("IODevices directory doesn´t exists") );

        //RESOURCES
        DirEffects.Open( pluginsfullpath + moText("/resources"), moText("/*.dll") );
        if (DirEffects.Exists()) {

            moFile* pFile = NULL;

            pFile = DirEffects.FindFirst();
            moText FileNameEnd;

            if (pFile!=NULL)
                Log( moText("File founded") );

            while(pFile!=NULL) {
                FileNameEnd = pFile->GetFileName();
                FileNameEnd.Right(1);
                if ( pFile->GetExtension()==moText("dll") && FileNameEnd!=moText("d") ) {
                    rPluginDefs.Add( moPluginDefinition( pFile->GetFileName(), pFile->GetCompletePath(), MO_OBJECT_RESOURCE ) );
                }
                Log( pFile->GetFileName() );
                pFile = DirEffects.FindNext();
            }
        } else LogError( moText("Resources directory doesn´t exists") );


        moApplicationDescriptor pAppDes;
        pAppDes = m_ApplicationDescriptor;
        return pAppDes;

    }

//================================================================
//Project
//================================================================

	moDirectorStatus moDirectorCore::OpenProject( moProjectDescriptor p_projectdescriptor ) {
		return m_pDirectorConsole->OpenProject( p_projectdescriptor );
	}

	moDirectorStatus moDirectorCore::CloseProject() {
		if(m_pDirectorConsole->CloseProject() == MO_DIRECTOR_STATUS_OK) {
			return MO_DIRECTOR_STATUS_OK;
		}

		return MO_DIRECTOR_STATUS_ERROR;
	}

	moDirectorStatus moDirectorCore::SaveProject() {
        return m_pDirectorConsole->SaveProject();
    }

	moDirectorStatus moDirectorCore::SaveAsProject( moText p_configname, moText p_configpath ) {
	    return MO_DIRECTOR_STATUS_OK;
    }

	moDirectorStatus moDirectorCore::ProjectUpdated( moProjectDescriptor p_projectdescriptor ) {

		return m_pUserInterface->ProjectUpdated( p_projectdescriptor );

	}

	moDirectorStatus moDirectorCore::SetProject( moProjectDescriptor p_projectdescriptor ) { return MO_DIRECTOR_STATUS_OK; }

	moProjectDescriptor moDirectorCore::GetProject() {
		return m_pDirectorConsole->GetProject();
	}

	moDirectorStatus moDirectorCore::ReloadProject() { return MO_DIRECTOR_STATUS_OK; }

	moDirectorStatus moDirectorCore::ProjectPreview( ) {
		return m_pUserInterface->ProjectPreview();
	}
	moDirectorStatus moDirectorCore::EffectPreview( MOint p_n ) { return MO_DIRECTOR_STATUS_OK; }

    moMobDescriptors moDirectorCore::GetMobDescriptors() {
            return m_pDirectorConsole->GetMobDescriptors();
    }


//================================================================
//Resources
//================================================================


    moResourceDescriptor moDirectorCore::GetResourceDescriptor( moResourceDescriptor p_ResourceDescriptor ) {
        return m_pDirectorConsole->GetResourceDescriptor( p_ResourceDescriptor );
    }

    moResourceDescriptors moDirectorCore::GetResourceDescriptors(  moResourceType p_ResourceType  ) {
        return m_pDirectorConsole->GetResourceDescriptors( p_ResourceType );
    }


//================================================================
//MOB Moldeo Object
//================================================================


//================================================================
//PARAMETERS
//================================================================


	moParameterDescriptors  moDirectorCore::GetParameterDescriptors( moMobDescriptor p_MobDesc ) {
	    return m_pDirectorConsole->GetParameterDescriptors(p_MobDesc);
    }


//================================================================
// VALUES
//================================================================

	moValueDescriptors  moDirectorCore::GetValueDescriptors( moParameterDescriptor p_ParamDesc ) {
	    return m_pDirectorConsole->GetValueDescriptors( p_ParamDesc );
    }

	moValueDescriptor moDirectorCore::GetValue( moValueDescriptor p_ValueDesc ) {
	    return m_pDirectorConsole->GetValue( p_ValueDesc );
    }

//===============================================================
// LOGS
//===============================================================

	void moDirectorCore::Log( moText p_message ) {

		m_pUserInterface->Log( p_message );
		//m_pUserInterface->SetStatusText( w );
	}
	void moDirectorCore::LogError( moText p_message ) {

		m_pUserInterface->LogError( p_message );

	}
	void moDirectorCore::ShowMessage( moText p_message ) {

		wxString msg =(wxChar*)(char*)p_message;

		wxMessageBox( msg );

	}

	bool moDirectorCore::ConfirmMessage( moText p_message ) {

		wxString msg =(wxChar*)(char*)p_message;

		int answer = wxMessageBox( msg, wxT("Confirm"),
                            wxYES_NO | wxCANCEL | wxICON_QUESTION );

		if (answer == wxYES) {
			return(true);
		} else {
			return(false);
		}

	}

	void moDirectorCore::AlertMessage( moText p_message ) {

		wxString msg =(wxChar*)(char*)p_message;

		int answer = wxMessageBox( msg, wxT("Alert!"),
                            wxOK | wxICON_EXCLAMATION );

	}

	void moDirectorCore::ErrorMessage( moText p_message ) {

		wxString msg =(wxChar*)(char*)p_message;

		int answer = wxMessageBox( msg, wxT("Error!"),
                            wxOK | wxICON_ERROR );

	}


//=============================================================================================================================

moConfigDefinition *
moDirectorCore::GetDefinition( moConfigDefinition *p_configdefinition ) {

	if ( p_configdefinition==NULL ) {
		p_configdefinition = m_Config.GetConfigDefinition();
	}

    p_configdefinition->Set( "moldeoplayer", "moldeoguicore" );
    p_configdefinition->Add( moText("installationpath"), MO_PARAM_TEXT );
    p_configdefinition->Add( moText("configurationpath"), MO_PARAM_TEXT );
    p_configdefinition->Add( moText("pluginspath"), MO_PARAM_TEXT );
    p_configdefinition->Add( moText("lastprojects"), MO_PARAM_TEXT );
    p_configdefinition->Add( moText("openfiles"), MO_PARAM_TEXT );
    p_configdefinition->Add( moText("autosave"), MO_PARAM_TEXT );
    p_configdefinition->Add( moText("executions"), MO_PARAM_NUMERIC );
    p_configdefinition->Add( moText("splashscreen"), MO_PARAM_TEXTURE );
    p_configdefinition->Add( moText("3dlogo"), MO_PARAM_OBJECT );

	//p_configdefinition->Add( moText("inlet"), MO_PARAM_INLET );
	//p_configdefinition->Add( moText("outlet"), MO_PARAM_OUTLET );
	return p_configdefinition;
}

void
moDirectorCore::LoadDefinition() {

	GetDefinition();

	moParamDefinitions *pdefinitions = m_Config.GetConfigDefinition()->GetParamDefinitions();

	for( MOuint i=0; i < pdefinitions->Count(); i++ ) {
		pdefinitions->Get(i).SetIndex( m_Config.GetParamIndex( pdefinitions->Get(i).GetName() ));
	}

}


