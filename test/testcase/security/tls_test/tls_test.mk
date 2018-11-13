NAME := tls_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS  += mbedtls 

$(NAME)_SOURCES     += tls_test.c

$(NAME)_CFLAGS      += -Wall -Werror

