cd /D %2\platform\mcu\rtl8710bn\
::echo %1 %2 %3 %4 > tmp3.txt
set rootdir=%2
set amebazdir=%2\platform\mcu\rtl8710bn
set tooldir=%2\platform\mcu\rtl8710bn\tools
set libdir=%2\platform\mcu\rtl8710bn\lib
set bindir=%2\platform\mcu\rtl8710bn\Debug\Exe
set ota_bin_ver=0x%date:~0,4%%date:~5,2%%date:~8,2%
set ota_offset=%1
set outputname=%3@%4
set outputdir=%2\out\%outputname%\binary
::echo %outputdir% >tmp2.txt
::echo %outputname% >tmp4.txt
::echo input1=%1 >tmp.txt
::echo input2=%2 >>tmp.txt
::echo ota_bin_ver=%ota_bin_ver% >>tmp.txt
IF NOT EXIST %bindir% MD %bindir%
copy %outputdir%\%outputname%.elf %bindir%\Application.axf
del Debug\Exe\target.map Debug\Exe\application.asm *.bin
cmd /c "%tooldir%\nm %bindir%/application.axf | %tooldir%\sort > %bindir%/application.map"
cmd /c "%tooldir%\objdump -d %bindir%/application.axf > %bindir%/application.asm"

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __ram_image2_text_start__ %bindir%/application.map | %tooldir%\gawk '{print $1}'"') do set ram2_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __ram_image2_text_end__ %bindir%/application.map |  %tooldir%\gawk '{print $1}'"') do set ram2_end=0x%%i

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __xip_image2_start__ %bindir%/application.map | %tooldir%\gawk '{print $1}'"') do set xip_image2_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep __xip_image2_end__ %bindir%/application.map | %tooldir%\gawk '{print $1}'"') do set xip_image2_end=0x%%i

::echo ram2_start: %ram2_start% > tmp.txt
::echo ram2_end: %ram2_end% >> tmp.txt
::echo xip_image2_start: %xip_image2_start% >> tmp.txt
::echo xip_image2_end: %xip_image2_end% >> tmp.txt

::findstr /rg "place" Debug\List\application.map > tmp.txt
::setlocal enabledelayedexpansion
::for /f "delims=:" %%i in ('findstr /rg "0x1000" tmp.txt') do (
::    set "var=%%i"
::    set "sectname_ram2=!var:~1,2!"
::)
::for /f "delims=:" %%i in ('findstr /rg "xip_image2.text" tmp.txt') do (
::    set "var=%%i"
::    set "sectname_xip=!var:~1,2!"
::)
::for /f "delims=:" %%i in ('findstr /rg "0x1003f000" tmp.txt') do (
::    set "var=%%i"
::    set "sectname_rdp=!var:~1,2!"
::)
::setlocal disabledelayedexpansion
::del tmp.txt
::echo sectname_ram2: %sectname_ram2% sectname_xip: %sectname_xip% sectname_rdp: %sectname_rdp% >tmp1.txt

%tooldir%\objcopy -j .ram_image2.entry -j .ram_image2.data -j .ram_image2.text -j .ram_image2.bss -j .ram_image2.skb.bss -j .ram_heap.data -Obinary %bindir%/application.axf %bindir%/ram_2.r.bin
%tooldir%\objcopy -j .xip_image2.text -Obinary %bindir%/application.axf %bindir%/xip_image2.bin
%tooldir%\objcopy -j .ram_rdp.text -Obinary %bindir%/application.axf %bindir%/rdp.bin

:: remove bss sections
%tooldir%\pick %ram2_start% %ram2_end% %bindir%\ram_2.r.bin %bindir%\ram_2.bin raw
del %bindir%\ram_2.r.bin

:: add header
%tooldir%\pick %ram2_start% %ram2_end% %bindir%\ram_2.bin %bindir%\ram_2.p.bin
::%tooldir%\pick %xip_image2_start% %xip_image2_end% Debug\Exe\xip_image2.bin Debug\Exe\xip_image2.p.bin
%tooldir%\pick 0 0 %bindir%\xip_image2.bin %bindir%\xip_image2.p.bin

:: get ota_idx and ota_offset from image2.icf
::setlocal enabledelayedexpansion
::findstr /rg "__ICFEDIT_region_XIP_OTA1_start__" image2.icf>test.txt
::for /f "tokens=1,2,3,4,5*" %%i in ('findstr /rg "symbol" test.txt') do (
::    set "var=%%m"
::    set "ota_offset=!var:~0,10!"
::)
::setlocal disabledelayedexpansion
if "%ota_offset%"=="0x0800B000" (  
    set ota_idx=1
) else (
    set ota_idx=2
)
::del test.txt
::echo ota_idx=%ota_idx% >tmp.txt
::echo ota_offset=%ota_offset% >>tmp.txt

:: aggregate image2_all.bin and add checksum
if "%ota_idx%"=="2" (
    copy /b %bindir%\xip_image2.p.bin+%bindir%\ram_2.p.bin %bindir%\image2_all_ota2.bin
    %tooldir%\checksum %bindir%\image2_all_ota2.bin
    %tooldir%\ota %bindir%\image2_all_ota1.bin 0x800B000 %bindir%\image2_all_ota2.bin %ota_offset% %ota_bin_ver% Debug\Exe\ota_all.bin
) else (
    copy /b %bindir%\xip_image2.p.bin+%bindir%\ram_2.p.bin %bindir%\image2_all_ota1.bin
    %tooldir%\checksum %bindir%\image2_all_ota1.bin
)

del Debug\Exe\ram_2.bin
del Debug\Exe\ram_2.p.bin
del Debug\Exe\xip_image2.bin
del Debug\Exe\xip_image2.p.bin

:: force update boot_all.bin
del Debug\Exe\boot_all.bin

:: check boot_all.bin exist, copy default
if not exist Debug\Exe\boot_all.bin (
	copy %amebazdir%\bin\boot_all.bin %bindir%\boot_all.bin
)


if "%ota_idx%"=="2" (
    copy %bindir%\image2_all_ota2.bin %outputdir%\image2_all_ota2.bin
	copy %bindir%\ota_all.bin %outputdir%\ota_all.bin
) else (
	copy %bindir%\boot_all.bin %outputdir%\boot_all.bin
	copy %bindir%\image2_all_ota1.bin %outputdir%\image2_all_ota1.bin
)

:: board generator
::%tooldir%\..\gen_board_img2.bat %ram2_start% %ram2_end%

exit
