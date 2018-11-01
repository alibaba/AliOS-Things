NAME := halwifi

$(NAME)_TYPE := kernel

$(NAME)_SOURCES := wifi.c

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

GLOBAL_DEFINES += AOS_HAL_WIFI
