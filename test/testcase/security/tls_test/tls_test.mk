NAME := tls_test

$(NAME)_COMPONENTS  += imbedtls

$(NAME)_SOURCES     += tls_test.c

$(NAME)_CFLAGS      += -Wall -Werror

