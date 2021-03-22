cmake_minimum_required (VERSION 3.6)

project(CMSISDSPCommon)



add_library(CMSISDSPCommon STATIC arm_common_tables.c)

if (CONFIGTABLE AND ALLFFT)
    target_compile_definitions(CMSISDSPCommon PUBLIC ARM_ALL_FFT_TABLES) 
endif()

if (CONFIGTABLE AND ALLFAST)
    target_compile_definitions(CMSISDSPCommon PUBLIC ARM_ALL_FAST_TABLES) 
endif()

include(fft)
fft(CMSISDSPCommon)

include(interpol)
interpol(CMSISDSPCommon)

target_sources(CMSISDSPCommon PRIVATE arm_const_structs.c)

configdsp(CMSISDSPCommon ..)

### Includes
target_include_directories(CMSISDSPCommon PUBLIC "${DSP}/../../Include")



