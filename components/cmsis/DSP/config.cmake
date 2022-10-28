include(CMakePrintHelpers)
cmake_policy(SET CMP0077 NEW)

SET(CORTEXM ON)
option(FASTMATHCOMPUTATIONS "Fast Math enabled" ON)
option(NEON "Neon acceleration" OFF)
option(NEONEXPERIMENTAL "Neon experimental acceleration" OFF)
option(LOOPUNROLL "Loop unrolling" ON)
option(ROUNDING "Rounding" OFF)
option(MATRIXCHECK "Matrix Checks" OFF)

###################
#
# ALL CORTEX
#

function(configdsp PROJECTNAME DSP)
  target_compile_options(${PROJECTNAME} PUBLIC "-mfloat-abi=hard;-mlittle-endian")

  if (CONFIGTABLE)
      # Public because initialization for FFT may be defined in client code 
      # and needs access to the table.
      target_compile_definitions(${PROJECTNAME} PUBLIC ARM_DSP_CONFIG_TABLES)
  endif()
  
  if (FASTMATHCOMPUTATIONS)
    target_compile_options(${PROJECTNAME} PUBLIC "-ffast-math")
  endif()
  
  if (LOOPUNROLL)
    target_compile_definitions(${PROJECTNAME} PRIVATE ARM_MATH_LOOPUNROLL)
  endif()
  
  if (ROUNDING)
    target_compile_definitions(${PROJECTNAME} PRIVATE ARM_MATH_ROUNDING)
  endif()
  
  if (MATRIXCHECK)
    target_compile_definitions(${PROJECTNAME} PRIVATE ARM_MATH_MATRIX_CHECK)
  endif()
  
  
  ###################
  #
  # CORTEX-A
  #
  
  # CORTEX-A9
  if (ARM_CPU STREQUAL "cortex-a9" )
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core_A/Include")
    SET(CORTEXM OFF)
  
    if (NOT (NEON OR NEONEXPERIMENTAL))
      target_compile_options(${PROJECTNAME} PUBLIC "-mfpu=vfpv3-d16-fp16")
    endif()
  
  endif()
  
  # CORTEX-A7
  if (ARM_CPU STREQUAL "cortex-a7" )
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core_A/Include")
    SET(CORTEXM OFF)
  
    if (NOT (NEON OR NEONEXPERIMENTAL))
      target_compile_options(${PROJECTNAME} PUBLIC "-mfpu=vfpv4-d16")
    endif()
  
  endif()
  
  # CORTEX-A5
  if (ARM_CPU STREQUAL "cortex-a5" )
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core_A/Include")
    SET(CORTEXM OFF)
    
    if ((NEON OR NEONEXPERIMENTAL))
      target_compile_options(${PROJECTNAME} PUBLIC "-mfpu=neon-vfpv4")
    else()
      target_compile_options(${PROJECTNAME} PUBLIC "-mfpu=vfpv4-d16")
    endif()
  endif()
  
  
  ###################
  #
  # CORTEX-M
  #
  
  # CORTEX-M35
  if (ARM_CPU STREQUAL "cortex-m35")
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core/Include")
  endif()
  
  # CORTEX-M33
  if (ARM_CPU STREQUAL "cortex-m33")
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core/Include")
  endif()
  
  # CORTEX-M23
  if (ARM_CPU STREQUAL "cortex-m23")
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core/Include")
  endif()
  
  # CORTEX-M7
  if (ARM_CPU STREQUAL "cortex-m7")
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core/Include")  
  endif()
  
  # CORTEX-M4
  if (ARM_CPU STREQUAL "cortex-m4")
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core/Include")
  
  endif()
  
  # CORTEX-M3
  if (ARM_CPU STREQUAL "cortex-m3")
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core/Include")
  endif()
  
  # CORTEX-M0plus
  if (ARM_CPU STREQUAL "cortex-m0p")
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core/Include")
  endif()
  
  # CORTEX-M0
  if (ARM_CPU STREQUAL "cortex-m0")
    target_include_directories(${PROJECTNAME} PUBLIC "${DSP}/../../Core/Include")
  endif()
  
  ###################
  #
  # FEATURES
  #
    
  if (NEON AND NOT CORTEXM)
    target_compile_definitions(${PROJECTNAME} PRIVATE ARM_MATH_NEON __FPU_PRESENT)
  endif()
  
  if (NEONEXPERIMENTAL AND NOT CORTEXM)
    target_compile_definitions(${PROJECTNAME} PRIVATE ARM_MATH_NEON_EXPERIMENTAL __FPU_PRESENT)
  endif()
endfunction()