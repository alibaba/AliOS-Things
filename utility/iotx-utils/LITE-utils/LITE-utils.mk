NAME := LITE-utils

#$(NAME)_COMPONENTS := src
$(NAME)_INCLUDES :=  \
    ./ \
    ../LITE-log
    

$(NAME)_SOURCES := \
    json_parser.c \
    json_token.c \
    mem_stats.c \
    string_utils.c
    
$(NAME)_COMPONENTS += iotx-utils.LITE-log

# TODO: fix warnings
$(NAME)_CFLAGS := $(filter-out -Werror,$(CFLAGS))

$(NAME)_DEFINES += DEBUG
