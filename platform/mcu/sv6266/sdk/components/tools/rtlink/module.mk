LIB_SRC := 
LIB_SRC += rtlink_main.c

LIB_ASRC :=
LIBRARY_NAME := rtlink
LOCAL_CFLAGS += -Wno-address
#LOCAL_CFLAGS += -Wno-address
LOCAL_AFLAGS +=
LOCAL_INC := -I$(TOPDIR)/components/tools/rtlinklib
LOCAL_INC += -I$(TOPDIR)/components/tools/rtlink
LOCAL_INC += -I$(TOPDIR)/components/wifi_pkg_cap
LOCAL_INC += -I$(TOPDIR)/components/third_party/mbedtls/include
LOCAL_INC += -I$(TOPDIR)/components/drv
LOCAL_INC += -I$(TOPDIR)/components/softmac
LOCAL_INC += -I$(TOPDIR)/components/iotapi
RELEASE_SRC := 2
$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
