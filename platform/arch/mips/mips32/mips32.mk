NAME := mips32

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
$(NAME)_SOURCES := gcc/port_c.c
$(NAME)_SOURCES += gcc/port_s.S
GLOBAL_INCLUDES += gcc/
endif
