
LIB_SRC := intc.c systick.c proftick.c backtrace.c sys_clock.c exception_info.c
LIB_SRC += sys_random.c
ifeq ($(strip $(XIP_MODE)), 1)
LIB_SRC += xip.c flash.c sys_boot.c
endif

LIB_ASRC :=
LIBRARY_NAME := srm_sys
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC += -I$(TOPDIR)/components/drv
LOCAL_INC += -I$(TOPDIR)/components/drv/wdt

RELEASE_SRC := 1

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
