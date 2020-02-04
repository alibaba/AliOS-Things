NAME := cramfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := compress ROM filesystem

$(NAME)_SOURCES += src/cramfs.c \
                   src/uncompress.c \
                   src/flashio.c

ifeq ($(AOS_COMP_VFS), y)
$(NAME)_SOURCES += cramfs_vfs.c
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_INCLUDES   += include
$(NAME)_COMPONENTS += zlib

GLOBAL_DEFINES  += AOS_COMP_CRAMFS
