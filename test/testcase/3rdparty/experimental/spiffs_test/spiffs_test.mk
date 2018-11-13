NAME := spiffs_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS  += 3rdparty.experimental.fs.spiffs

$(NAME)_SOURCES     += spiffs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

