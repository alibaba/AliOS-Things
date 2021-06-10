SET TOOLS_FOLDER=%~dp0

%TOOLS_FOLDER%\mklittlefs.exe -c %TOOLS_FOLDER%\..\\prebuild\\data\\ -d 5 -b 4096 -p 256 -s 684032 %TOOLS_FOLDER%\..\\prebuild\\littlefs.bin
