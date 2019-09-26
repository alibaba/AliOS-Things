NAME := kernel_init

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := kernel init unit

$(NAME)_SOURCES := aos_init.c

$(NAME)_COMPONENTS += ulog

GLOBAL_INCLUDES += include
