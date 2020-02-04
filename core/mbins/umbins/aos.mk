NAME := umbins

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := mbins for app

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_INCLUDES := ./ \
                    ../kmbins

$(NAME)_SOURCES := umbins_api.c \
                   umbins_entry.c

GLOBAL_LDFLAGS += -uapp_info

GLOBAL_DEFINES += AOS_BINS
