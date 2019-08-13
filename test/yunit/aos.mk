NAME := yunit

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := yunit framework

$(NAME)_SOURCES += yts_main.c yunit.c

$(NAME)_CFLAGS += -Wall -Werror

