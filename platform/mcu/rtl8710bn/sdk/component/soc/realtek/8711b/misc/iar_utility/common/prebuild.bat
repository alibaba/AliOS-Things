cd /D %1
set ota_idx=%2
set ota_offset=%3

:: Generate build_info.h
::echo off
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
echo #define RTL8195AFW_COMPILER "IAR compiler" >> ..\inc\build_info.h

if "%ota_idx%"=="0" (
    goto :end
)

set cf=image2.icf
if "%ota_idx%"=="2" (
    set dt=%ota_offset%
) else (
    set dt=0x0800B000
)

findstr /rg "__ICFEDIT_region_XIP_OTA1_start__" %cf%>ttst.txt
setlocal enabledelayedexpansion
for /f "tokens=1,2,3,4,5*" %%i in ('findstr /rg "symbol" ttst.txt') do (
    set "var=%%m"
    set "st=!var:~0,10!"
)
setlocal disabledelayedexpansion
del ttst.txt

echo /*###ICF### Section handled by ICF editor, don't touch! ****/>root.txt
setlocal enabledelayedexpansion
for /f "skip=1 tokens=1,2* delims=@" %%i in ('findstr ".*" %cf%') do (
    set txt=%%i
    echo !txt:%st%=%dt%!>>root.txt
)
move /y root.txt image2.icf

:end
exit

