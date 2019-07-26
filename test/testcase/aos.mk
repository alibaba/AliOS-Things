NAME := testcase

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase framework

$(NAME)_COMPONENTS := auto_component

$(NAME)_SOURCES := yunit.c yts_main.c

$(NAME)_CFLAGS += -Wall -Werror

