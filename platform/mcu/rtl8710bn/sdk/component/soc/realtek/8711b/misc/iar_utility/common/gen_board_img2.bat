@set /a tmp1 = %1 
@call :toHex %tmp1% ram_start
@set /a tmp2 = %2-1
@call :toHex %tmp2% ram_end

@echo echo image 2 start %1
@echo echo image 2 end %2
@echo off
@echo ^<?xml version="1.0" encoding="iso-8859-1"?^> > tmp.board
@echo.  >> tmp.board
@echo ^<flash_board^> >> tmp.board
@echo   ^<pass^>    >> tmp.board
@echo     ^<range^>CODE 0x8000000 0x8004fff^</range^>    >> tmp.board
@echo     ^<loader^>$PROJ_DIR$\..\..\..\component\soc\realtek\8711b\misc\iar_utility\common\flashloader\FlashRTL8195aMP.flash^</loader^>    >> tmp.board
@echo     ^<abs_offset^>0x00000000^</abs_offset^>    >> tmp.board
@echo   ^</pass^>    >> tmp.board
@echo   ^<pass^> >> tmp.board
@echo     ^<range^>CODE 0x800b020 0x807ffff^</range^> >> tmp.board
@echo     ^<loader^>$PROJ_DIR$\..\..\..\component\soc\realtek\8711b\misc\iar_utility\common\flashloader\FlashRTL8195aMP.flash^</loader^>    >> tmp.board
@echo     ^<abs_offset^>0xb020^</abs_offset^>  >> tmp.board
@echo   ^</pass^> >> tmp.board
@echo   ^<pass^>    >> tmp.board
@echo     ^<range^>CODE %1 0x%ram_end%^</range^>    >> tmp.board
@echo     ^<loader^>$PROJ_DIR$\..\..\..\component\soc\realtek\8711b\misc\iar_utility\common\flashloader\FlashRTL8195aMP.flash^</loader^>    >> tmp.board  
@echo     ^<abs_offset^>0xb000^</abs_offset^>  >> tmp.board  
@echo     ^<args^>--concat^</args^>  >> tmp.board  
@echo   ^</pass^>    >> tmp.board

@echo   ^<ignore^>CODE %2 0x1003dfff^</ignore^> >> tmp.board
@echo ^</flash_board^> >> tmp.board    >> tmp.board

exit

:toHex dec hex -- convert a decimal number to hexadecimal, i.e. -20 to FFFFFFEC or 26 to 0000001A
@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
set /a dec=%~1
set "hex="
set "map=0123456789ABCDEF"
for /L %%N in (1,1,8) do (
    set /a "d=dec&15,dec>>=4"
    for %%D in (!d!) do set "hex=!map:~%%D,1!!hex!"
)

( ENDLOCAL & REM RETURN VALUES
    IF "%~2" NEQ "" (SET %~2=%hex%) ELSE ECHO.%hex%
)
EXIT /b
