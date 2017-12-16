NAME := testcase

GLOBAL_INCLUDES     += include

$(NAME)_COMPONENTS  := yunit

$(NAME)_SOURCES     := yts_main.c

$(NAME)_CFLAGS      += -Wall -Werror

