NAME := tls_test

$(NAME)_COMPONENTS  += mbedtls 

$(NAME)_SOURCES     += tls_test.c

$(NAME)_CFLAGS      += -Wall -Werror

