NAME := alink_test

$(NAME)_COMPONENTS  += middleware.alink.protocol middleware.alink.ywss

$(NAME)_SOURCES     += alink_test.c

$(NAME)_CFLAGS      += -Wall -Werror

