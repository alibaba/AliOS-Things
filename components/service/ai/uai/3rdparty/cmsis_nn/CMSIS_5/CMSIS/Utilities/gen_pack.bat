:: Batch file for generating CMSIS pack
:: This batch file uses:
::    7-Zip for packaging
::    Doxygen version 1.8.2 and Mscgen version 0.20 for generating html documentation.
:: The generated pack and pdsc file are placed in folder %RELEASE_PATH% (../../Local_Release)
@ECHO off

SETLOCAL

:: Tool path for zipping tool 7-Zip
SET ZIPPATH=C:\Program Files\7-Zip

:: Tool path for doxygen
SET DOXYGENPATH=C:\Program Files\doxygen\bin

:: Tool path for mscgen utility
SET MSCGENPATH=C:\Program Files (x86)\Mscgen

:: These settings should be passed on to subprocesses as well
SET PATH=%ZIPPATH%;%DOXYGENPATH%;%MSCGENPATH%;%PATH%

:: Pack Path (where generated pack is stored)
SET RELEASE_PATH=..\..\Local_Release

:: !!!!!!!!!!!!!!!!!
:: DO NOT EDIT BELOW
:: !!!!!!!!!!!!!!!!! 

:: Remove previous build
IF EXIST %RELEASE_PATH% (
  ECHO removing %RELEASE_PATH%
  RMDIR /Q /S  %RELEASE_PATH%
)

:: Create build output directory
MKDIR %RELEASE_PATH%


:: Copy PDSC file
COPY ..\..\ARM.CMSIS.pdsc %RELEASE_PATH%\ARM.CMSIS.pdsc

:: Copy LICENSE file
COPY ..\..\LICENSE.txt %RELEASE_PATH%\LICENSE.txt

:: Copy Device folder
XCOPY /Q /S /Y ..\..\Device\*.* %RELEASE_PATH%\Device\*.*

:: Copy CMSIS folder 
:: -- Core files 
XCOPY /Q /S /Y ..\..\CMSIS\Core\Include\*.* %RELEASE_PATH%\CMSIS\Core\Include\*.*
XCOPY /Q /S /Y ..\..\CMSIS\Core\Include\*.* %RELEASE_PATH%\CMSIS\Include\*.*
XCOPY /Q /S /Y ..\..\CMSIS\Core\Template\ARMv8-M\*.* %RELEASE_PATH%\CMSIS\Core\Template\ARMv8-M\*.*
XCOPY /Q /S /Y ..\..\CMSIS\Core_A\Include\*.* %RELEASE_PATH%\CMSIS\Core_A\Include\*.*
XCOPY /Q /S /Y ..\..\CMSIS\Core_A\Source\*.* %RELEASE_PATH%\CMSIS\Core_A\Source\*.*

:: -- DAP files 
XCOPY /Q /S /Y ..\..\CMSIS\DAP\*.* %RELEASE_PATH%\CMSIS\DAP\*.*

:: -- Driver files 
XCOPY /Q /S /Y ..\..\CMSIS\Driver\*.* %RELEASE_PATH%\CMSIS\Driver\*.*

:: -- DSP files 
XCOPY /Q /S /Y ..\..\CMSIS\DSP\Include\*.* %RELEASE_PATH%\CMSIS\DSP\Include\*.*
XCOPY /Q /S /Y ..\..\CMSIS\DSP\Include\*.* %RELEASE_PATH%\CMSIS\Include\*.*
XCOPY /Q /S /Y ..\..\CMSIS\DSP\Source\*.* %RELEASE_PATH%\CMSIS\DSP\Source\*.*
XCOPY /Q /S /Y ..\..\CMSIS\DSP\Projects\*.* %RELEASE_PATH%\CMSIS\DSP\Projects\*.*
XCOPY /Q /S /Y ..\..\CMSIS\DSP\Examples\*.* %RELEASE_PATH%\CMSIS\DSP\Examples\*.*
XCOPY /Q /S /Y ..\..\CMSIS\DSP\Lib\*.* %RELEASE_PATH%\CMSIS\DSP\Lib\*.*

:: -- NN files 
XCOPY /Q /S /Y ..\..\CMSIS\NN\*.* %RELEASE_PATH%\CMSIS\NN\*.*

:: -- Pack files 
XCOPY /Q /S /Y ..\..\CMSIS\Pack\Example\*.*   %RELEASE_PATH%\CMSIS\Pack\Example\*.*
XCOPY /Q /S /Y ..\..\CMSIS\Pack\Tutorials\*.* %RELEASE_PATH%\CMSIS\Pack\Tutorials\*.*

:: -- RTOS files 
XCOPY /Q /S /Y ..\..\CMSIS\RTOS\Template\*.* %RELEASE_PATH%\CMSIS\RTOS\Template\*.*
XCOPY /Q /S /Y ..\..\CMSIS\RTOS\RTX\*.* %RELEASE_PATH%\CMSIS\RTOS\RTX\*.*

:: -- RTOS2 files 
XCOPY /Q /S /Y ..\..\CMSIS\RTOS2\Include\*.* %RELEASE_PATH%\CMSIS\RTOS2\Include\*.*
XCOPY /Q /S /Y ..\..\CMSIS\RTOS2\Source\*.* %RELEASE_PATH%\CMSIS\RTOS2\Source\*.*
XCOPY /Q /S /Y ..\..\CMSIS\RTOS2\Template\*.* %RELEASE_PATH%\CMSIS\RTOS2\Template\*.*
XCOPY /Q /S /Y ..\..\CMSIS\RTOS2\RTX\*.* %RELEASE_PATH%\CMSIS\RTOS2\RTX\*.*

