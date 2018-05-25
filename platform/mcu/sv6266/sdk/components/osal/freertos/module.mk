


LIB_SRC := \
	kernel/Source/list.c \
	kernel/Source/event_groups.c \
	kernel/Source/list.c \
	kernel/Source/queue.c \
	kernel/Source/tasks.c \
	kernel/Source/timers.c \
	kernel/Source/timers_ext.c

LIB_ASRC :=

LIBRARY_NAME := freertos
LOCAL_CFLAGS +=
LOCAL_AFLAGS +=
LOCAL_INC +=
LOCAL_INC += -Icomponents/bsp/soc/lowpower


ifeq ($(strip $(MCU_DEF)), ANDES_N10)
LIB_ASRC   += nds32/os_cpu_a.S
LIB_SRC    += nds32/port.c  nds32/portISR.c
else
$(error please define MCU_DEF)
endif

RELEASE_SRC := 1
$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))

