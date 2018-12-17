NAME := arch_armv7m-svc

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1.0
$(NAME)_SUMMARY    := arch for armv7m

ifeq ($(ENABLE_USPACE),1)

$(NAME)_SOURCES := panic/panic_c.c
$(NAME)_SOURCES += common/arch_mpu.c
$(NAME)_SOURCES += common/mpu.c
$(NAME)_SOURCES += common/svc_debug.c
GLOBAL_INCLUDES += common

ifeq ($(COMPILER),armcc)
$(NAME)_SOURCES += panic/panic_armcc.S
GLOBAL_ASMFLAGS += --cpreproc

ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += armcc/m3/port_c.c
$(NAME)_SOURCES += armcc/m3/port_s.S
GLOBAL_INCLUDES += armcc/m3/
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += armcc/m4/port_c.c
$(NAME)_SOURCES += armcc/m4/port_s.S
GLOBAL_INCLUDES += armcc/m4/
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_SOURCES += armcc/m7/port_c.c
$(NAME)_SOURCES += armcc/m7/port_s.S
GLOBAL_INCLUDES += armcc/m7/
endif

else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += panic/panic_iccarm.S

ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += iccarm/m3/port_c.c
$(NAME)_SOURCES += iccarm/m3/port_s.S
GLOBAL_INCLUDES += iccarm/m3/
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += iccarm/m4/port_c.c
$(NAME)_SOURCES += iccarm/m4/port_s.S
GLOBAL_INCLUDES += iccarm/m4/
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_SOURCES += iccarm/m7/port_c.c
$(NAME)_SOURCES += iccarm/m7/port_s.S
GLOBAL_INCLUDES += iccarm/m7/
endif

else
$(NAME)_SOURCES += panic/panic_gcc.S
$(NAME)_SOURCES += common/svc_gcc.S

ifeq ($(HOST_ARCH),Cortex-M3)
$(NAME)_SOURCES += gcc/m3/port_c.c
$(NAME)_SOURCES += gcc/m3/port_s.S
GLOBAL_INCLUDES += gcc/m3/
else ifeq ($(HOST_ARCH),Cortex-M4)
$(NAME)_SOURCES += gcc/m4/port_c.c
$(NAME)_SOURCES += gcc/m4/port_s.S
GLOBAL_INCLUDES += gcc/m4/
else ifeq ($(HOST_ARCH),Cortex-M7)
$(NAME)_SOURCES += gcc/m7/port_c.c
$(NAME)_SOURCES += gcc/m7/port_s.S
GLOBAL_INCLUDES += gcc/m7/
endif

endif

endif # ENABLE_USPACE

