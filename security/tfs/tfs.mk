NAME := libtfs.a

$(NAME)_CFLAGS      += -Wall -Werror -Os

GLOBAL_INCLUDES     += include

ifeq ($(HOST_ARCH), linux)
PLATFORM := linuxhost
else ifeq ($(HOST_ARCH), armhflinux)
PLATFORM := armhflinux
else ifeq ($(HOST_ARCH), ARM968E-S)
PLATFORM := mk3060
else ifeq ($(HOST_ARCH), Cortex-M4)
PLATFORM := b_l475e
else
$(error "not find correct platform!")
endif

$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libtfs.a
$(NAME)_PREBUILT_LIBRARY += ../libid2/lib/$(PLATFORM)/libid2.a
$(NAME)_PREBUILT_LIBRARY += ../libkm/lib/$(PLATFORM)/libkm.a
