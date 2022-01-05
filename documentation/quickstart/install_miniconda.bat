@echo off
rem win os version, refer to https://en.wikipedia.org/wiki/Ver_(command)
CHCP 65001

for /f "tokens=4,5,6 delims=[]. " %%G in ('ver') do (set _major=%%G& set _minor=%%H& set _build=%%I)
echo System information: [%_major%.%_minor%.%_build%]

if %_major% LSS 6 (
    echo Not supported system, minimal Windows 7.
    exit /B 1
)

if "%PROCESSOR_ARCHITECTURE%"=="x86" (
    echo Not supported 32-bit system.
	exit /B 1
)

where curl > NUL 2>&1
if errorlevel 1 (
    echo Please download curl from https://curl.se/windows/, install it and add it into PATH.
    exit /B 1
)

curl -o  %UserProfile%\Miniconda3-latest-Windows-x86_64.exe https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe 

echo Installing miniconda3...
start /wait "" %UserProfile%\Miniconda3-latest-Windows-x86_64.exe /InstallationType=JustMe /AddToPath=0 /RegisterPython=0 /S /D=%UserProfile%\Miniconda3

echo open "Anaconda Prompt(Miniconda3)" to install aos-tools
pause
exit 0
