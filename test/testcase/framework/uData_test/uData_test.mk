NAME := uData_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS  += middleware.common middleware.udata

$(NAME)_SOURCES     += uData_test.c

$(NAME)_CFLAGS      += -Wall -Werror

