NAME := jffs2

$(NAME)_TYPE        := kernel

$(NAME)_SOURCES     +=  vfs_jffs2.c \
                        src/build.c \
                        src/compat-crc32.c \
                        src/compr.c \
                        src/compr_rtime.c \
                        src/compr_zlib.c \
                        src/debug.c \
                        src/dir-alios.c \
                        src/erase.c \
                        src/fs-alios.c \
                        src/flashio.c \
                        src/gc.c \
                        src/malloc-alios.c \
                        src/nodelist.c \
                        src/nodemgmt.c \
                        src/read.c \
                        src/readinode.c \
                        src/scan.c \
                        src/write.c \
                        src/rbtree.c

$(NAME)_CFLAGS      +=  -Wno-pointer-sign -Wno-unused-variable -Wno-unused-function

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_INCLUDES     += include
$(NAME)_DEFINES      += __ECOS

ifeq ($(HOST_ARCH), linux)
$(NAME)_DEFINES     += __LINUX
endif

GLOBAL_INCLUDES     += .
GLOBAL_DEFINES      += AOS_JFFS2
