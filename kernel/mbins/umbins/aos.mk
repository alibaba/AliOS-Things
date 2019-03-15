NAME := umbins

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := mbins for app

$(NAME)_CFLAGS += -Wall -Werror


ifneq ($(ENABLE_USPACE),1)

$(NAME)_INCLUDES := ./ \
                    ../kmbins

$(NAME)_SOURCES := umbins_api.c \
                   umbins_entry.c

GLOBAL_LDFLAGS += -uapp_info
endif

GLOBAL_DEFINES += AOS_BINS
