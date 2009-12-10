# Microsoft Developer Studio Project File - Name="libmoldeo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libmoldeo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmoldeo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmoldeo.mak" CFG="libmoldeo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmoldeo - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libmoldeo - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libmoldeo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBAJAY_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./inc" /I "../../../inc" /I "../../../inc/3ds" /I "../../../contrib/inc" /I "../../../../externlibs/opengl/include" /I "../../../../externlibs/SDL/include" /I "../../../../externlibs/SDL_image/include" /I "../../../../externlibs/SDL_net/include" /I "../../../../externlibs/smpeg/include" /I "../../../../externlibs/dx9sdk/include" /I "../../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /I "../../inc" /I "../../inc/3ds" /I "../../contrib/inc" /I "../../../externlibs/opengl/include/GL" /I "../../../externlibs/SDL/include" /I "../../../externlibs/SDL_image/include" /I "../../../externlibs/SDL_net/include" /I "../../../externlibs/smpeg/include" /I "../../../externlibs/dx9sdk/include" /I "../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBAJAY_EXPORTS" /D _WIN32_WINNT=0x400 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sdl.lib sdlmain.lib glut32.lib opengl32.lib glu32.lib glaux.lib smpeg.lib SDL_image.lib SDL_net.lib quartz.lib strmbasd.lib /nologo /dll /machine:I386 /out:"../../bin/debug/libmoldeo.dll" /libpath:"../../contrib/lib" /libpath:"../../../externlibs/smpeg/lib" /libpath:"../../../externlibs/opengl/lib" /libpath:"../../../externlibs/SDL/lib" /libpath:"../../../externlibs/SDL_image/lib" /libpath:"../../../externlibs/SDL_net/lib" /libpath:"../../../externlibs/dx9sdk/lib"

!ELSEIF  "$(CFG)" == "libmoldeo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBAJAY_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./inc" /I "../../../inc" /I "../../../inc/3ds" /I "../../../contrib/inc" /I "../../../../externlibs/opengl/include" /I "../../../../externlibs/SDL/include" /I "../../../../externlibs/SDL_image/include" /I "../../../../externlibs/SDL_net/include" /I "../../../../externlibs/smpeg/include" /I "../../../../externlibs/dx9sdk/include" /I "../../inc" /I "../../inc/3ds" /I "../../contrib/inc" /I "../../../../externlibs/opengl/include/GL" /I "../../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /I "../../../../externlibs/glew/include" /I "../../../../externlibs/cg/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMOLDEO_EXPORTS" /D _WIN32_WINNT=0x400 /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sdl.lib sdlmain.lib glut32.lib opengl32.lib glu32.lib glaux.lib smpeg.lib SDL_image.lib SDL_net.lib quartz.lib strmbasd.lib cg.lib cgGL.lib glew32.lib /nologo /dll /debug /machine:I386 /out:"../../bin/debug/libmoldeod.dll" /pdbtype:sept /libpath:"../../contrib/lib" /libpath:"../../../../externlibs/smpeg/lib" /libpath:"../../../../externlibs/opengl/lib" /libpath:"../../../../externlibs/SDL/lib" /libpath:"../../../../externlibs/SDL_image/lib" /libpath:"../../../../externlibs/SDL_net/lib" /libpath:"../../../../externlibs/dx9sdk/lib" /libpath:"../../../../externlibs/cg/lib" /libpath:"../../../../externlibs/glew/lib"

!ENDIF 

# Begin Target

# Name "libmoldeo - Win32 Release"
# Name "libmoldeo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\libmoldeo\3ds\3ds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\mo3dModelManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moBuckets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moConsole.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moConsoleState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moCurve.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moDeviceCode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moDsGraph.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moEffect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moEffectState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moEventList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moGUIManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moImages.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moIODeviceManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moLive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moLock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moMasterEffect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moMasterPlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moMixer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moMotion.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moMouse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moNetTCPIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moNetTCPOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moNetUDPIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moNetUDPOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moParam.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moPlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moPostEffect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moPostPlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moPreConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moPreEffect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moPrePlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moPresets.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moResourceManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moSceneEffect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moSequenceEffect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moShader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moShaderCG.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moShaderGLSL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moSoundManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moText.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moTextureManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\libmoldeo\moTypes.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\inc\3ds\3ds.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\mo3dModelManager.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moAbstract.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moArray.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moBasePlugin.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moBuckets.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moConsole.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moConsoleState.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moCurve.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moDeviceCode.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moDsGraph.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moEffect.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moEffectState.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moEffectTemplate.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moEventList.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moFileRead.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moFileWrite.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moFontManager.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moGUIManager.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moImages.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moIODeviceManager.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moKeyboard.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moLive.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moLock.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moMasterEffect.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moMasterPlugin.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moMixer.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moMotion.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moMouse.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moNetTCPIn.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moNetTCPOut.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moNetUDPIn.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moNetUDPOut.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moParam.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moPlugin.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moPostEffect.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moPostPlugin.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moPreConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moPreEffect.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moPrePlugin.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moPresets.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moResourceManager.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moSceneEffect.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moSequenceEffect.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moShader.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moShaderCG.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moShaderGLSL.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moSoundManager.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moText.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moTextureManager.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moThread.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moTracker.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\moTypes.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
