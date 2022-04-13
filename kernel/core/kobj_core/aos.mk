NAME := kobj_core

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := kernel object

GLOBAL_INCLUDES += include

ifeq ($(HOST_ARCH),Cortex-M33)
$(NAME)_PREBUILT_LIBRARY += lib/cortex-m33/kobj_core.a
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_PREBUILT_LIBRARY += lib/cortex-a9/kobj_core.a
endif
