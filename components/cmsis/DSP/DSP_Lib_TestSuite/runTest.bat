@echo off

set UVEXE=C:\Keil_v5\UV4\UV4.EXE

if .%1==. goto help
for %%a in (ARM GCC ARMCLANG) do if %1==%%a goto checkParam2
echo   parameter %1 not supported.
goto help

:checkParam2
if .%2==. goto help
for %%a in ( ^
 cortexM0l ^
 cortexM3l ^
 cortexM4l ^
 cortexM4lf ^
 cortexM7l ^
 cortexM7lfsp ^
 cortexM7lfdp ^
 ARMv8MBLl ^
 ARMv8MMLl ^
 ARMv8MMLlfsp ^
 ARMv8MMLlfdp ^
 ARMv8MMLld ^
 ARMv8MMLldfsp ^
 ARMv8MMLldfdp ^
           ) do if %2==%%a goto checkParam3
echo   parameter %2 not supported.
goto help

:checkParam3
if .%3==. goto help
for %%a in (MPS2 FVP Simulator) do if %3==%%a goto CheckLibraries
echo   parameter %3 not supported.
goto help

:CheckLibraries
if %1==ARM                goto CheckLibrariesARM
if %1==ARMCLANG           goto CheckLibrariesARMCLANG
if %1==GCC                goto CheckLibrariesGCC
goto end

:CheckLibrariesARM
if not exist ".\RefLibs\%1\Lib\arm_%2_ref.lib"   (echo ".\RefLibs\%1\Lib\arm_%2_ref.lib" not found.  & goto end)
if not exist "..\Lib\%1\arm_%2_math.lib"         (echo "..\Lib\%1\arm_%2_ref.lib" not found.         & goto end)
goto CopyLibrariesARM

:CheckLibrariesARMCLANG
if not exist ".\RefLibs\%1\Lib\arm_%2_ref.lib"   (echo ".\RefLibs\%1\Lib\arm_%2_ref.lib" not found.  & goto end)
if not exist "..\Lib\%1\arm_%2_math.lib"         (echo "..\Lib\%1\arm_%2_ref.lib" not found.         & goto end)
goto CopyLibrariesARMCLANG

:CheckLibrariesGCC
if not exist ".\RefLibs\%1\Lib\libarm_%2_ref.a"  (echo ".\RefLibs\%1\Lib\libarm_%2_ref.a" not found.  & goto end)
if not exist "..\Lib\%1\libarm_%2_math.a"        (echo "..\Lib\%1\libarm_%2_math.a" not found.        & goto end)
goto CopyLibrariesGCC

:CopyLibrariesARM
copy /B ".\RefLibs\%1\Lib\arm_%2_ref.lib"    .\DspLibTest_%3\%1\Lib\arm_ref.lib  /B /Y
copy /B "..\Lib\%1\arm_%2_math.lib"          .\DspLibTest_%3\%1\Lib\arm_math.lib /B /Y
goto buildProject

:CopyLibrariesARMCLANG
copy /B ".\RefLibs\%1\Lib\arm_%2_ref.lib"    .\DspLibTest_%3\%1\Lib\arm_ref.lib  /B /Y
copy /B "..\Lib\%1\arm_%2_math.lib"          .\DspLibTest_%3\%1\Lib\arm_math.lib /B /Y
goto buildProject

:CopyLibrariesGCC
copy /B ".\RefLibs\%1\Lib\libarm_%2_ref.a"   .\DspLibTest_%3\%1\Lib\libarm_ref.a  /B /Y
copy /B "..\Lib\%1\libarm_%2_math.a"         .\DspLibTest_%3\%1\Lib\libarm_math.a /B /Y
goto buildProject

:buildProject
  echo   Build Test Project ...
%UVEXE%  -r -j0 .\DspLibTest_%3\%1\DspLibTest_%3.uvprojx -t "%2" -o ".\Logs\DspLibTest_%3_%2_build.log"

  echo   Run Test ...
del /Q ".\DspLibTest_%3\%1\Logs\DspLibTest_%3.log" 2>NUL
del /Q ".\DspLibTest_%3\%1\Logs\DspLibTest_%3_%2.log" 2>NUL

rem get start time (The format of %TIME% is HH:MM:SS,CS for example 23:59:59,99)
set STARTTIME=%TIME%

rem run the test
%UVEXE% -d .\DspLibTest_%3\%1\DspLibTest_%3.uvprojx -t "%2"

rem get end time
set ENDTIME=%TIME%

rem calculate duration
rem Change formatting for the start and end times
for /F "tokens=1-4 delims=:.," %%a in ("%STARTTIME%") do (
   set /A "start=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)

for /F "tokens=1-4 delims=:.," %%a in ("%ENDTIME%") do (
   set /A "end=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)

rem Test midnight rollover. If so, add 1 day=8640000 1/100ths secs
if %end% lss %start% set /a end+=8640000

rem Calculate the elapsed time by subtracting values
set /A elapsed=end-start

rem Format the results for output
set /A hh=elapsed/(60*60*100), rest=elapsed%%(60*60*100), mm=rest/(60*100), rest%%=60*100, ss=rest/100, cc=rest%%100
if %hh% lss 10 set hh=0%hh%
if %mm% lss 10 set mm=0%mm%
if %ss% lss 10 set ss=0%ss%
if %cc% lss 10 set cc=0%cc%

set DURATION=%hh%:%mm%:%ss%,%cc%

rem write time to file
echo Test %1 %2 :             > ".\DspLibTest_%3\%1\Logs\DspLibTest_%3_%2_time.log"
echo Start time: %STARTTIME% >> ".\DspLibTest_%3\%1\Logs\DspLibTest_%3_%2_time.log"
echo End time:   %ENDTIME%   >> ".\DspLibTest_%3\%1\Logs\DspLibTest_%3_%2_time.log"
echo Duration:   %DURATION%  >> ".\DspLibTest_%3\%1\Logs\DspLibTest_%3_%2_time.log"

  echo   Copy Logfile ...
copy /B ".\DspLibTest_%3\%1\Logs\DspLibTest_%3.log" ".\DspLibTest_%3\%1\Logs\DspLibTest_%3_%2.log"

goto end


:help
echo.
echo   Syntax: runTest toolchain core test
echo.
echo     toolchain: ARM ^| GCC
echo     core:      cortexM0l ^| cortexM3l ^| cortexM4l ^| cortexM4lf ^| cortexM7l ^| cortexM7lfsp ^| cortexM7lfdp
echo                ARMv8MBLl
echo                ARMv8MMLl ^| ARMv8MMLlfsp ^| ARMv8MMLlfdp
echo                ARMv8MMLld ^| ARMv8MMLldfsp ^| ARMv8MMLldfdp
echo     test:      MPS2 ^| FVP ^| Simulator
echo.
echo   e.g.: runTest ARM cortexM3l Simulator

:end
@echo on
