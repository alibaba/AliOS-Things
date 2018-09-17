NAME := cramfs

$(NAME)_TYPE        := kernel

$(NAME)_SOURCES     +=  src/cramfs.c \
                        src/uncompress.c \
                        src/flashio.c \
                        vfs_cramfs.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_INCLUDES    += include
$(NAME)_COMPONENTS  += zlib

GLOBAL_INCLUDES     += .
GLOBAL_DEFINES      += AOS_CRAMFS
