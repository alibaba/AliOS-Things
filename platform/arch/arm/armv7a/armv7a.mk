NAME := armv7a

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
$(NAME)_SOURCES := gcc/a5/port_s.S
$(NAME)_SOURCES += gcc/a5/port_c.c
GLOBAL_INCLUDES += gcc/a5/

endif
