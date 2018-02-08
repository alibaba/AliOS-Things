Dim WshShell

Set WshShell = WScript.CreateObject("WScript.Shell")

WshShell.Run "cmd /c "+WScript.Arguments.Item(1)+"postbuild_img2.bat "+WScript.Arguments.Item(0)+" "+WScript.Arguments.Item(1), 0
