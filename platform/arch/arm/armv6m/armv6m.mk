NAME := armv6m

$(NAME)_SOURCES := panic/panic_c.c

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpreproc
$(NAME)_SOURCES += panic/panic_armcc.S
$(NAME)_SOURCES += armcc/m0/port_c.c
$(NAME)_SOURCES += armcc/m0/port_s.S
GLOBAL_INCLUDES += armcc/m0/
else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += panic/panic_iccarm.S
$(NAME)_SOURCES += iccarm/m0/port_c.c
$(NAME)_SOURCES += iccarm/m0/port_s.S
GLOBAL_INCLUDES += iccarm/m0/
else
$(NAME)_SOURCES += panic/panic_gcc.S
$(NAME)_SOURCES += gcc/m0/port_c.c
$(NAME)_SOURCES += gcc/m0/port_s.S
GLOBAL_INCLUDES += gcc/m0/
endif
