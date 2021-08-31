ifneq ($(GEN_SDK_LIB),1)
ifeq ($(A7_DSP_ENABLE),1)
LIB_NAME := platform_a7
else
LIB_NAME := platform
endif
ifneq ($(SDK),1)

src_obj := drivers/

$(LIB_NAME)-y := $(src_obj)
endif

obj-y :=
obj-y += $(LIB_NAME).a
ifneq ($(DUAL_BOOT),1)
obj-y += cmsis/ hal/ main/
endif


else


cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libplatform
ifneq ($(SDK),1)

src_obj := cmsis/ drivers/ hal/

ifneq ($(DUAL_BOOT),1)
src_obj += main/
endif

$(LIB_NAME)-y := $(src_obj)
endif

obj-y := $(LIB_NAME).a
endif

subdir-ccflags-y += -Iplatform/cmsis/inc -Iplatform/hal -Iutils/hwtimer_list
