LIB_SRC := 
LIB_SRC += airkiss_main.c

ifeq ($(strip $(AIRKISSAES_EN)), 1)
STATIC_LIB += $(MYDIR)/libairkiss_aes.a $(MYDIR)/libairkiss_aes_log.a
else
STATIC_LIB += $(MYDIR)/libairkiss.a $(MYDIR)/libairkiss_log.a
endif

LIB_ASRC :=
LIBRARY_NAME := airkiss
LOCAL_CFLAGS += -Wno-address
ifeq ($(strip $(AIRKISSAES_EN)), 1)
LOCAL_CFLAGS += -DAIRKISS_AES
endif
LOCAL_AFLAGS +=
LOCAL_INC := -I$(TOPDIR)/components/wifi_pkg_cap
LOCAL_INC += -I$(TOPDIR)/components/third_party/airkiss
LOCAL_INC += -I$(TOPDIR)/components/drv
LOCAL_INC += -I$(TOPDIR)/components/softmac
LOCAL_INC += -I$(TOPDIR)/components/iotapi

RELEASE_SRC := 2

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