:: -- SVD files 
XCOPY /Q /S /Y ..\..\CMSIS\Utilities\ARM_Example.* %RELEASE_PATH%\CMSIS\SVD\*.*

:: -- Utilities files 
XCOPY /Q /S /Y ..\..\CMSIS\Utilities\CMSIS-SVD.xsd       %RELEASE_PATH%\CMSIS\Utilities\*.*
XCOPY /Q /S /Y ..\..\CMSIS\Utilities\PACK.xsd            %RELEASE_PATH%\CMSIS\Utilities\*.*
XCOPY /Q /S /Y ..\..\CMSIS\Utilities\PackIndex.xsd       %RELEASE_PATH%\CMSIS\Utilities\*.*

XCOPY /Q /S /Y ..\..\CMSIS\Utilities\Win32\*.*           %RELEASE_PATH%\CMSIS\Utilities\Win32\*.*
XCOPY /Q /S /Y ..\..\CMSIS\Utilities\Linux-gcc-4.4.4\*.* %RELEASE_PATH%\CMSIS\Utilities\Linux-gcc-4.4.4\*.*
XCOPY /Q /S /Y ..\..\CMSIS\Utilities\Linux-gcc-4.8.3\*.* %RELEASE_PATH%\CMSIS\Utilities\Linux-gcc-4.8.3\*.*

:: -- index file 
REM COPY ..\..\CMSIS\index.html %RELEASE_PATH%\CMSIS\index.html

:: Generate Documentation 
:: -- Generate doxygen files 
PUSHD ..\DoxyGen

:: -- Delete previous generated HTML files
ECHO.
ECHO Delete previous generated HTML files

PUSHD ..\Documentation
FOR %%A IN (Core, Core_A, DAP, Driver, DSP, General, Pack, RTOS, RTOS2, SVD, Zone) DO IF EXIST %%A (RMDIR /S /Q %%A)
POPD

:: -- Generate HTML Files
ECHO.
ECHO Generate HTML Files

pushd Core
doxygen core.dxy
popd

pushd Core_A
doxygen core_A.dxy
popd

pushd DAP
doxygen dap.dxy
popd

pushd Driver
doxygen Driver.dxy
popd

pushd DSP
doxygen dsp.dxy
popd

pushd General
doxygen general.dxy
popd

pushd NN
doxygen nn.dxy
popd

pushd Pack
doxygen Pack.dxy
popd

pushd RTOS
doxygen rtos.dxy
popd

pushd RTOS2
doxygen rtos.dxy
popd

pushd SVD
doxygen svd.dxy
popd

pushd Zone
doxygen zone.dxy
popd

:: -- Copy search style sheet
ECHO.
ECHO Copy search style sheets
copy /Y Doxygen_Templates\search.css ..\Documentation\CORE\html\search\. 
copy /Y Doxygen_Templates\search.css ..\Documentation\Driver\html\search\.
REM copy /Y Doxygen_Templates\search.css ..\Documentation\General\html\search\. 
copy /Y Doxygen_Templates\search.css ..\Documentation\Pack\html\search\.
REM copy /Y Doxygen_Templates\search.css ..\Documentation\SVD\html\search\.
copy /Y Doxygen_Templates\search.css ..\Documentation\DSP\html\search\.
copy /Y Doxygen_Templates\search.css ..\Documentation\DAP\html\search\.
copy /Y Doxygen_Templates\search.css ..\Documentation\NN\html\search\.
xcopy /E /I /Q /Y Zone\genmodel ..\Documentation\Zone\genmodel

ECHO.
POPD

:: -- Copy generated doxygen files 
XCOPY /Q /S /Y ..\Documentation\*.* %RELEASE_PATH%\CMSIS\Documentation\*.*

:: -- Remove generated doxygen files
PUSHD ..\Documentation
FOR %%A IN (Core, Core_A, DAP, Driver, DSP, General, NN, Pack, RTOS, RTOS2, SVD, Zone) DO IF EXIST %%A (RMDIR /S /Q %%A)
POPD


:: Checking 
Win32\PackChk.exe %RELEASE_PATH%\ARM.CMSIS.pdsc -n %RELEASE_PATH%\PackName.txt -x M353 -x M364 -x M335

:: --Check if PackChk.exe has completed successfully
IF %errorlevel% neq 0 GOTO ErrPackChk

:: Packing 
PUSHD %RELEASE_PATH%

:: -- Pipe Pack's Name into Variable
SET /P PackName=<PackName.txt
DEL /Q PackName.txt

:: Pack files
ECHO Creating pack file ...
7z.exe a %PackName% -tzip > zip.log
ECHO Packaging complete
POPD
GOTO End

:ErrPackChk
ECHO PackChk.exe has encountered an error!
EXIT /b

:End
ECHO Removing temporary files and folders
RMDIR /Q /S  %RELEASE_PATH%\CMSIS
RMDIR /Q /S  %RELEASE_PATH%\Device
DEL %RELEASE_PATH%\LICENSE.txt
DEL %RELEASE_PATH%\zip.log

ECHO gen_pack.bat completed successfully
