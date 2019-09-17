NAME := lwip_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for lwip
$(NAME)_COMPONENTS += lwip

$(NAME)_SOURCES += lwip_test.c

$(NAME)_CFLAGS += -Wall -Werror

