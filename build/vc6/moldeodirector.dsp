# Microsoft Developer Studio Project File - Name="moldeodirector" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=moldeodirector - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "moldeodirector.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "moldeodirector.mak" CFG="moldeodirector - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "moldeodirector - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "moldeodirector - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "moldeodirector - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../externlibs/wxWidgets/include" /I "../../../externlibs/wxWidgets/include/msvc" /I "./inc" /I "../../../inc" /I "../../../inc/3ds" /I "../../../contrib/inc" /I "../../../../externlibs/opengl/include" /I "../../../../externlibs/SDL/include" /I "../../../../externlibs/SDL_image/include" /I "../../../../externlibs/SDL_net/include" /I "../../../../externlibs/smpeg/include" /I "../../../../externlibs/dx9sdk/include" /I "../../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /I "../../inc" /I "../../inc/3ds" /I "../../contrib/inc" /I "../../../externlibs/opengl/include/GL" /I "../../../externlibs/SDL/include" /I "../../../externlibs/SDL_image/include" /I "../../../externlibs/SDL_net/include" /I "../../../externlibs/smpeg/include" /I "../../../externlibs/dx9sdk/include" /I "../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sdl.lib sdlmain.lib glut32.lib opengl32.lib glu32.lib glaux.lib smpeg.lib SDL_image.lib libmoldeo.lib /nologo /subsystem:windows /machine:I386 /out:"../../bin/debug/moldeo_win32.exe" /libpath:"./Release/" /libpath:"../../contrib/lib" /libpath:"../../contrib/lib/smpeg" /libpath:"../../contrib/lib/sdl" /libpath:"../../contrib/lib/sdlimg"

!ELSEIF  "$(CFG)" == "moldeodirector - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../externlibs/wxWidgets/contrib/include" /I "../../src/moldeodirector" /I "../../../../externlibs/wxWidgets/include" /I "../../../../externlibs/wxWidgets/include/msvc" /I "../../../../externlibs/wxWidgets/lib/vc_lib/mswd" /I "../../inc" /I "../../inc/3ds" /I "../../contrib/inc" /I "../../../../externlibs/SDL/include" /I "../../../../externlibs/opengl/include/GL" /I "../../../../externlibs/smpeg/include" /I "../../../../externlibs/dx9sdk/include" /I "../../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /I "../../../../externlibs/SDL_image/include/" /I "../../../../externlibs/SDL_net/include/" /I "../../../externlibs/dx9sdk/Samples/C++/DirectShow/BaseClasses" /I "../../../../externlibs/glew/include" /I "../../../../externlibs/cg/include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../../../../externlibs/wxWidgets/include" /i "./" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sdl.lib wxauid.lib wxmsw26d_gl.lib opengl32.lib glu32.lib libmoldeod.lib wxmsw26d_core.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib cg.lib cgGL.lib glew32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../bin/debug/moldeodirector_win32d.exe" /pdbtype:sept /libpath:"./Debug" /libpath:"../../contrib/lib" /libpath:"../../../../externlibs/wxWidgets/lib/vc_lib/" /libpath:"../../../../externlibs/smpeg/lib" /libpath:"../../../../externlibs/opengl/lib" /libpath:"../../../../externlibs/SDL/lib" /libpath:"../../../../externlibs/SDL_image/lib" /libpath:"../../../../externlibs/SDL_net/lib" /libpath:"../../../../externlibs/dx9sdk/lib" /libpath:"../../../../externlibs/cg/lib" /libpath:"../../../../externlibs/glew/lib"

!ENDIF 

# Begin Target

# Name "moldeodirector - Win32 Release"
# Name "moldeodirector - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorChildConsole.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorChildFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorConsole.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorCore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorIODeviceManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moFilesbook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moGLCanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moldeodirector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moMobConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moProjectTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moSessionProject.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorChildConsole.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorChildFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorConsole.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorCore.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorIODeviceManager.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moDirectorTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moFilesbook.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moGLCanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moIDirectorActions.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moMobConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moProjectTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\moldeodirector\moSessionProject.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\colours.bmp
# End Source File
# Begin Source File

SOURCE=.\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\cross.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=.\mondrian.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\rightarr.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=.\wxauitest.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\wx\msw\wx.manifest
# End Source File
# End Target
# End Project
