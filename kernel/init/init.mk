NAME := kernel_init

$(NAME)_SOURCES    := aos_init.c

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

GLOBAL_INCLUDES += include
