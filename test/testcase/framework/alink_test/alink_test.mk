NAME := alink_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS  += middleware.alink.protocol middleware.alink.ywss

$(NAME)_SOURCES     += alink_test.c

$(NAME)_CFLAGS      += -Wall -Werror

