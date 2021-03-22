cmake_minimum_required (VERSION 3.6)
cmake_policy(SET CMP0077 NEW)
# The tests are assuming that MATRIX_CHECK is enabled when building
# CMSIS-DSP.
set(MATRIXCHECK ON)
set(FASTMATHCOMPUTATIONS OFF)
option(DUMPPATTERN "Dump test patterns when test is failing" ON)

option(CUSTOMIZE_TESTS "Enable customizations of tests" ON)
option(BASICMATH_TESTS "Enable Basic Math testing" ON)
option(COMPLEXMATH_TESTS "Enable Complex Math testing" ON)
option(CONTROLLER_TESTS "Enable Controller testing" ON)
option(FASTMATH_TESTS "Enable Fast Math testing" ON)
option(INTRINSICS_TESTS "Enable Intrinsics testing" ON)
option(FILTERING_TESTS "Enable Filtering testing" ON)
option(MATRIX_TESTS "Enable Matrix testing" ON)
option(STATISTICS_TESTS "Enable Statistics testing" ON)
option(SUPPORT_TESTS "Enable Support testing" ON)
option(TRANSFORM_TESTS "Enable Transform testing" ON)


project(DSP_Lib_TestSuite)

# Needed to find the config modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)


set(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)


file(GLOB MAIN "Common/src/*.c")
file(GLOB BASICMATH_TESTS_SRC "Common/src/basic_math_tests/*.c")
file(GLOB COMPLEXMATH_TESTS_SRC "Common/src/complex_math_tests/*.c")
file(GLOB CONTROLLER_TESTS_SRC "Common/src/controller_tests/*.c")
file(GLOB FASTMATH_TESTS_SRC "Common/src/fast_math_tests/*.c")
file(GLOB FILTERING_TESTS_SRC "Common/src/filtering_tests/*.c")
file(GLOB INTRINSINCS_TESTS_SRC "Common/src/intrinsics_tests/*.c")
file(GLOB MATRIX_TESTS_SRC "Common/src/matrix_tests/*.c")
file(GLOB STATISTICS_TESTS_SRC "Common/src/statistics_tests/*.c")
file(GLOB SUPPORT_TESTS_SRC "Common/src/support_tests/*.c")
file(GLOB TRANSFORM_TESTS_SRC "Common/src/transform_tests/*.c")
file(GLOB JTEST_MAIN "Common/JTest/src/*.c")

set(TESTSRC ${MAIN}
  ${BASICMATH_TESTS_SRC}
  ${COMPLEXMATH_TESTS_SRC}
  ${CONTROLLER_TESTS_SRC}
  ${FASTMATH_TESTS_SRC}
  ${FILTERING_TESTS_SRC}
  ${INTRINSINCS_TESTS_SRC}
  ${MATRIX_TESTS_SRC}
  ${STATISTICS_TESTS_SRC}
  ${SUPPORT_TESTS_SRC}
  ${TRANSFORM_TESTS_SRC}
  ${JTEST_MAIN}
  )

set(JINCS 
  Common/JTest/inc
  Common/JTest/inc/arr_desc
  Common/inc/basic_math_tests
  Common/inc/complex_math_tests
  Common/inc/controller_tests
  Common/inc/fast_math_tests
  Common/inc/filtering_tests
  Common/inc/intrinsics_tests
  Common/inc/matrix_tests
  Common/inc/statistics_tests 
  Common/inc/support_tests 
  Common/inc/transform_tests
  )

add_subdirectory(../Source bin_dsp)
add_subdirectory(RefLibs bin_ref)


add_executable(DSP_Lib_TestSuite)

if (CUSTOMIZE_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE CUSTOMIZE_TESTS)
endif()

if (BASICMATH_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_BASICMATH_TESTS)
endif()
if (COMPLEXMATH_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_COMPLEXMATH_TESTS)
endif()
if (CONTROLLER_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_CONTROLLER_TESTS)
endif()
if (FASTMATH_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_FASTMATH_TESTS)
endif()
if (FILTERING_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_FILTERING_TESTS)
endif()
if (INTRINSICS_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_INTRINSICS_TESTS)
endif()
if (MATRIX_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_MATRIX_TESTS)
endif()
if (STATISTICS_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_STATISTICS_TESTS)
endif()
if (SUPPORT_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_SUPPORT_TESTS)
endif()
if (TRANSFORM_TESTS)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE ENABLE_TRANSFORM_TESTS)
endif()


if (DUMPPATTERN)
      target_compile_definitions(DSP_Lib_TestSuite PRIVATE DUMPPATTERN)
endif()

# Change behavior of configBoot for scatter file
set(TESTFRAMEWORK ON)

include(configBoot)

file(COPY ${ROOT}/CMSIS/DSP/Examples/ARM/boot/RTE_Components.h DESTINATION tempLink)

target_link_libraries(DSP_Lib_TestSuite PRIVATE CMSISDSP)
target_link_libraries(DSP_Lib_TestSuite PRIVATE DspRefLibs)

target_sources(DSP_Lib_TestSuite PRIVATE ${TESTSRC})

### Includes
target_include_directories(DSP_Lib_TestSuite PRIVATE "Common/inc")
target_include_directories(DSP_Lib_TestSuite PRIVATE "Common/inc/templates")
target_include_directories(DSP_Lib_TestSuite PRIVATE ${JINCS})


