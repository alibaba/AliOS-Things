LIB_SRC := init_mem.c
LIB_SRC += soc_init.c

LIB_ASRC :=
LIBRARY_NAME := soc_init
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=
LOCAL_INC +=
LOCAL_INC += -I$(TOPDIR)/components/drv

RELEASE_SRC := 2

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
