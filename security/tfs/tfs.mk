NAME := libtfs.a

$(NAME)_CFLAGS      += -Wall -Werror -Os

GLOBAL_INCLUDES     += include

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libtfs.a
$(NAME)_PREBUILT_LIBRARY += ../libid2/lib/$(HOST_ARCH)/libid2.a
$(NAME)_PREBUILT_LIBRARY += ../libkm/lib/$(HOST_ARCH)/libkm.a
