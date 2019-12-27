NAME := yaffs2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := Yet Another Flash File System

$(NAME)_SOURCES += yaffs_alios.c yaffs_install_drv.c

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
$(NAME)_DEFINES  += __LINUX
endif

GLOBAL_DEFINES  += AOS_COMP_YAFFS2
