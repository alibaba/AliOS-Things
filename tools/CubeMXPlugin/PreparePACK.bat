@ECHO OFF

REM Batch File for Add Component into Pack
SET AOS_SRC_ROOT="%cd%\..\..\"
echo %AOS_SRC_ROOT%
echo %cd%

REM Create Folder
IF EXIST ".\Files\aos" (
    echo "aos exist"
) ELSE (
    mkdir ".\Files\aos"
)
set list=board device example framework framework\GUI platform security tools utility platform\mcu platform\arch platform\arch\arm framework\tinyengine 3rdparty
for %%a in (%list%) do ( 
    IF EXIST ".\Files\aos\%%a" ( 
        echo "Already Exist Folder %%a, skip Create it"
    ) ELSE (
        mkdir ".\Files\aos\%%a"
    )
)

for /f "tokens=*" %%a in (packlist.txt) do (
    IF EXIST ".\Files\aos\%%a" (
        echo "Already Linked %%a, skip it"
    ) ELSE (
        MKLINK /D ".\Files\aos\%%a" "%AOS_SRC_ROOT%%%a"
        echo "New LINK Created for %%a"
    )
)

ECHO End of batch file.