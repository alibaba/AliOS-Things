NAME := uart_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for uart

$(NAME)_SOURCES += uart_test.c ymodem.c

$(NAME)_CFLAGS += -Wall -Werror

