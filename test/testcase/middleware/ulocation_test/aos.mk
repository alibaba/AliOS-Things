NAME := ulocation_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for ulocation
$(NAME)_COMPONENTS += ulocation cjson

$(NAME)_SOURCES += ulocation_test.c

$(NAME)_CFLAGS += -Wall -Werror