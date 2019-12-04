NAME := yloop_yts_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := YTS testcase for yloop
$(NAME)_COMPONENTS += yloop

$(NAME)_SOURCES += yloop_yts_test.c

$(NAME)_CFLAGS += -Wall -Werror

