@echo off

set UVEXE=C:\Keil_v5\UV4\UV4.EXE
set CURDIR=%CD%

if .%1==. goto help
for %%a in (ARM GCC) do if %1==%%a goto startBuild
goto help

:startBuild
echo.
echo Building DSP Libraries %1

if %1==ARM                goto buildARM
if %1==GCC                goto buildGCC
goto err

:buildARM
:buildGCC
cd ..\Projects\%1

echo   Building DSP Library for Cortex-M0 Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM0l"     -o "DspLib_cortexM0l_build.log"

echo   Building DSP Library for Cortex-M3 Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM3l"     -o "DspLib_cortexM3l_build.log"

echo   Building DSP Library for Cortex-M4 Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM4l"     -o "DspLib_cortexM4l_build.log"

echo   Building DSP Library for Cortex-M4 Little Endian with single precision FPU
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM4lf"    -o "DspLib_cortexM4lf_build.log"

echo   Building DSP Library for Cortex-M7 Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM7l"     -o "DspLib_cortexM7l_build.log"

echo   Building DSP Library for Cortex-M7 Little Endian with single precision FPU
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM7lfsp"  -o "DspLib_cortexM7lfsp_build.log"

echo   Building DSP Library for Cortex-M7 Little Endian with double precision FPU
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM7lfdp"  -o "DspLib_cortexM7lfdp_build.log"

echo   Building DSP Library for ARMv8-M Baseline Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "ARMv8MBLl"     -o "DspLib_ARMv8MBLl_build.log"

echo   Building DSP Library for ARMv8-M Mainline Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "ARMv8MMLl"     -o "DspLib_ARMv8MMLl_build.log"

echo   Building DSP Library for ARMv8-M Mainline Little Endian with single precision FPU
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "ARMv8MMLlfsp"  -o "DspLib_ARMv8MMLlfsp_build.log"

REM echo   Building DSP Library for ARMv8-M Mainline Little Endian with double precision FPU
REM %UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "ARMv8MMLlfdp"  -o "DspLib_ARMv8MMLlfdp_build.log"

echo   Building DSP Library for ARMv8-M Mainline Little Endian with DSP instructions
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "ARMv8MMLld"    -o "DspLib_ARMv8MMLld_build.log"

echo   Building DSP Library for ARMv8-M Mainline Little Endian with DSP instructions, single precision FPU
%UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "ARMv8MMLldfsp" -o "DspLib_ARMv8MMLldfsp_build.log"

REM echo   Building DSP Library for ARMv8-M Mainline Little Endian with DSP instructions, double precision FPU
REM %UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "ARMv8MMLldfdp" -o "DspLib_ARMv8MMLldfdp_build.log"


REM big endian libraries are skipped!

REM echo   Building DSP Library for Cortex-M0 Big Endian
REM %UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM0b"    -o "DspLib_cortexM0b_build.log"

REM echo   Building DSP Library for Cortex-M3 Big Endian
REM %UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM3b"    -o "DspLib_cortexM3b_build.log"

REM echo   Building DSP Library for Cortex-M4 Big Endian
REM %UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM4b"    -o "DspLib_cortexM4b_build.log"

REM echo   Building DSP Library for Cortex-M4 Big Endian with single precision FPU
REM %UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM4bf"   -o "DspLib_cortexM4bf_build.log"

REM echo   Building DSP Library for Cortex-M7 Big Endian
REM %UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM7b"    -o "DspLib_cortexM7b_build.log"

REM echo   Building DSP Library for Cortex-M7 Big Endian with single precision FPU
REM %UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM7bfsp" -o "DspLib_cortexM7bfsp_build.log"

REM echo   Building DSP Library for Cortex-M7 Big Endian with double precision FPU
REM %UVEXE% -rb -j0 arm_cortexM_math.uvprojx -t "cortexM7bfdp" -o "DspLib_cortexM7bfdp_build.log"

goto deleteIntermediateFiles


:deleteIntermediateFiles
echo.
echo   Deleting intermediate files
rmdir /S /Q IntermediateFiles
del /Q *.bak
del /Q *.dep
del /Q *.uvguix.*
del /Q ArInp.*

goto changeDir


:changeDir
cd %CURDIR%
goto end

:err

:help
echo   Syntax: buildDspLibs toolchain
echo.
echo     toolchain:     ARM ^| GCC
echo.
echo   e.g.: buildDspLibs ARM

:end
