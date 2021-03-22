cmake_minimum_required (VERSION 3.6)

project(CMSISDSPSupport)


file(GLOB SRC "./*_*.c")

add_library(CMSISDSPSupport STATIC ${SRC})

configdsp(CMSISDSPSupport ..)

### Includes
target_include_directories(CMSISDSPSupport PUBLIC "${DSP}/../../Include")



