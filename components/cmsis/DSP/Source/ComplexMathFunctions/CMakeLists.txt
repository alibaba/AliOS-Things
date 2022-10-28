cmake_minimum_required (VERSION 3.6)

project(CMSISDSPComplexMath)


file(GLOB SRC "./*_*.c")

add_library(CMSISDSPComplexMath STATIC ${SRC})

configdsp(CMSISDSPComplexMath ..)

### Includes
target_include_directories(CMSISDSPComplexMath PUBLIC "${DSP}/../../Include")



