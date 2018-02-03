NAME := armv6m

ifeq ($(COMPILER),armcc)
$(NAME)_SOURCES := armcc/m0/port_c.c
$(NAME)_SOURCES += armcc/m0/port_s.S
GLOBAL_INCLUDES += armcc/m0/
else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES := iccarm/m0/port_c.c
$(NAME)_SOURCES += iccarm/m0/port_s.S
GLOBAL_INCLUDES += iccarm/m0/
else
$(NAME)_SOURCES := gcc/m0/port_c.c
$(NAME)_SOURCES += gcc/m0/port_s.S
GLOBAL_INCLUDES += gcc/m0/
endif
