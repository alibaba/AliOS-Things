NAME := alink_test

$(NAME)_COMPONENTS  += protocol.alink ywss

$(NAME)_SOURCES     += alink_test.c

$(NAME)_CFLAGS      += -Wall -Werror

