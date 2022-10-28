@echo off

set UVEXE=C:\Keil_v5\UV4\UV4.EXE
set CURDIR=%CD%

if .%1==. goto help
for %%a in (ARM ARMCLANG GCC) do if %1==%%a goto startBuild
goto help

:startBuild
echo.
echo Building DSP Reference Libraries %1

if %1==ARM                goto buildARM
if %1==ARMCLANG           goto buildARMCLANG
if %1==GCC                goto buildGCC
goto err

:buildARM
:buildARMCLANG
:buildGCC
cd .\RefLibs\%1

echo   Building DSP Reference Library for Cortex-M0 Little Endian
%UVEXE% -rb -j0  RefLibs.uvprojx -t "cortexM0l"    -o "RefLib_cortexM0l_build.log"

echo   Building DSP Reference Library for Cortex-M3 Little Endian
%UVEXE% -rb -j0  RefLibs.uvprojx -t "cortexM3l"    -o "RefLib_cortexM3l_build.log"

echo   Building DSP Reference Library for Cortex-M4 Little Endian
%UVEXE% -rb -j0  RefLibs.uvprojx -t "cortexM4l"    -o "RefLib_cortexM4l_build.log"

echo   Building DSP Reference Library for Cortex-M4 Little Endian with single precision FPU
%UVEXE% -rb -j0  RefLibs.uvprojx -t "cortexM4lf"   -o "RefLib_cortexM4lf_build.log"

echo   Building DSP Reference Library for Cortex-M7 Little Endian
%UVEXE% -rb -j0 RefLibs.uvprojx -t "cortexM7l"     -o "RefLib_cortexM7l_build.log"

echo   Building DSP Reference Library for Cortex-M7 Little Endian with single precision FPU
%UVEXE% -rb -j0 RefLibs.uvprojx -t "cortexM7lfsp"  -o "RefLib_cortexM7lfsp_build.log"

echo   Building DSP Reference Library for Cortex-M7 Little Endian with double precision FPU
%UVEXE% -rb -j0 RefLibs.uvprojx -t "cortexM7lfdp"  -o "RefLib_cortexM7lfdp_build.log"

echo   Building DSP Reference Library for ARMv8-M Baseline Little Endian
%UVEXE% -rb -j0 RefLibs.uvprojx -t "ARMv8MBLl"     -o "RefLib_ARMv8MBLl_build.log"

echo   Building DSP Reference Library for ARMv8-M Mainline Little Endian
%UVEXE% -rb -j0 RefLibs.uvprojx -t "ARMv8MMLl"     -o "RefLib_ARMv8MMLl_build.log"

echo   Building DSP Reference Library for ARMv8-M Mainline Little Endian with single precision FPU
%UVEXE% -rb -j0 RefLibs.uvprojx -t "ARMv8MMLlfsp"  -o "RefLib_ARMv8MMLlfsp_build.log"

REM echo   Building DSP Reference Library for ARMv8-M Mainline Little Endian with double precision FPU
REM %UVEXE% -rb -j0 RefLibs.uvprojx -t "ARMv8MMLlfdp"  -o "RefLib_ARMv8MMLlfdp_build.log"

echo   Building DSP Reference Library for ARMv8-M Mainline Little Endian with DSP instructions
%UVEXE% -rb -j0 RefLibs.uvprojx -t "ARMv8MMLld"    -o "RefLib_ARMv8MMLld_build.log"

echo   Building DSP Reference Library for ARMv8-M Mainline Little Endian with DSP instructions, single precision FPU
%UVEXE% -rb -j0 RefLibs.uvprojx -t "ARMv8MMLldfsp" -o "RefLib_ARMv8MMLldfsp_build.log"

REM echo   Building DSP Reference Library for ARMv8-M Mainline Little Endian with DSP instructions, double precision FPU
REM %UVEXE% -rb -j0 RefLibs.uvprojx -t "ARMv8MMLldfdp" -o "RefLib_ARMv8MMLldfdp_build.log"


REM big endian libraries are skipped!

REM echo   Building DSP Reference Library for Cortex-M0 Big Endian
REM %UVEXE% -rb -j0  RefLibs.uvprojx -t"cortexM0b"    -o "RefLib_cortexM0b_build.log"

REM echo   Building DSP Reference Library for Cortex-M3 Big Endian
REM %UVEXE% -rb -j0  RefLibs.uvprojx -t"cortexM3b"    -o "RefLib_cortexM3b_build.log"

REM echo   Building DSP Reference Library for Cortex-M4 Big Endian
REM %UVEXE% -rb -j0  RefLibs.uvprojx -t"cortexM4b"    -o "RefLib_cortexM4b_build.log"

REM echo   Building DSP Reference Library for Cortex-M4 Big Endian Big Endian with single precision FPU
REM %UVEXE% -rb -j0  RefLibs.uvprojx -t"cortexM4bf"   -o "RefLib_cortexM4bf_build.log"

REM echo   Building DSP Reference Library for Cortex-M7 Big Endian
REM %UVEXE% -rb -j0 RefLibs.uvprojx -t "cortexM7b"    -o "RefLib_cortexM7b_build.log"

REM echo   Building DSP Reference Library for Cortex-M7 Big Endian with single precision FPU
REM %UVEXE% -rb -j0 RefLibs.uvprojx -t "cortexM7bfsp" -o "RefLib_cortexM7bfsp_build.log"

REM echo   Building DSP Reference Library for Cortex-M7 Big Endian with double precision FPU
REM %UVEXE% -rb -j0 RefLibs.uvprojx -t "cortexM7bfdp" -o "RefLib_cortexM7bfdp_build.log"

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
echo   Syntax: buildRefLibs toolchain
echo.
echo     toolchain:     ARM ^| ARMCLANG ^| GCC
echo.
echo   e.g.: buildRefLibs ARM

:end
