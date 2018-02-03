NAME := armv7m

ifeq ($(COMPILER),armcc)
$(NAME)_SOURCES := armcc/m4/port_c.c
$(NAME)_SOURCES += armcc/m4/port_s.S
GLOBAL_INCLUDES += armcc/m4/
else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES := iccarm/m4/port_c.c
$(NAME)_SOURCES += iccarm/m4/port_s.S
GLOBAL_INCLUDES += iccarm/m4/
else
$(NAME)_SOURCES := gcc/m4/port_c.c
$(NAME)_SOURCES += gcc/m4/port_s.S
GLOBAL_INCLUDES += gcc/m4/
endif
