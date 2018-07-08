NAME := ali_lib

$(NAME)_MBINS_TYPE := kernel

GLOBAL_INCLUDES += include

$(NAME)_COMPONENTS := protocols.bluetooth digest_algorithm

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/gcc/ali_lib.a

