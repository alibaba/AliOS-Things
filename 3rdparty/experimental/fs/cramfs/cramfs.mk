NAME := cramfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := compress ROM filesystem

$(NAME)_SOURCES += src/cramfs.c \
                   src/uncompress.c \
                   src/flashio.c \
                   vfs_cramfs.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_INCLUDES   += include
$(NAME)_COMPONENTS += zlib

GLOBAL_INCLUDES += .
GLOBAL_DEFINES  += AOS_CRAMFS
