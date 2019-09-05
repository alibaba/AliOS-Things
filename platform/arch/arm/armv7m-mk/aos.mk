NAME := arch_armv7m-mk

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := arch for armv7m

$(NAME)_SOURCES := common/panic_c.c
$(NAME)_SOURCES += common/port_c.c
$(NAME)_SOURCES += common/mpu.c
$(NAME)_SOURCES += common/k_bin.c

$(NAME)_INCLUDES += common/include

GLOBAL_INCLUDES += include

ifeq ($(COMPILER),armcc)
$(NAME)_SOURCES += common/panic_armcc.S
GLOBAL_ASMFLAGS += --cpreproc

ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += armcc/m3/port_s.S
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += armcc/m4/port_s.S
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_SOURCES += armcc/m7/port_s.S
endif

$(NAME)_PREBUILT_LIBRARY := lib/armcc/libarchmk.a

else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += common/panic_iccarm.S

ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += iccarm/m3/port_s.S
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += iccarm/m4/port_s.S
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_SOURCES += iccarm/m7/port_s.S
endif

$(NAME)_PREBUILT_LIBRARY := lib/iccarm/libarchmk.a
else
$(NAME)_SOURCES += common/panic_gcc.S

ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += gcc/m3/port_s.S
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += gcc/m4/port_s.S
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_SOURCES += gcc/m7/port_s.S
endif

$(NAME)_PREBUILT_LIBRARY := lib/gcc/libarchmk.a

endif

