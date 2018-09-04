NAME := umbins

$(NAME)_MBINS_TYPE := app

$(NAME)_INCLUDES := ./ \
		../../../kernel/mbins/kmbins

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS :=

$(NAME)_SOURCES := umbins_api.c

GLOBAL_DEFINES += AOS_BINS
