NAME := kernel_init

ifneq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES    := aos_init.c
endif

ifeq ($(MBINS),app)
ifneq ($(ENABLE_USPACE),1)
$(NAME)_COMPONENTS += kernel.init.mbins_entry
endif
endif

$(NAME)_COMPONENTS += log

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

GLOBAL_INCLUDES += include
