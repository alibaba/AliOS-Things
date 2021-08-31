cmake_minimum_required (VERSION 3.6)

project(CMSISNNSoftmax)

include(configLib)

file(GLOB SRC "./*_*.c")

add_library(CMSISNNSoftmax STATIC ${SRC})

configLib(CMSISNNSoftmax ${ROOT})
configDsp(CMSISNNSoftmax ${ROOT})

### Includes
target_include_directories(CMSISNNSoftmax PUBLIC "${NN}/Include")
target_include_directories(CMSISNNSoftmax PUBLIC "${ROOT}/CMSIS/DSP/Include")



