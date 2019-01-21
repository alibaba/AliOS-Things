
LIB_SRC := wifi_api.c

LIB_ASRC :=
LIBRARY_NAME := wifiapi
ifeq ($(strip $(OS)), freertos)
LOCAL_CFLAGS := -DFREERTOS
endif
ifeq ($(strip $(OS)), rhino)
LOCAL_CFLAGS := -DRHINO
endif
LOCAL_AFLAGS :=
LOCAL_INC += -I$(TOPDIR)/components/inc
LOCAL_INC += -I$(TOPDIR)/components/inc/crypto
LOCAL_INC += -I$(TOPDIR)/components/inc/bsp/$(CHIP_NAME)
LOCAL_INC += -I$(TOPDIR)/components/inc/bsp
LOCAL_INC += -I$(TOPDIR)/components/drv
LOCAL_INC += -I$(TOPDIR)/components/drv/$(CHIP_NAME)
LOCAL_INC += -I$(TOPDIR)/components/softmac
LOCAL_INC += -I$(TOPDIR)/components/crypto
LOCAL_INC += -I$(TOPDIR)/components/netstack_wrapper

RELEASE_SRC := 1

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
