NAME := spiffs_test

$(NAME)_COMPONENTS  += 3rdparty.experimental.fs.spiffs

$(NAME)_SOURCES     += spiffs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

