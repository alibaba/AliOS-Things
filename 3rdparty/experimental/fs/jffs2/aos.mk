NAME := jffs2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := JFFS2 filesystem

$(NAME)_SOURCES += src/build.c \
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

ifeq ($(AOS_COMP_VFS), y)
$(NAME)_SOURCES += jffs2_vfs.c
endif

$(NAME)_CFLAGS += -Wno-pointer-sign -Wno-unused-variable -Wno-unused-function

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_INCLUDES += include
$(NAME)_DEFINES  += __ECOS

ifeq ($(HOST_ARCH), linux)
$(NAME)_DEFINES += __LINUX
endif

GLOBAL_DEFINES  += AOS_COMP_JFFS2
