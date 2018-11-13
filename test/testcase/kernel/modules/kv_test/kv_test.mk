NAME := kv_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS  += kernel.fs.kv

$(NAME)_SOURCES     += kv_test.c

$(NAME)_CFLAGS      += -Wall -Werror

