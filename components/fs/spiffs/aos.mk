NAME := spiffs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := SPIFFS filesystem

$(NAME)_SOURCES += spiffs/spiffs_cache.c \
                   spiffs/spiffs_check.c \
                   spiffs/spiffs_gc.c \
                   spiffs/spiffs_hydrogen.c \
                   spiffs/spiffs_nucleus.c

ifeq ($(AOS_COMP_VFS), y)
$(NAME)_SOURCES += spiffs_vfs.c
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_INCLUDES += include spiffs/include
GLOBAL_DEFINES  += AOS_COMP_SPIFFS
