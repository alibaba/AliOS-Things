cd /D %2
set tooldir=%2\..\..\..\component\soc\realtek\8711b\misc\iar_utility\common\tools
set libdir=%2\..\..\..\component\soc\realtek\8711b\misc\bsp
set ota_bin_ver=0x%date:~0,4%%date:~5,2%%date:~8,2%

::echo input1=%1 >tmp.txt
::echo input2=%2 >>tmp.txt
::echo ota_bin_ver=%ota_bin_ver% >>tmp.txt
 
del Debug\Exe\target.map Debug\Exe\application.asm *.bin
cmd /c "%tooldir%\nm Debug/Exe/application.axf | %tooldir%\sort > Debug/Exe/application.map"
cmd /c "%tooldir%\objdump -d Debug/Exe/application.axf > Debug/Exe/application.asm"

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE2 Debug/Exe/application.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set ram2_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE2 Debug/Exe/application.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set ram2_end=0x%%i

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep xip_image2 Debug/Exe/application.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set xip_image2_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep xip_image2 Debug/Exe/application.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set xip_image2_end=0x%%i

::echo ram2_start: %ram2_start% > tmp.txt
::echo ram2_end: %ram2_end% >> tmp.txt
::echo xip_image2_start: %xip_image2_start% >> tmp.txt
::echo xip_image2_end: %xip_image2_end% >> tmp.txt

findstr /rg "place" Debug\List\application.map > tmp.txt
setlocal enabledelayedexpansion
for /f "delims=:" %%i in ('findstr /rg "0x1000" tmp.txt') do (
    set "var=%%i"
    set "sectname_ram2=!var:~1,2!"
)
for /f "delims=:" %%i in ('findstr /rg "xip_image2.text" tmp.txt') do (
    set "var=%%i"
    set "sectname_xip=!var:~1,2!"
)
for /f "delims=:" %%i in ('findstr /rg "0x1003f000" tmp.txt') do (
    set "var=%%i"
    set "sectname_rdp=!var:~1,2!"
)
setlocal disabledelayedexpansion
del tmp.txt
::echo sectname_ram2: %sectname_ram2% sectname_xip: %sectname_xip% sectname_rdp: %sectname_rdp% >tmp1.txt

%tooldir%\objcopy -j "%sectname_ram2% rw" -Obinary Debug/Exe/application.axf Debug/Exe/ram_2.r.bin
%tooldir%\objcopy -j "%sectname_xip% rw" -Obinary Debug/Exe/application.axf Debug/Exe/xip_image2.bin
%tooldir%\objcopy -j "%sectname_rdp% rw" -Obinary Debug/Exe/application.axf Debug/Exe/rdp.bin

:: remove bss sections
%tooldir%\pick %ram2_start% %ram2_end% Debug\Exe\ram_2.r.bin Debug\Exe\ram_2.bin raw
del Debug\Exe\ram_2.r.bin

:: add header
%tooldir%\pick %ram2_start% %ram2_end% Debug\Exe\ram_2.bin Debug\Exe\ram_2.p.bin
%tooldir%\pick %xip_image2_start% %xip_image2_end% Debug\Exe\xip_image2.bin Debug\Exe\xip_image2.p.bin

:: get ota_idx and ota_offset from image2.icf
setlocal enabledelayedexpansion
findstr /rg "__ICFEDIT_region_XIP_OTA1_start__" image2.icf>test.txt
for /f "tokens=1,2,3,4,5*" %%i in ('findstr /rg "symbol" test.txt') do (
    set "var=%%m"
    set "ota_offset=!var:~0,10!"
)
setlocal disabledelayedexpansion
if "%ota_offset%"=="0x0800B000" (  
    set ota_idx=1
) else (
    set ota_idx=2
)
del test.txt
::echo ota_idx=%ota_idx% >tmp.txt
::echo ota_offset=%ota_offset% >>tmp.txt

:: aggregate image2_all.bin and add checksum
if "%ota_idx%"=="2" (
    copy /b Debug\Exe\xip_image2.p.bin+Debug\Exe\ram_2.p.bin Debug\Exe\image2_all_ota2.bin
    %tooldir%\checksum Debug\Exe\image2_all_ota2.bin
    %tooldir%\ota Debug\Exe\image2_all_ota1.bin 0x800B000 Debug\Exe\image2_all_ota2.bin %ota_offset% %ota_bin_ver% Debug\Exe\ota_all.bin
) else (
    copy /b Debug\Exe\xip_image2.p.bin+Debug\Exe\ram_2.p.bin Debug\Exe\image2_all_ota1.bin
    %tooldir%\checksum Debug\Exe\image2_all_ota1.bin
)

del Debug\Exe\ram_2.bin
del Debug\Exe\ram_2.p.bin
del Debug\Exe\xip_image2.bin
del Debug\Exe\xip_image2.p.bin

:: force update boot_all.bin
del Debug\Exe\boot_all.bin

:: check boot_all.bin exist, copy default
if not exist Debug\Exe\boot_all.bin (
	copy %libdir%\image\boot_all.bin Debug\Exe\boot_all.bin
)

:: board generator
%tooldir%\..\gen_board_img2.bat %ram2_start% %ram2_end%

exit
