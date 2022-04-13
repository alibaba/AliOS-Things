NAME := uservice

ifeq (y,$(AOS_COMP_USERVICE_IN_SERVICE))
$(NAME)_MBINS_TYPE := share
else
$(NAME)_MBINS_TYPE := kernel
endif

$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := uservice

$(NAME)_SOURCES-y := src/event.c \
                     src/event_svr.c \
                     src/rpc.c \
                     src/uservice.c \
                     src/utask.c \
                     src/select.c \

GLOBAL_INCLUDES-y += include
