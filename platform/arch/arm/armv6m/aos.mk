NAME := arch_armv6m

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := arch for armv6m

$(NAME)_SOURCES := common/panic_c.c
$(NAME)_SOURCES += common/port_c.c
$(NAME)_SOURCES += common/backtrace.c

GLOBAL_INCLUDES += common/

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpreproc
$(NAME)_SOURCES += common/panic_armcc.S
$(NAME)_SOURCES += armcc/m0/port_s.S
else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += common/panic_iccarm.S
$(NAME)_SOURCES += iccarm/m0/port_s.S
else
$(NAME)_SOURCES += common/panic_gcc.S
$(NAME)_SOURCES += gcc/m0/port_s.S
endif
