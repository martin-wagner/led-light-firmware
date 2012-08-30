@echo off
REM Remove files generated by compiler in this directory
REM and all subdirectories.

echo Removing *.$$$ files...
del *.$$$ /f /q /s
echo.

echo Removing *.bkx files...
del *.bkx /f /q /s
echo.

echo Removing *.cce files...
del *.cce /f /q /s
echo.

echo Removing *.cod files...
del *.cod /f /q /s
echo.

echo Removing *.cof files...
del *.cof /f /q /s
echo.

echo Removing *.err files...
del *.err /f /q /s
echo.

echo Removing *.hex files...
del *.hex /f /q /s
echo.

echo Removing *.i files...
del *.i /f /q /s
echo.

echo Removing *.lde files...
del *.lde /f /q /s
echo.

echo Removing *.lst files...
del *.lst /f /q /s
echo.

echo Removing *.obj files...
del *.obj /f /q /s
echo.

echo Removing *.o files...
del *.o /f /q /s
echo.

echo Removing *.rlf files...
del *.rlf /f /q /s
echo.

echo Removing *.sym files...
del *.sym /f /q /s
echo.

echo Removing *.sdb files...
del *.sdb /f /q /s
echo.

echo Removing *.wat files...
del *.wat /f /q /s
echo.

echo Removing *.mcs files...
del *.mcs /f /q /s
echo.

echo Removing *.mptags files...
del *.mptags /f /q /s
echo.

echo Removing *.tagsrc files...
del *.tagsrc /f /q /s
echo.

echo Removing *.map files...
del *.map /f /q /s
echo.

echo Removing *.elf files...
del *.elf /f /q /s
echo.

echo Removing *.dep files...
del *.dep /f /q /s
echo.

echo Removing *.p1 files...
del *.p1 /f /q /s
echo.

echo Removing *.pre files...
del *.pre /f /q /s
echo.

echo Removing *.as files...
del *.as /f /q /s
echo.

echo Removing *.hxl files...
del *.hxl /f /q /s
echo.

rd Objects /S /Q

echo Done.