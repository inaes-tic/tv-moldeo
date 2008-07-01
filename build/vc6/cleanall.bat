rd Debug /S /Q
rd DebugExe /S /Q
rd Release /S /Q
rd ReleaseExe /S /Q
del *.plg *.ncb
del ..\..\bin\debug\moldeo*.exe
del ..\..\bin\debug\libmoldeo*.dll
del ..\..\bin\debug\plugins\effects\*.dll
del ..\..\bin\debug\plugins\mastereffects\*.dll
del ..\..\bin\debug\plugins\posteffects\*.dll
del ..\..\bin\debug\plugins\preeffects\*.dll
CALL cleanbingarbage.bat
CALL cleanpluginsgarbage.bat
pause