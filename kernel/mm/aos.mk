NAME := mm

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := memory management unit

$(NAME)_CFLAGS += -Wall -Werror


ifeq ($(ENABLE_USPACE),1)

$(NAME)_SOURCES := tlsf/tlsf.c

GLOBAL_INCLUDES += tlsf/ ./include
endif

