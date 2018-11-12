NAME := mm

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS :=

ifeq ($(ENABLE_USPACE),1)

$(NAME)_SOURCES := tlsf/tlsf.c

GLOBAL_INCLUDES += tlsf/
endif

