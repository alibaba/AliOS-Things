set tooldir=%1\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\tools
set alinkdir=%1\..\..\..\component\common\application\alink

::echo %tooldir% > %alinkdir%\test.txt
::echo %alinkdir% >> %alinkdir%\test.txt

cd /D %tooldir%

::echo "%tooldir%\iarchive.exe --extract %alinkdir%\cloud\lib\libalink.a" >> %alinkdir%\test.txt

echo cmd /c iarchive.exe --create lib_alink.a  >out.bat
cmd /c "iarchive.exe -t %alinkdir%\lib_porting.a" >>out.bat
cmd /c "iarchive.exe -t %alinkdir%\cloud\lib\libalink.a" >>out.bat
cmd /c "iarchive.exe -t %alinkdir%\zconfig\lib\libaws.a" >>out.bat
cmd /c sed ':a;N;$ s/\n/ /g;ba' out.bat > out1.bat

cmd /c "iarchive.exe --extract %alinkdir%\cloud\lib\libalink.a"
cmd /c "iarchive.exe --extract %alinkdir%\zconfig\lib\libaws.a"
cmd /c "iarchive.exe --extract %alinkdir%\lib_porting.a"

cmd /c "out1.bat"

del %alinkdir%\lib_porting.a
cmd /c copy lib_alink.a %alinkdir%

del *.o
del *.bat
del *.a 

exit
