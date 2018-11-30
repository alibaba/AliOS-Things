NAME := riscy

$(NAME)_SOURCES := gcc/port_s.S
$(NAME)_SOURCES += gcc/port_c.c
GLOBAL_INCLUDES += gcc/
