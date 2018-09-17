NAME := fatfs

$(NAME)_TYPE        := kernel

$(NAME)_SOURCES     := fatfs.c
$(NAME)_SOURCES     += diskio.c
$(NAME)_SOURCES     += ff/ff.c
$(NAME)_SOURCES     += ff/ffunicode.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_INCLUDES     += include ff/include
GLOBAL_DEFINES      += AOS_FATFS
