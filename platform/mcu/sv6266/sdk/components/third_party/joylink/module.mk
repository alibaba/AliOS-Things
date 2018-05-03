LIB_SRC := 
LIB_SRC += tiny_aes/tiny_aes.c
LIB_SRC += joylink_main.c
LIB_SRC += joylink_auth_aes.c
LIB_SRC += joylink_smnt.c

LIB_ASRC :=
LIBRARY_NAME := joylink
LOCAL_CFLAGS += -Wno-address -DUSING_TINY_AES
#LOCAL_CFLAGS += -Wno-address
LOCAL_AFLAGS +=
LOCAL_INC := -I$(TOPDIR)/components/wifi_pkg_cap
LOCAL_INC += -I$(TOPDIR)/components/third_party/joylink
LOCAL_INC += -I$(TOPDIR)/components/third_party/joylink/tiny_aes
LOCAL_INC += -I$(TOPDIR)/components/drv
LOCAL_INC += -I$(TOPDIR)/components/softmac
LOCAL_INC += -I$(TOPDIR)/components/iotapi
RELEASE_SRC := 2
$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
