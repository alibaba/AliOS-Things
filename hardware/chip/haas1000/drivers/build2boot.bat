
set PATH=%CD%\..\..\..\..\build\compiler\gcc-arm-none-eabi\Win32\bin\;%PATH%

make T=ota_boot2a clean --print-directory
make T=ota_boot2a CPU=m4 SECURE_BOOT=1 REMAP_SUPPORT=1 SDK=1 HW_MODULE=1 LARGE_RAM=1 all lst --print-directory
copy out\ota_boot2a\ota_boot2a.bin ..\prebuild\ota_boot2a.bin