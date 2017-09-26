NAME := armv7m

$(NAME)_SOURCES := gcc/m4/port_c.c
$(NAME)_SOURCES += gcc/m4/port_s.S

GLOBAL_INCLUDES += gcc/m4/
