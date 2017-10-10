NAME := libkm

$(NAME)_CFLAGS      += -Wall -Werror -Os

GLOBAL_INCLUDES     += include

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libkm.a
