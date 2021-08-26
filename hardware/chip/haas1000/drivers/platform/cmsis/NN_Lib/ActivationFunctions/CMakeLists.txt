cmake_minimum_required (VERSION 3.6)

project(CMSISNNActivation)

include(configLib)

file(GLOB SRC "./*_*.c")

add_library(CMSISNNActivation STATIC ${SRC})

configLib(CMSISNNActivation ${ROOT})
configDsp(CMSISNNActivation ${ROOT})

### Includes
target_include_directories(CMSISNNActivation PUBLIC "${NN}/Include")
target_include_directories(CMSISNNActivation PUBLIC "${ROOT}/CMSIS/DSP/Include")



