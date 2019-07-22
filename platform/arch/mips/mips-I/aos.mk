NAME := arch_mips-I

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := arch for mips-I

$(NAME)_SOURCES := gcc/port_c.c
$(NAME)_SOURCES += gcc/port_s.S

GLOBAL_INCLUDES += include

