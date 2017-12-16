@echo off

SET TMP=C:\Temp
SET TEMP=C:\Temp
SET UVEXE=C:\Keil\UV4\UV4.EXE

echo.
echo Building DSP Libraries ARM
echo.
echo   Building DSP Library for Cortex-M0 Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvproj -t "M0l"  -o "DspLib_M0l_build.log"
echo   Building DSP Library for Cortex-M0 Big Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvproj -t "M0b"  -o "DspLib_M0b_build.log"
echo   Building DSP Library for Cortex-M3 Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvproj -t "M3l"  -o "DspLib_M3l_build.log"
echo   Building DSP Library for Cortex-M3 Big Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvproj -t "M3b"  -o "DspLib_M3b_build.log"
echo   Building DSP Library for Cortex-M4 Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvproj -t "M4l"  -o "DspLib_M4l_build.log"
echo   Building DSP Library for Cortex-M4 Big Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvproj -t "M4b"  -o "DspLib_M4b_build.log"
echo   Building DSP Library for Cortex-M4 with FPU Little Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvproj -t "M4lf" -o "DspLib_M4lf_build.log"
echo   Building DSP Library for Cortex-M4 with FPU Big Endian
%UVEXE% -rb -j0 arm_cortexM_math.uvproj -t "M4bf" -o "DspLib_M4bf_build.log"

echo.
ECHO   Deleting intermediate files
rmdir /S /Q IntermediateFiles\M0l
rmdir /S /Q IntermediateFiles\M0b
rmdir /S /Q IntermediateFiles\M3l
rmdir /S /Q IntermediateFiles\M3b
rmdir /S /Q IntermediateFiles\M4l
rmdir /S /Q IntermediateFiles\M4b
rmdir /S /Q IntermediateFiles\M4lf
rmdir /S /Q IntermediateFiles\M4bf
del /Q IntermediateFiles\*.*
del /Q *.bak
del /Q *.dep
del /Q *.uvgui.*
del /Q ArInp.*