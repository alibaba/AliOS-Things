NAME := arch_armv6m-mk

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := arch for armv6m

$(NAME)_SOURCES := common/panic_c.c
$(NAME)_SOURCES += common/port_c.c
$(NAME)_SOURCES += common/mpu.c
$(NAME)_SOURCES += common/k_bin.c

$(NAME)_INCLUDES += common/include

GLOBAL_INCLUDES += include

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpreproc
$(NAME)_SOURCES += common/panic_armcc.S
$(NAME)_SOURCES += armcc/m0/port_s.S
$(NAME)_PREBUILT_LIBRARY := lib/armcc/libarchmk.a
else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += common/panic_iccarm.S
$(NAME)_SOURCES += iccarm/m0/port_s.S
$(NAME)_PREBUILT_LIBRARY := lib/iccarm/libarchmk.a
else
$(NAME)_SOURCES += common/panic_gcc.S
$(NAME)_SOURCES += gcc/m0/port_s.S
$(NAME)_PREBUILT_LIBRARY := lib/gcc/libarchmk.a
endif

