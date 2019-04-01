LIB_SRC := sntp.c

LIB_ASRC :=
LIBRARY_NAME := sntp
LOCAL_CFLAGS += -Wno-address
LOCAL_AFLAGS +=
LOCAL_INC += -I$(TOPDIR)/components/third_party/sntp
RELEASE_SRC := 1
$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
