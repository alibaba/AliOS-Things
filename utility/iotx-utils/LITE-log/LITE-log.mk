NAME := LITE-log

#$(NAME)_COMPONENTS := src
$(NAME)_INCLUDES :=  \
    ./ \
    

$(NAME)_SOURCES := \
    lite-log.c

# TODO: fix warnings
$(NAME)_CFLAGS := $(filter-out -Werror,$(CFLAGS))

$(NAME)_DEFINES += DEBUG
