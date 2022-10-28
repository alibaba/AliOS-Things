cmake_minimum_required (VERSION 3.6)
project (arm_variance_example VERSION 0.1)

# Needed to include the configBoot module
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

################################### 
#
# LIBRARIES
#
###################################

########### 
#
# CMSIS DSP
#

add_subdirectory(../../../Source bin_dsp)


################################### 
#
# TEST APPLICATION
#
###################################


add_executable(arm_variance_example)

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../..)

include(configBoot)

target_sources(arm_variance_example PRIVATE arm_variance_example_f32.c)

### Sources and libs

target_link_libraries(arm_variance_example PRIVATE CMSISDSP)

################################### 
#
# INSTALLATION
#
###################################

install (TARGETS arm_variance_example DESTINATION "${PROJECT_SOURCE_DIR}/varianceExampleBuild.axf")