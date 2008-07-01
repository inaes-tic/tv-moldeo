del ..\..\plugins\effects\*.plg /S
del ..\..\plugins\mastereffects\*.plg  /S
del ..\..\plugins\posteffects\*.plg  /S
del ..\..\plugins\preeffects\*.plg  /S
for /d %%a in (..\..\plugins\effects\*.) do rd "%%a\Debug" /S /Q
for /d %%a in (..\..\plugins\effects\*.) do rd "%%a\Release" /S /Q
for /d %%a in (..\..\plugins\mastereffects\*.) do rd "%%a\Debug" /S /Q
for /d %%a in (..\..\plugins\mastereffects\*.) do rd "%%a\Release" /S /Q
for /d %%a in (..\..\plugins\posteffects\*.) do rd "%%a\Debug" /S /Q
for /d %%a in (..\..\plugins\posteffects\*.) do rd "%%a\Release" /S /Q
for /d %%a in (..\..\plugins\preeffects\*.) do rd "%%a\Debug" /S /Q
for /d %%a in (..\..\plugins\preeffects\*.) do rd "%%a\Release" /S /Q
pause