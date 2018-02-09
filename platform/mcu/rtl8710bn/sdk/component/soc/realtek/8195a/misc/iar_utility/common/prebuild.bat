cd /D %1
set tooldir=%1\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\tools
set libdir=%1\..\..\..\component\soc\realtek\8195a\misc\bsp
:: Generate build_info.h
echo off
::echo %date:~0,10%-%time:~0,8%
::echo %USERNAME%
for /f "usebackq" %%i in (`hostname`) do set hostname=%%i
::echo %hostname%

echo #define UTS_VERSION "%date:~0,10%-%time:~0,8%" > ..\inc\build_info.h
echo #define RTL8195AFW_COMPILE_TIME "%date:~0,10%-%time:~0,8%" >> ..\inc\build_info.h
echo #define RTL8195AFW_COMPILE_DATE "%date:~0,10%" >> ..\inc\build_info.h
echo #define RTL8195AFW_COMPILE_BY "%USERNAME%" >> ..\inc\build_info.h
echo #define RTL8195AFW_COMPILE_HOST "%hostname%" >> ..\inc\build_info.h
echo #define RTL8195AFW_COMPILE_DOMAIN >> ..\inc\build_info.h
echo #define RTL195AFW_COMPILER "IAR compiler" >> ..\inc\build_info.h

echo. > main.icf

for /f "delims=" %%i in ('cmd /c "%tooldir%\coan defs -g e ../src/main.c | %tooldir%\grep "#define" | %tooldir%\grep __ICFEDIT_region_BD_RAM_start__ | %tooldir%\gawk '{print $3}'"') do set BD_RAM_start=%%i

if defined %BD_RAM_start (
	echo define symbol __ICFEDIT_region_BD_RAM_start__ = %BD_RAM_start%; >> main.icf
	echo define symbol __ICFEDIT_region_BD_RAM_end__ = 0x1006CFFF; >> main.icf
)

exit

