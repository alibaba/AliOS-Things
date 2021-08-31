cmake_minimum_required (VERSION 3.6)

project(CMSISNNConcatenation)

include(configLib)
include(configDsp)

file(GLOB SRC "./*_*.c")

add_library(CMSISNNConcatenation STATIC ${SRC})

configLib(CMSISNNConcatenation ${ROOT})
configDsp(CMSISNNConcatenation ${ROOT})

### Includes
target_include_directories(CMSISNNConcatenation PUBLIC "${NN}/Include")
target_include_directories(CMSISNNConcatenation PUBLIC "${ROOT}/CMSIS/DSP/Include")
