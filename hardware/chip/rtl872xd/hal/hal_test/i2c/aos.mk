NAME := i2c_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for i2c

$(NAME)_SOURCES += i2c_test.c

$(NAME)_CFLAGS += -Wall -Werror

