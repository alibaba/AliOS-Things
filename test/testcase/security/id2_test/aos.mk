NAME := id2_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS  += id2 irot

$(NAME)_SOURCES     += id2_test.c

$(NAME)_CFLAGS      += -Wall -Werror -Wno-pointer-sign

