NAME := uai

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := uAI Framework

GLOBAL_LDFLAGS     += -lm

GLOBAL_INCLUDES    += include

$(NAME)_INCLUDES    += common function/include model/load nn/include

$(NAME)_COMPONENTS += ulog

$(NAME)_SOURCES  += function/activation/uai_relu.c               \
					function/activation/uai_sigmoid.c            \
					function/activation/uai_tanh.c               \
					function/connected/uai_fconn.c               \
					function/softmax/uai_softmax.c               \
					function/convolution/uai_conv.c              \
					function/convolution/uai_conv_depthwise.c    \
					function/pool/uai_pool.c                     \
					model/load/uai_load.c

$(NAME)_COMPONENTS-$(CONFIG_UAI_USE_CMSIS_NN) += cmsis_nn

ifeq ($(CONFIG_UAI_USE_CMSIS_NN), y)
GLOBAL_DEFINES     += 	UAI_USE_CMSIS_NN
$(NAME)_INCLUDES   +=   3rdparty/cmsis_nn/Core/Include   \
						3rdparty/cmsis_nn/DSP/Include    \
						3rdparty/cmsis_nn/NN/Include
endif

ifeq ($(dnn_enable), 1)
GLOBAL_DEFINES     += 	UAI_DNN
$(NAME)_SOURCES    += 	nn/dnn/uai_dnn.c
$(NAME)_INCLUDES   += 	nn/dnn/
endif

ifeq ($(cnn_enable), 1)
GLOBAL_DEFINES     += 	UAI_CNN
$(NAME)_SOURCES    += 	nn/cnn/uai_cnn.c
$(NAME)_INCLUDES   += 	nn/cnn/
endif

ifeq ($(HOST_ARCH), Cortex-M0)
GLOBAL_DEFINES += ARM_MATH_CM0
else ifeq ($(HOST_ARCH), Cortex-M3)
GLOBAL_DEFINES += ARM_MATH_CM3
else ifeq ($(HOST_ARCH), Cortex-M4)
GLOBAL_DEFINES += ARM_MATH_CM4
else ifeq ($(HOST_ARCH), Cortex-M7)
GLOBAL_DEFINES += ARM_MATH_CM7
endif
