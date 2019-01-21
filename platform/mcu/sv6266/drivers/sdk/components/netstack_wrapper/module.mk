
LIB_SRC := netstack.c

LIB_ASRC :=
LIBRARY_NAME := netstack
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC := -Icomponents/bsp
LOCAL_INC += -Icomponents/inc
LOCAL_INC += -Icomponents/drv
LOCAL_INC += -Icomponents/softmac
LOCAL_INC += -Icomponents/inc/bsp/$(CHIP_NAME)
LOCAL_INC += -Icomponents/net/tcpip/$(LWIP_PATH)/src/include
LOCAL_INC += -Icomponents/net/tcpip/$(LWIP_PATH)/ports/icomm/include
LOCAL_INC += -Icomponents/net/tcpip/$(LWIP_PATH)/src/include/ipv4
LOCAL_INC += -Icomponents/net/udhcp

RELEASE_SRC := 1
$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
