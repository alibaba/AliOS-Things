@echo off

set UVEXE=C:\Keil_v5\UV4\UV4.EXE

echo Building NN Test for Cortex-M7 Single Precision Little Endian
%UVEXE% -rb -j0 arm_nnexamples_nn_test.uvprojx -t "ARMCM7_SP" -o "NN_Test_ARMCM7_SP_build.log"

echo Running NN Test for Cortex-M7 Single Precision Little Endian
%UVEXE% -d arm_nnexamples_nn_test.uvprojx -t "ARMCM7_SP" -j0

type NN_TEST.log
del NN_TEST.log

echo Building NN Test for Cortex-M4 FP Little Endian
%UVEXE% -rb -j0 arm_nnexamples_nn_test.uvprojx -t "ARMCM4_FP" -o "NN_Test_ARMCM4_FP_build.log"

echo Running NN Test for Cortex-M4 FP Little Endian
%UVEXE% -d arm_nnexamples_nn_test.uvprojx -t "ARMCM4_FP" -j0

type NN_TEST.log
del NN_TEST.log

echo Building NN Test for Cortex-M4 FP Little Endian Truncate
%UVEXE% -rb -j0 arm_nnexamples_nn_test.uvprojx -t "ARMCM4_FP_truncate" -o "NN_Test_ARMCM4_FP_truncate_build.log"

echo Running NN Test for Cortex-M4 FP Little Endian Truncate
%UVEXE% -d arm_nnexamples_nn_test.uvprojx -t "ARMCM4_FP_truncate" -j0

type NN_TEST.log
del NN_TEST.log

echo Building NN Test for Cortex-M7 Single Precision Big Endian
%UVEXE% -rb -j0 arm_nnexamples_nn_test.uvprojx -t "ARMCM7b_SP" -o "NN_Test_ARMCM7b_SP_build.log"

echo Running NN Test for Cortex-M7 Single Precision Big Endian
%UVEXE% -d arm_nnexamples_nn_test.uvprojx -t "ARMCM7b_SP" -j0

type NN_TEST.log
del NN_TEST.log

echo Building NN Test for Cortex-M4 FP Big Endian
%UVEXE% -rb -j0 arm_nnexamples_nn_test.uvprojx -t "ARMCM4b_FP" -o "NN_Test_ARMCM4b_FP_build.log"

echo Running NN Test for Cortex-M4 FP Big Endian
%UVEXE% -d arm_nnexamples_nn_test.uvprojx -t "ARMCM4b_FP" -j0

type NN_TEST.log
del NN_TEST.log

echo Building NN Test for Cortex-M3
%UVEXE% -rb -j0 arm_nnexamples_nn_test.uvprojx -t "ARMCM3" -o "NN_Test_ARMCM3_build.log"

echo Running NN Test for Cortex-M3
%UVEXE% -d arm_nnexamples_nn_test.uvprojx -t "ARMCM3" -j0

type NN_TEST.log
del NN_TEST.log