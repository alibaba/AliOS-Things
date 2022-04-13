NAME := uspace_aos

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY :=

ifeq ($(HOST_ARCH),Cortex-M33)
	$(NAME)_PREBUILT_LIBRARY += lib/cortex-m33/uspace_aos.a
else ifeq ($(HOST_ARCH),Cortex-A9)
	$(NAME)_PREBUILT_LIBRARY += lib/cortex-a9/uspace_aos.a
endif

GLOBAL_INCLUDES += ./include
