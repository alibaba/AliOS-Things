
LIB_SRC := efuse_api.c

LIB_ASRC :=
LIBRARY_NAME := efuseapi
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC += -I$(TOPDIR)/components/drv

RELEASE_SRC := 1

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
