NAME := spiffs

$(NAME)_TYPE        := kernel

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
