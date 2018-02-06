NAME := spiffs_test

$(NAME)_COMPONENTS  += modules.fs.spiffs

$(NAME)_SOURCES     += spiffs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

