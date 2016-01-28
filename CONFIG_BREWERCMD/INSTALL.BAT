@ECHO OFF
SET SCIVER=V1R3
IF "%1" == "alpha" GOTO ALPHA
IF NOT "%1" == "" GOTO DRIVE
:USAGE
ECHO To install BrewCMD source:
ECHO.
ECHO   1. Identify the subdirectory in O:\SOFTWARE\DEV to hold the source to
ECHO      be modified by the project.
ECHO.
ECHO   2. Make the drive and directory containing this batch file the current
ECHO      directory.
ECHO.
ECHO   3. Then type INSTALL {dest}, where {dest} is the subdirectory
ECHO      identified above (usually the project modifying the source). For
ECHO      example:
ECHO.
ECHO         INSTALL 7B27
ECHO.
ECHO      installs into O:\SOFTWARE\DEV\7B27\12340073.
ECHO.
ECHO.  4. When you run the Borland C++ compiler, make sure to change any
ECHO      output directories in the IDE to O:\SOFTWARE\DEV\{dest}\12340073.
GOTO DONE

REM ***
REM *** Alpha installation of BrewCMD
REM ***
:ALPHA
IF EXIST ..\12340073\BREWCMD.EXE GOTO :ALPHADIR
ECHO There is no BREWCMD.EXE file to copy.  Either you have not compiled
ECHO the executable, or you are in the wrong directory.
GOTO DONE

:ALPHADIR
IF EXIST O:\SOFTWARE\ALPHA\BREWER\12340073\NUL GOTO :SOURCEDIR
md O:\SOFTWARE\ALPHA\BREWER\12340073
:SOURCEDIR
IF EXIST O:\SOFTWARE\SOURCE\BREWER\12340073\NUL GOTO :AEXE
md O:\SOFTWARE\SOURCE\BREWER\12340073
:AEXE
IF NOT EXIST O:\SOFTWARE\SOURCE\BREWER\12340073\%SCIVER%.ZIP GOTO :BEXE
ECHO You cannot overwrite an older version of the source.  Please
ECHO update the version number in this batch file.
GOTO DONE
:BEXE
copy BREWCMD.EXE O:\SOFTWARE\ALPHA\BREWER\12340073
del O:\SOFTWARE\ALPHA\BREWER\12340073\*.VER
copy O:\SOFTWARE\ALPHA\SCITEC.GID O:\SOFTWARE\ALPHA\BREWER\12340073\%SCIVER%.VER
IF EXIST %SCIVER%.ZIP del %SCIVER%.ZIP
wzzip -rp %SCIVER%.ZIP *.*
copy %SCIVER%.ZIP O:\SOFTWARE\SOURCE\BREWER\12340073
copy README.TXT O:\SOFTWARE\SOURCE\BREWER\12340073
copy INSTALL.BAT O:\SOFTWARE\SOURCE\BREWER\12340073
cd O:\SOFTWARE\SOURCE\BREWER\12340073
ECHO An alpha copy of the BrewCMD utility is now available in
ECHO O:\SOFTWARE\ALPHA\BREWER\12340073.  You may now type
ECHO DELTREE O:\SOFTWARE\DEV\{proj}\12340073 to remove the development
ECHO code.
GOTO DONE

REM ***
REM *** Brewer firmware source code installation
REM ***
:DRIVE
IF EXIST O:\SOFTWARE\DEV\%1\NUL GOTO :PROJECT
md O:\SOFTWARE\DEV\%1
:PROJECT
IF EXIST O:\SOFTWARE\DEV\%1\12340073\NUL GOTO :SOURCE
md O:\SOFTWARE\DEV\%1\12340073
:SOURCE
wzunzip -d -o %SCIVER%.ZIP O:\SOFTWARE\DEV\%1\12340073\
:INSTALD
ECHO The BrewCMD utility source code has been installed into
ECHO O:\SOFTWARE\DEV\%1\12340073.
GOTO DONE

:DONE
SET SCIVER=
pause
