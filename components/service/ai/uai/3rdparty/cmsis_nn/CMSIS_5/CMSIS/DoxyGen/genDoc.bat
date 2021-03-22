@echo off

REM ====================================================================================
REM Batch file for generating
REM
REM Author  : 
REM Date    :  7th April 2016
REM Version : 1.0
REM Company : ARM 
REM
REM 
REM Command syntax: genDoc.bat
REM
REM  Version: 1.0 Initial Version.
REM ====================================================================================

SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

REM -- Delete previous generated HTML files ---------------------
  ECHO.
  ECHO Delete previous generated HTML files

REM -- Remove generated doxygen files ---------------------
PUSHD ..\Documentation
FOR %%A IN (Core, Core_A, DAP, Driver, DSP, NN, General, Pack, RTOS, RTOS2, SVD, Zone) DO IF EXIST %%A (RMDIR /S /Q %%A)
POPD

REM -- Generate New HTML Files ---------------------
  ECHO.
  ECHO Generate New HTML Files

pushd Core
CALL doxygen_core.bat
popd

pushd Core_A
CALL doxygen_core_A.bat
popd

pushd DAP
CALL doxygen_dap.bat
popd

pushd Driver
CALL doxygen_driver.bat
popd

pushd DSP
CALL doxygen_dsp.bat
popd

pushd NN
CALL doxygen_nn.bat
popd

pushd General
CALL doxygen_general.bat
popd

pushd Pack
CALL doxygen_pack.bat
popd

pushd RTOS
CALL doxygen_rtos.bat
popd

pushd RTOS2
CALL doxygen_rtos.bat
popd

pushd SVD
CALL doxygen_svd.bat
popd

pushd Zone
CALL doxygen_zone.bat
popd

REM -- Copy search style sheet ---------------------
  ECHO.
  ECHO Copy search style sheets
copy /Y Doxygen_Templates\search.css ..\Documentation\Core\html\search\. 
copy /Y Doxygen_Templates\search.css ..\Documentation\Core_A\html\search\. 
copy /Y Doxygen_Templates\search.css ..\Documentation\Driver\html\search\.
REM copy /Y Doxygen_Templates\search.css ..\Documentation\General\html\search\. 
copy /Y Doxygen_Templates\search.css ..\Documentation\Pack\html\search\.
REM copy /Y Doxygen_Templates\search.css ..\Documentation\SVD\html\search\.
copy /Y Doxygen_Templates\search.css ..\Documentation\DSP\html\search\.
copy /Y Doxygen_Templates\search.css ..\Documentation\NN\html\search\.
copy /Y Doxygen_Templates\search.css ..\Documentation\DAP\html\search\.
copy /Y Doxygen_Templates\search.css ..\Documentation\RTOS\html\search\.
copy /Y Doxygen_Templates\search.css ..\Documentation\RTOS2\html\search\.
copy /Y Doxygen_Templates\search.css ..\Documentation\Zone\html\search\. 
  
:END
  ECHO.
REM done
