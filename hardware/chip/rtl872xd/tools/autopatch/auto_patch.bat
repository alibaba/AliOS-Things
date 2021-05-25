@echo off
setlocal enableDelayedExpansion 

REM patch files to sdk (for customer)
REM Owen Chiu

echo ... patch files to sdk ...

set FILE_LIST=patch_list
set SDK_PATH=..\..
set UNZIP_PATH=unzip_tmp

echo Please drag in the patch file you want to use:
set /p PATCH_PATH=

if NOT "%PATCH_PATH:~-4%" == ".zip" (
	echo Not zip file
	pause
	exit /b
)

if exist %UNZIP_PATH% rmdir %UNZIP_PATH% /s/q
mkdir %UNZIP_PATH%

call :getFileName %PATCH_PATH:~0,-4% PATCH_NAME

REM unzip patch file
echo Unzip %PATCH_NAME%
call :genUnzipScript
CScript _unzip.vbs %PATCH_PATH% %UNZIP_PATH%

if NOT exist %UNZIP_PATH%\%FILE_LIST% (
	echo patch_list not included in the patch file, cannot use auto patch
	if exist %UNZIP_PATH% rmdir %UNZIP_PATH% /s/q
	del _unzip.vbs /q
	pause
	exit /b
)

for /f "skip=1 delims=" %%i in (%UNZIP_PATH%\%FILE_LIST%) do (
	set line=%%i
	if "!line:~-1!" == "\" (
		REM is directory
		call :getFileName !line:~0,-1! DIR_NAME
		xcopy "%UNZIP_PATH%\!DIR_NAME!\*.*" "%SDK_PATH%\%%i" /e/y
	) else (
		REM is file
		call :getFileName !line! FILE_NAME
		call :getPrefix %%i PREFIX_PATH
		xcopy "%UNZIP_PATH%\!FILE_NAME!" "%SDK_PATH%\!PREFIX_PATH!\" /y
	)
)

if exist %UNZIP_PATH% rmdir %UNZIP_PATH% /s/q
del _unzip.vbs /q
echo.
echo Patch %PATCH_NAME% done
pause
exit /b

:genUnzipScript
echo Set objArgs = WScript.Arguments > _unzip.vbs
echo ZipFile=objArgs(0) >> _unzip.vbs
echo ExtractTo=objArgs(1)>> _unzip.vbs
echo Set fso = CreateObject("Scripting.FileSystemObject") >> _unzip.vbs
echo If NOT fso.FolderExists(ExtractTo) Then >> _unzip.vbs
echo    fso.CreateFolder(ExtractTo) >> _unzip.vbs
echo End If >> _unzip.vbs
echo set objShell = CreateObject("Shell.Application") >> _unzip.vbs
echo set FilesInZip=objShell.NameSpace(ZipFile).items >> _unzip.vbs
echo objShell.NameSpace(fso.GetAbsolutePathName(ExtractTo)).CopyHere(FilesInZip) >> _unzip.vbs
echo Set fso = Nothing >> _unzip.vbs
echo Set objShell = Nothing >> _unzip.vbs
goto :eof

:getPrefix
set str=%1
call :lastindexof "%str%" "\"
set /a lastindex=!errorlevel!
set %2=!str:~0,%lastindex%!
goto :eof

:getFileName
set str=%1
call :lastindexof "%str%" "\"
set /a lastindex=!errorlevel!+1
set %2=!str:~%lastindex%!
goto :eof

:lastindexof [%1 - string ; %2 - find last index of ; %3 - if defined will store the result in variable with same name]
setlocal enableDelayedExpansion 


set "str=%~1"
set "splitter=%~2"

set LF=^


REM ** Two empty lines are required
echo off
for %%L in ("!LF!") DO (
	for /f "delims=" %%R in ("!splitter!") do ( 
		set "var=!str:%%R=%%L!"
	)
)

for /f  delims^=^" %%P in ("!var!") DO ( 
	set "last_part=%%~P"  
)

if "!last_part!" equ ""  if "%~3" NEQ "" (
 echo "not contained" >2 
 endlocal
 set %~3=-1 
 exit
) else (
 echo "not contained" >2 
 endlocal
 echo -1 
)
setlocal DisableDelayedExpansion

set ^"\n=^^^%LF%%LF%^%LF%%LF%^^"
set $strLen=for /L %%n in (1 1 2) do if %%n==2 (%\n%
      for /F "tokens=1,2 delims=, " %%1 in ("!argv!") do (%\n%
         set "str=A!%%~2!"%\n%
           set "len=0"%\n%
           for /l %%A in (12,-1,0) do (%\n%
             set /a "len|=1<<%%A"%\n%
             for %%B in (!len!) do if "!str:~%%B,1!"=="" set /a "len&=~1<<%%A"%\n%
           )%\n%
           for %%v in (!len!) do endlocal^&if "%%~b" neq "" (set "%%~1=%%v") else echo %%v%\n%
      ) %\n%
) ELSE setlocal enableDelayedExpansion ^& set argv=,


%$strlen% strlen,str
%$strlen% plen,last_part
%$strlen% slen,splitter

set /a lio=strlen-plen-slen
REM endlocal & if "%~3" NEQ "" (set %~3=%lio%) else echo %lio%
exit /b %lio%
