This directory is needed due to the MinGW version of uname.exe.
The program seems to reference the ../etc directory even though it is empty.
Without this directory uname.exe prints nothing.