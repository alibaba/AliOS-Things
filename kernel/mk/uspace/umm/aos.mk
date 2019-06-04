NAME := umm

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := memory management unit

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_SOURCES := pool/mm_pool.c
$(NAME)_SOURCES += umm.c

GLOBAL_INCLUDES += .

