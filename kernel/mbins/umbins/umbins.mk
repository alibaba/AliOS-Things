NAME := umbins

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := mbins for app

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS :=

ifneq ($(ENABLE_USPACE),1)

$(NAME)_INCLUDES := ./ \
                    ../../../kernel/mbins/kmbins

$(NAME)_SOURCES := umbins_api.c \
                   umbins_entry.c

GLOBAL_LDFLAGS += -uapp_info
endif

GLOBAL_DEFINES += AOS_BINS
