NAME := prov

DEBUG := no

GLOBAL_INCLUDES     += include

$(NAME)_CFLAGS      += -Wall -Werror -Os

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/prov.a

