NAME := armv5

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += ./gcc

$(NAME)_CFLAGS := -marm -mthumb-interwork

$(NAME)_SOURCES	 += gcc/port_c.c \
                    gcc/port_s.S \
                    panic/panic_c.c \
                    panic/panic_gcc.S

