cmake_minimum_required (VERSION 3.6)

project(CMSISDSPStatistics)


file(GLOB SRC "./*_*.c")

add_library(CMSISDSPStatistics STATIC ${SRC})

configdsp(CMSISDSPStatistics ..)

### Includes
target_include_directories(CMSISDSPStatistics PUBLIC "${DSP}/../../Include")



