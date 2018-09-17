NAME := uData_test

$(NAME)_COMPONENTS  += middleware.common middle.udata

$(NAME)_SOURCES     += uData_test.c

$(NAME)_CFLAGS      += -Wall -Werror

