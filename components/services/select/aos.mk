NAME := select

$(NAME)_COMPONENTS := vfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Multiplexing IO (epoll/select/poll) implement

$(NAME)_SOURCES     := poll/poll.c \
                    select/select.c

GLOBAL_INCLUDES     += include

