NAME := kernel_init

ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES    := aos_uspace_init.c
else
$(NAME)_SOURCES    := aos_init.c
endif

ifeq ($(MBINS),app)
ifeq ($(ENABLE_USPACE),1)
$(NAME)_COMPONENTS += kernel.init.app_entry
else
$(NAME)_COMPONENTS += kernel.init.mbins_entry
endif
endif


$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

GLOBAL_INCLUDES += include
