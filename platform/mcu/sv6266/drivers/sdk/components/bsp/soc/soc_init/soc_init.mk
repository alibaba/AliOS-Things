NAME := soc_init

$(NAME)_CFLAGS += -Wno-implicit-function-declaration
$(NAME)_INCLUDES += ../../../drv

$(NAME)_SOURCES := init_mem.c soc_init.c
