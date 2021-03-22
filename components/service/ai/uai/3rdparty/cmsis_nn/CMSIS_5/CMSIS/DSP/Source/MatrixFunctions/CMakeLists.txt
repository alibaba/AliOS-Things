cmake_minimum_required (VERSION 3.6)

project(CMSISDSPMatrix)


file(GLOB SRC "./*_*.c")

add_library(CMSISDSPMatrix STATIC ${SRC})

configdsp(CMSISDSPMatrix ..)

### Includes
target_include_directories(CMSISDSPMatrix PUBLIC "${DSP}/../../Include")



