
LIB_SRC := atcmd_icomm.c test_bus.c  test_timer.c
LIB_SRC += sysconf_api.c

LIB_ASRC :=
LIBRARY_NAME := atcmd
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC += -Icomponents/inc
LOCAL_INC += -Icomponents/drv
LOCAL_INC += -Icomponents/softmac
LOCAL_INC += -Icomponents/drv/timer
LOCAL_INC += -Icomponents/drv/flash
LOCAL_INC += -Icomponents/drv/flashctl
LOCAL_INC += -Icomponents/netstack_wrapper
LOCAL_INC += -Icomponents/net/tcpip/$(LWIP_PATH)/src/include
#LOCAL_INC += -Icomponents/net/tcpip/$(LWIP_PATH)/src/include/netif
LOCAL_INC += -Icomponents/net/tcpip/$(LWIP_PATH)/ports/icomm/include
LOCAL_INC += -Icomponents/net/tcpip/$(LWIP_PATH)/src/include/ipv4
LOCAL_INC += -I$(PROJ_DIR)/src/cfg
LOCAL_INC += -I$(PROJ_DIR)/src/cli

RELEASE_SRC := 1

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
