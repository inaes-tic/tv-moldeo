# Microsoft Developer Studio Project File - Name="moldeo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=moldeo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "moldeo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "moldeo.mak" CFG="moldeo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "moldeo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "moldeo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "moldeo - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./inc" /I "../../../inc" /I "../../../inc/3ds" /I "../../../contrib/inc" /I "../../../../externlibs/opengl/include" /I "../../../../externlibs/SDL/include" /I "../../../../externlibs/SDL_image/include" /I "../../../../externlibs/SDL_net/include" /I "../../../../externlibs/smpeg/include" /I "../../../../externlibs/dx9sdk/include" /I "../../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /I "../../inc" /I "../../inc/3ds" /I "../../contrib/inc" /I "../../../externlibs/opengl/include/GL" /I "../../../externlibs/SDL/include" /I "../../../externlibs/SDL_image/include" /I "../../../externlibs/SDL_net/include" /I "../../../externlibs/smpeg/include" /I "../../../externlibs/dx9sdk/include" /I "../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sdl.lib sdlmain.lib glut32.lib opengl32.lib glu32.lib glaux.lib smpeg.lib SDL_image.lib libmoldeo.lib /nologo /subsystem:windows /machine:I386 /out:"../../bin/debug/moldeo_win32.exe" /libpath:"../../build/vc/Release" /libpath:"../../contrib/lib" /libpath:"../../../externlibs/smpeg/lib" /libpath:"../../../externlibs/opengl/lib" /libpath:"../../../externlibs/SDL/lib" /libpath:"../../../externlibs/SDL_image/lib" /libpath:"../../../externlibs/SDL_net/lib" /libpath:"../../../externlibs/dx9sdk/lib"

!ELSEIF  "$(CFG)" == "moldeo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugExe"
# PROP Intermediate_Dir "DebugExe"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./inc" /I "../../../inc" /I "../../../inc/3ds" /I "../../../contrib/inc" /I "../../../../externlibs/opengl/include" /I "../../../../externlibs/SDL/include" /I "../../../../externlibs/SDL_image/include" /I "../../../../externlibs/SDL_net/include" /I "../../../../externlibs/smpeg/include" /I "../../../../externlibs/dx9sdk/include" /I "../../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /I "../../inc" /I "../../inc/3ds" /I "../../contrib/inc" /I "../../../../externlibs/opengl/include/GL" /I "../../../externlibs/SDL/include" /I "../../../externlibs/SDL_image/include" /I "../../../externlibs/SDL_net/include" /I "../../../externlibs/smpeg/include" /I "../../../externlibs/dx9sdk/include" /I "../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /I "../../../../externlibs/glew/include" /I "../../../../externlibs/cg/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sdl.lib sdlmain.lib glut32.lib opengl32.lib glu32.lib glaux.lib smpeg.lib SDL_image.lib libmoldeod.lib quartz.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../bin/debug/moldeo_win32d.exe" /pdbtype:sept /libpath:"./Debug" /libpath:"../../contrib/lib" /libpath:"../../../../externlibs/smpeg/lib" /libpath:"../../../../externlibs/opengl/lib" /libpath:"../../../../externlibs/SDL/lib" /libpath:"../../../../externlibs/SDL_image/lib" /libpath:"../../../../externlibs/SDL_net/lib" /libpath:"../../../../externlibs/dx9sdk/lib" /libpath:"../../../../externlibs/cg/lib" /libpath:"../../../../externlibs/glew/lib"

!ENDIF 

# Begin Target

# Name "moldeo - Win32 Release"
# Name "moldeo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\main\moldeomain.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
