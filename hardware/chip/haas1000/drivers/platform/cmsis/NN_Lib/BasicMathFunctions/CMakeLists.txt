cmake_minimum_required (VERSION 3.6)

project(CMSISNNBasicMaths)

include(configLib)

file(GLOB SRC "./*_*.c")

add_library(CMSISNNBasicMaths STATIC ${SRC})

configLib(CMSISNNBasicMaths ${ROOT})
configDsp(CMSISNNBasicMaths ${ROOT})

### Includes
target_include_directories(CMSISNNBasicMaths PUBLIC "${NN}/Include")
target_include_directories(CMSISNNBasicMaths PUBLIC "${ROOT}/CMSIS/DSP/Include")



