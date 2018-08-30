NAME := armv7a

ifeq ($(COMPILER),armcc)

$(NAME)_SOURCES := armcc/a5/port_s.S
$(NAME)_SOURCES += armcc/a5/port_c.c
GLOBAL_INCLUDES += armcc/a5/

else ifeq ($(COMPILER),rvct)

$(NAME)_SOURCES := armcc/a5/port_s.S
$(NAME)_SOURCES += armcc/a5/port_c.c
GLOBAL_INCLUDES += armcc/a5/

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
endif
endif
