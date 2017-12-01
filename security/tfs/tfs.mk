NAME := libtfs.a

$(NAME)_CFLAGS      += -Wall -Werror -Os

GLOBAL_INCLUDES     += include

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libtfs.a \
                            lib/$(HOST_ARCH)/libtfspal.a
