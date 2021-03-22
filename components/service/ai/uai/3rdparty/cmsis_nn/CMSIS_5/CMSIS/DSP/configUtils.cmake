function(cortexm CORE)
    target_sources(${PROJECT_NAME} PRIVATE ${ROOT}/Device/ARM/${CORE}/Source/ARM/startup_${CORE}.s)
    target_sources(${PROJECT_NAME} PRIVATE ${ROOT}/Device/ARM/${CORE}/Source/system_${CORE}.c)
    target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT}/Device/ARM/${CORE}/Include)
    target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT}/CMSIS/Core/Include)
  
    if (TESTFRAMEWORK)
      # Need bigger sections for test framework
      # So we use the test framework scatter file
      set(SCATTERFILE "${ROOT}/CMSIS/DSP/DSP_Lib_TestSuite/Common/platform/ARMCLANG/armcc6_arm.sct")
    else()
      set(SCATTERFILE "${ROOT}/Device/ARM/${CORE}/Source/ARM/${CORE}_ac6.sct")
    endif()

    target_link_options(${PROJECT_NAME} PRIVATE "--info=sizes;--entry=Reset_Handler;--scatter=${SCATTERFILE}")

endfunction()

function(cortexa CORE)
    target_sources(${PROJECT_NAME} PRIVATE ${ROOT}/Device/ARM/${CORE}/Source/mmu_${CORE}.c)
    target_sources(${PROJECT_NAME} PRIVATE ${ROOT}/Device/ARM/${CORE}/Source/system_${CORE}.c)
    target_sources(${PROJECT_NAME} PRIVATE ${ROOT}/CMSIS/Core_A/Source/irq_ctrl_gic.c)
    
    target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT}/Device/ARM/${CORE}/Include)
    target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT}/Device/ARM/${CORE}/Config)
    target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT}/CMSIS/Core_A/Include)
    
    target_sources(${PROJECT_NAME} PRIVATE ${ROOT}/Device/ARM/${CORE}/Source/AC6/startup_${CORE}.c)
    
    if (TESTFRAMEWORK)
      # Test scatter file is missing some sections required by startup file for
      # cortex-a
      #set(SCATTERFILE "${ROOT}/CMSIS/DSP/DSP_Lib_TestSuite/Common/platform/ARMCLANG/armcc6_arm.sct")
      target_include_directories(${PROJECT_NAME} PRIVATE ../Examples/ARM/boot)
    else()
      target_include_directories(${PROJECT_NAME} PRIVATE ../boot)
    endif()

    set(SCATTERFILE ${CMAKE_CURRENT_BINARY_DIR}/tempLink/${CORE}.sct)

    
    # Copy the mem file to the build directory 
    # so that it can be find when preprocessing the scatter file
    # since we cannot pass an include path to armlink
    file(COPY ${ROOT}/Device/ARM/${CORE}/Config/mem_${CORE}.h DESTINATION tempLink)
    file(COPY ${ROOT}/Device/ARM/${CORE}/Source/AC6/${CORE}.sct DESTINATION tempLink)
    
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DCMSIS_device_header="${CORE}.h")

    target_link_options(${PROJECT_NAME} PRIVATE "--info=sizes;--entry=Vectors;--scatter=${SCATTERFILE}")
endfunction()