NAME := armv5

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += .

$(NAME)_CFLAGS := -marm -mthumb-interwork

$(NAME)_SOURCES	 += gcc/port_c.c \
                    gcc/port_s.S

