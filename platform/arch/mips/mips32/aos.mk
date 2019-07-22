NAME := arch_mips32

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := arch for mips32

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
$(NAME)_SOURCES := gcc/port_c.c
$(NAME)_SOURCES += gcc/port_s.S

GLOBAL_INCLUDES += include
endif
