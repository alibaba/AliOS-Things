NAME := ali_lib

$(NAME)_MBINS_TYPE := kernel

ifeq ($(HOST_ARCH), Cortex-M4)
LIB_DIR := cortex-m4
else
$(error "not find correct platform!")
endif

GLOBAL_INCLUDES += include

$(NAME)_COMPONENTS := protocols.bluetooth digest_algorithm

$(NAME)_PREBUILT_LIBRARY := lib/$(LIB_DIR)/ali_lib.a

