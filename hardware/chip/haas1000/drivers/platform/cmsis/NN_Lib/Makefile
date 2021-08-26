cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

NN_LIB_NAME := libcmsis_nn

#ifneq ($(SDK),1)
obj-y :=
obj-y += ActivationFunctions/
# obj-y += BasicMathFunctions/
# obj-y += ConcatenationFunctions/
obj-y += ConvolutionFunctions/
obj-y += FullyConnectedFunctions/
obj-y += NNSupportFunctions/
# obj-y += PoolingFunctions/
# obj-y += ReshapeFunctions/
# obj-y += SoftmaxFunctions/
$(NN_LIB_NAME)-y := $(obj-y)
#endif

obj-y := $(NN_LIB_NAME).a

subdir-ccflags-y += -Iplatform/cmsis/inc -Iinclude/cmsis_nn
subdir-ccflags-y += -Iinclude/cmsis_dsp
