Option Explicit
DIM fso    

Dim WshShell

Set fso = CreateObject("Scripting.FileSystemObject")

If (fso.FileExists("""" & WScript.Arguments(0) & """\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\prebuild.bat")) Then
  MsgBox "script not exist " & WScript.Arguments(0) & "\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\prebuild.bat" , vbinformation
  WScript.Quit()
End If

Set WshShell = WScript.CreateObject("WScript.Shell")

WshShell.Run "cmd /c """"" & WScript.Arguments(0) & "\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\prebuild.bat"" """ & WScript.Arguments(0) & """""", 0, true