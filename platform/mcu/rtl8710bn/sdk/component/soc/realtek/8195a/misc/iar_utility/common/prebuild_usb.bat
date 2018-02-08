cd /D %1

set objdir=%1\Debug\Obj

echo off

del /q /f %objdir%\dwc*
del /q /f %objdir%\core_otg*
del /q /f %objdir%\usb*

exit

