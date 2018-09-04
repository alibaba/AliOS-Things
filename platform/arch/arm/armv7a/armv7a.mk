NAME := armv7a

$(NAME)_SOURCES := panic/panic_c.c

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_SOURCES := gcc/a5/port_s.S
$(NAME)_SOURCES += gcc/a5/port_c.c
GLOBAL_INCLUDES += gcc/a5/
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_SOURCES := gcc/a7/port_s.S
$(NAME)_SOURCES += gcc/a7/port_c.c
GLOBAL_INCLUDES += gcc/a7/

else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_SOURCES := gcc/a9/port_s.S
$(NAME)_SOURCES += gcc/a9/port_c.c
GLOBAL_INCLUDES += gcc/a9/
endif
endif
