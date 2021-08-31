cmake_minimum_required (VERSION 3.6)

project(CMSISNNReshape)

include(configLib)
include(configDsp)


file(GLOB SRC "./*_*.c")

add_library(CMSISNNReshape STATIC ${SRC})

configLib(CMSISNNReshape ${ROOT})
configDsp(CMSISNNReshape ${ROOT})

### Includes
target_include_directories(CMSISNNReshape PUBLIC "${NN}/Include")
target_include_directories(CMSISNNReshape PUBLIC "${ROOT}/CMSIS/DSP/Include")

