NAME := LITE-log

#$(NAME)_COMPONENTS := src
$(NAME)_INCLUDES :=  \
    ./ \
    

$(NAME)_SOURCES := \
    lite-log.c

# TODO: fix warnings
ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
$(NAME)_CFLAGS := $(filter-out -Werror,$(CFLAGS))
endif

$(NAME)_DEFINES += DEBUG
