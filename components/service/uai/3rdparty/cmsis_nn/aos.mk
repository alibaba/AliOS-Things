NAME := cmsis_nn
$(NAME)_TYPE := third_party

GLOBAL_INCLUDES    +=   \
                        CMSIS_5/CMSIS/Core/Include \
                        CMSIS_5/CMSIS/DSP/Include \
                        CMSIS_5/CMSIS/NN/Include

include $(SOURCE_ROOT)/components/uai/3rdparty/cmsis_nn/source.mk
