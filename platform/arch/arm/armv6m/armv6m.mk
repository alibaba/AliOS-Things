NAME := armv6m

$(NAME)_SOURCES := gcc/m0plus/port_c.c
$(NAME)_SOURCES += gcc/m0plus/port_s.S

GLOBAL_INCLUDES += gcc/m0plus/
