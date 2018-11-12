NAME := umbins

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS :=

ifneq ($(ENABLE_USPACE),1)

$(NAME)_INCLUDES := ./ \
		../../../kernel/mbins/kmbins

$(NAME)_SOURCES := umbins_api.c

endif

GLOBAL_DEFINES += AOS_BINS
