NAME := epoll

$(NAME)_COMPONENTS := vfs lib_rbtree

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := epoll implement

$(NAME)_CFLAGS      += -Wall -Werror

$(NAME)_SOURCES     := src/epoll.c src/device.c src/rbtree_wrapper.c

$(NAME)_INCLUDES     += include

