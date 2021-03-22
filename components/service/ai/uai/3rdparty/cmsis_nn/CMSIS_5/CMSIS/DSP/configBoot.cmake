include(CMakePrintHelpers)
include(configUtils)

enable_language(C ASM)

option(FILEIO "Test trace using printf" ON)

# Otherwise there is a .obj on windows and it creates problems
# with armlink. 
SET(CMAKE_C_OUTPUT_EXTENSION .o)
SET(CMAKE_CXX_OUTPUT_EXTENSION .o)
SET(CMAKE_ASM_OUTPUT_EXTENSION .o)


get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

cmake_print_variables(PROJECT_NAME)

#set(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../..)

if (ARMAC6)

  ###################
  #
  # Cortex cortex-m7
  #
  if (ARM_CPU STREQUAL "cortex-m7")
    cortexm(ARMCM7)

    target_compile_definitions(${PROJECT_NAME} PRIVATE ARMCM7_DP)
    
    
  endif()
  
  ###################
  #
  # Cortex cortex-m4
  #
  if (ARM_CPU STREQUAL "cortex-m4")
      cortexm(ARMCM4)
      target_compile_definitions(${PROJECT_NAME} PRIVATE ARMCM4_FP)
  endif()
  
  ###################
  #
  # Cortex cortex-m35p
  #
  if (ARM_CPU STREQUAL "cortex-m35")
      cortexm(ARMCM35P)
      target_compile_definitions(${PROJECT_NAME} PRIVATE ARMCM35P)
  endif()
  
  ###################
  #
  # Cortex cortex-m33
  #
  if (ARM_CPU STREQUAL "cortex-m33")
      cortexm(ARMCM33)
      target_compile_definitions(${PROJECT_NAME} PRIVATE ARMCM33)
  endif()
  
  ###################
  #
  # Cortex cortex-m23
  #
  if (ARM_CPU STREQUAL "cortex-m23")
      cortexm(ARMCM23)
      target_compile_definitions(${PROJECT_NAME} PRIVATE ARMCM23)
  endif()

  ###################
  #
  # Cortex cortex-m0+
  #
  if (ARM_CPU STREQUAL "cortex-m0p")
      cortexm(ARMCM0plus)
  endif()

  ###################
  #
  # Cortex cortex-m0
  #
  if (ARM_CPU STREQUAL "cortex-m0")
      cortexm(ARMCM0)
  endif()
  
  ###################
  #
  # Cortex cortex-a5
  #
  if (ARM_CPU STREQUAL "cortex-a5")
    cortexa(ARMCA5)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ARMv7A) 
  endif()

  ###################
  #
  # Cortex cortex-a7
  #
  if (ARM_CPU STREQUAL "cortex-a7")
    cortexa(ARMCA7)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ARMv7A) 
  endif()

  ###################
  #
  # Cortex cortex-a9
  #
  if (ARM_CPU STREQUAL "cortex-a9")
    cortexa(ARMCA9)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ARMv7A) 
  endif()
  
endif()

if (FILEIO)
  target_compile_definitions(${PROJECT_NAME} PRIVATE FILEIO)
endif()