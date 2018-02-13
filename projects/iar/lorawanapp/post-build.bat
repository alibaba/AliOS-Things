set OUT=%1.out
set HEX=%1.hex
set BIN=%1.bin

:: calculate application checksum
ielftool --fill 0xFF;0x0-0xfffb --checksum __checksum:2,crc16,0x0;0x0-0xfffb --verbose %OUT% %OUT%

:: generate additional output: hex
ielftool.exe --ihex --verbose %OUT% %HEX%

:: generate additional output: binary
ielftool.exe --bin --verbose %OUT% %BIN%