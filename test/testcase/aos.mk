NAME := testcase

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase framework

GLOBAL_INCLUDES += include

$(NAME)_COMPONENTS := yunit auto_component

$(NAME)_SOURCES := yts_main.c

$(NAME)_CFLAGS += -Wall -Werror

