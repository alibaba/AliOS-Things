NAME := spiffs

$(NAME)_MBINS_TYPE        := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES     +=  spiffs_port.c \
                        spiffs/spiffs_cache.c \
                        spiffs/spiffs_check.c \
                        spiffs/spiffs_gc.c \
                        spiffs/spiffs_hydrogen.c \
                        spiffs/spiffs_nucleus.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_INCLUDES     += include spiffs/include
GLOBAL_DEFINES      += AOS_SPIFFS
