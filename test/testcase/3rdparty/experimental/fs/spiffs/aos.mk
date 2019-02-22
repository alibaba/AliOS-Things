NAME := spiffs_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := testcase for spiffs

$(NAME)_COMPONENTS += spiffs

$(NAME)_SOURCES += spiffs_test.c

$(NAME)_CFLAGS += -Wall -Werror

