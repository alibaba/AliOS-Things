NAME := kv_test

$(NAME)_COMPONENTS  += kernel.fs.kv

$(NAME)_SOURCES     += kv_test.c

$(NAME)_CFLAGS      += -Wall -Werror

