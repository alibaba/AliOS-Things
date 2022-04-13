ifneq ($(GEN_SDK_LIB_OS),1)
obj-y := common/

else

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libmbed
ifneq ($(SDK),1)

src_obj := common/

$(LIB_NAME)-y := $(src_obj)
endif

obj-y := $(LIB_NAME).a
endif

subdir-ccflags-y += -Iplatform/cmsis/inc -Iplatform/hal
