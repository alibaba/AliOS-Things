NAME := arch_armv8m-mk

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := arch for armv8m

$(NAME)_SOURCES += common/port_c.c
$(NAME)_SOURCES += common/k_proc.c
$(NAME)_SOURCES += common/mpu.c
$(NAME)_SOURCES += common/svc_debug.c
$(NAME)_SOURCES += common/k_asid.c
$(NAME)_SOURCES += common/iomap.c

GLOBAL_INCLUDES += include common/include

ifeq ($(COMPILER),armcc)
$(error The $(COMPILER) compiler is not currently supported)
else ifeq ($(COMPILER),iar)
$(error The $(COMPILER) compiler is not currently supported)
else
$(NAME)_SOURCES += common/svc_gcc.S

ifeq ($(HOST_ARCH),Cortex-M33)
$(NAME)_SOURCES += gcc/m33/port_s.S
endif

$(NAME)_PREBUILT_LIBRARY += debug/debug.a
endif
