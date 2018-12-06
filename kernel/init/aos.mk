NAME := kernel_init

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := kernel init unit

ifneq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := aos_init.c
endif

$(NAME)_COMPONENTS += log

GLOBAL_INCLUDES += include
