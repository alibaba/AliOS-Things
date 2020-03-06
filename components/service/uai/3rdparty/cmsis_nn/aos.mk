NAME := cmsis_nn
$(NAME)_TYPE := third_party

GLOBAL_INCLUDES    +=   \
                        CMSIS_5/CMSIS/Core/Include \
                        CMSIS_5/CMSIS/DSP/Include \
                        CMSIS_5/CMSIS/NN/Include

ifeq ($(CONFIG_UAI_SUPPORT_ODLA), 1)
$(NAME)_SOURCES    += \
                        src/PoolingFunctions/arm_pool_q7_HWC_nonsquare.c \
                        src/ConvolutionFunctions/arm_convolve_1x1_HWC_q7_fast_nonsquare_uai.c \
                        src/ConvolutionFunctions/arm_convolve_HWC_q7_RGB_uai.c \
                        src/ConvolutionFunctions/arm_convolve_HWC_q7_fast_uai.c \
                        src/ConvolutionFunctions/arm_convolve_HWC_q7_basic_uai.c \
                        src/ConvolutionFunctions/arm_convolve_HWC_q7_basic_nonsquare_uai.c \
                        src/ConvolutionFunctions/arm_convolve_HWC_q7_fast_nonsquare_uai.c \
                        src/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15_reordered_uai.c \
                        src/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15_uai.c \
                        src/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7_uai.c \
                        src/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7_nonsquare_uai.c \
                        src/FullyConnectedFunctions/arm_fully_connected_q7_uai.c
endif

include $(SOURCE_ROOT)/components/service/uai/3rdparty/cmsis_nn/source.mk
