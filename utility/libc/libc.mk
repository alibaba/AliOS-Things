NAME := newlib_stub

ifeq ($(COMPILER),armcc)
GLOBAL_INCLUDES += compilers/armlibc 
else 
ifeq ($(COMPILER),EWARM)
GLOBAL_INCLUDES += compilers/EWARM 
else
#default gcc 
ifneq ($(HOST_MCU_FAMILY),linux)
$(NAME)_TYPE := share
$(NAME)_SOURCES := newlib_stub.c
endif
endif
endif

