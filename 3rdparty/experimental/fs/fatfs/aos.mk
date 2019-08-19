NAME := fatfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := Fatfs filesystem

$(NAME)_SOURCES += diskio.c
$(NAME)_SOURCES += ff/ff.c
$(NAME)_SOURCES += ff/ffunicode.c

ifeq ($(AOS_COMP_VFS), y)
$(NAME)_SOURCES += fatfs_vfs.c
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_INCLUDES += include ff/include
GLOBAL_DEFINES  += AOS_COMP_FATFS
