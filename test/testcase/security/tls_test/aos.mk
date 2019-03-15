NAME := tls_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS  += imbedtls

$(NAME)_SOURCES     += tls_test.c

$(NAME)_CFLAGS      += -Wall -Werror

