@echo off
set boot_bin=board\%2\l496_bootloader.bin
set app_bin=out\%1@%2\binary\%1@%2.bin
set binpack=tools\binpack_stm32l4xx.exe

%binpack% 0x80000 0x4000 %boot_bin% %app_bin% 1
