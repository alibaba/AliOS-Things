@ECHO off

REM Usage: test.bat [PATH TO UV4.exe] 

IF "%1"=="" (
  SET UV4_EXE=C:\Keil\UV4\UV4.exe
) ELSE (
  SET UV4_EXE=%1
)
ECHO Using %UV4_EXE%

ECHO.
ECHO Building application...
IF EXIST .\Objects\Validation.axf del .\Objects\Validation.axf

%UV4_EXE% -b Validation.uvprojx

IF EXIST .\Objects\Validation.axf (
  ECHO Build succeded
) ELSE (
  ECHO Build failed
  GOTO :done
)

ECHO.
ECHO Loading application to hardware target...
%UV4_EXE% -f Validation.uvprojx -t"CMSIS_DAP"

IF ERRORLEVEL 1 (
  ECHO Flash download failed
  GOTO :done
)

ECHO.
ECHO Debugging hardware target...
IF EXIST .\test_results.txt del .\test_results.txt

%UV4_EXE% -d Validation.uvprojx -t"CMSIS_DAP"

IF EXIST .\test_results.txt (
  TYPE .\test_results.txt
) ELSE (
  ECHO Test ended abnormally - file test_results.txt was not produced
  GOTO :done
)

ECHO.
ECHO All tests completed

:done
