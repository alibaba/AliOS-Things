NAME := u_i2c_test

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := i2c test driver

$(NAME)_SOURCES := i2c_test.c
$(NAME)_SOURCES += si7006.c

#$(NAME)_CFLAGS += -Werror

GLOBAL_CFLAGS +=

GLOBAL_DEFINES +=

$(NAME)_INCLUDES +=
