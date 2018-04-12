NAME := spiffs_test

$(NAME)_COMPONENTS  += 3rdparty.experimental.spiffs

$(NAME)_SOURCES     += spiffs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

