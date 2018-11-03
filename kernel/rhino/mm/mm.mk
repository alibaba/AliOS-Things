NAME := mm

$(NAME)_MBINS_TYPE := app


$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS :=

ifeq ($(ENABLE_USPACE),1)

$(NAME)_SOURCES := tlsf/tlsf.c

GLOBAL_INCLUDES += tlsf/
endif

