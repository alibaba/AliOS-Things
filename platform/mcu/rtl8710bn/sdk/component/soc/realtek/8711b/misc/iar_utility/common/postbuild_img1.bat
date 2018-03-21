cd /D %2
set tooldir=%2\..\..\..\component\soc\realtek\8711b\misc\iar_utility\common\tools
set libdir=%2\..\..\..\component\soc\realtek\8711b\misc\bsp

del Debug\Exe\bootloader.map Debug\Exe\bootloader.asm *.bin
cmd /c "%tooldir%\nm Debug/Exe/bootloader.axf | %tooldir%\sort > Debug/Exe/bootloader.map"
cmd /c "%tooldir%\objdump -d Debug/Exe/bootloader.axf > Debug/Exe/bootloader.asm"

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE1 Debug/Exe/bootloader.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set ram1_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep IMAGE1 Debug/Exe/bootloader.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set ram1_end=0x%%i

for /f "delims=" %%i in ('cmd /c "%tooldir%\grep xip_image1 Debug/Exe/bootloader.map | %tooldir%\grep Base | %tooldir%\gawk '{print $1}'"') do set xip_image1_start=0x%%i
for /f "delims=" %%i in ('cmd /c "%tooldir%\grep xip_image1 Debug/Exe/bootloader.map | %tooldir%\grep Limit | %tooldir%\gawk '{print $1}'"') do set xip_image1_end=0x%%i

::echo ram1_start: %ram1_start% > tmp.txt
::echo ram1_end: %ram1_end% >> tmp.txt
::echo xip_image_start: %xip_image1_start% >> tmp.txt
::echo xip_image1_end: %xip_image1_end% >> tmp.txt

findstr /rg "place" Debug\List\bootloader.map > tmp.txt
setlocal enabledelayedexpansion
for /f "delims=:" %%i in ('findstr /rg "IMAGE1" tmp.txt') do (
    set "var=%%i"
    set "sectname_ram1=!var:~1,2!"
)
for /f "delims=:" %%i in ('findstr /rg "xip_image1.text" tmp.txt') do (
    set "var=%%i"
    set "sectname_xip1=!var:~1,2!"
)
setlocal disabledelayedexpansion
del tmp.txt
::echo sectname_ram1: %sectname_ram1% sectname_xip: %sectname_xip1% >tmp.txt

:: pick ram_1.bin
%tooldir%\objcopy -j "%sectname_ram1% rw" -Obinary Debug/Exe/bootloader.axf Debug/Exe/ram_1.bin
:: add header
%tooldir%\pick %ram1_start% %ram1_end% Debug\Exe\ram_1.bin Debug\Exe\ram_1.p.bin boot

:: pick xip_image1.bin
%tooldir%\objcopy -j "%sectname_xip1% rw" -Obinary Debug/Exe/bootloader.axf Debug/Exe/xip_image1.bin
:: add header
%tooldir%\pick %xip_image1_start% %xip_image1_end% Debug\Exe\xip_image1.bin Debug\Exe\xip_image1.p.bin boot

:: aggregate boot_all.bin
copy /b Debug\Exe\xip_image1.p.bin+Debug\Exe\ram_1.p.bin Debug\Exe\boot_all.bin

:: update boot_all.bin, raw file for application
copy Debug\Exe\boot_all.bin %libdir%\image\boot_all.bin

::padding boot_all.bin to 32K+4K+4K+4K, LOADER/RSVD/SYSTEM/CALIBRATION
::%tooldir%\padding 44k 0xFF Debug\Exe\boot_all.bin

:: create bootloader extern symbols
echo /* Bootloader symbol list */ > bootloader_symbol.icf
setlocal enabledelayedexpansion
for /f "tokens=1,2,3 delims= " %%i in ( Debug/Exe/bootloader.map ) do (
    set "var=%%i"
    if /i "!var:~7!"=="a" (
        if %%j==T (echo %%k 0x!var:~0,7!b >> test1.txt)
    ) else (
        if  /i "!var:~7!"=="c" (
            if %%j==T (echo %%k 0x!var:~0,7!d >> test1.txt)
        ) else (
            if  /i "!var:~7!"=="e" (
                if %%j==T (echo %%k 0x!var:~0,7!f >> test1.txt)
            ) else (
                set "n=!var:~7!"
                set /a n+=1
                if %%j==T (echo %%k 0x!var:~0,7!!n! >> test1.txt)
            )
        )
    )
)
for /f "eol=. tokens=1,2" %%i in ( test1.txt ) do echo %%i %%j >> test2.txt
findstr /v "Region" "test2.txt">>test3.txt
for /f "eol=_ tokens=1,2" %%i in ( test3.txt ) do echo define exported symbol %%i = %%j; >> bootloader_symbol.icf 
del test1.txt
del test2.txt
del test3.txt

del Debug\Exe\ram_1.bin
del Debug\Exe\ram_1.p.bin
del Debug\Exe\xip_image1.bin
del Debug\Exe\xip_image1.p.bin

exit
