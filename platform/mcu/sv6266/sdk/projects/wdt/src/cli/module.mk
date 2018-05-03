
LIB_SRC := cli.c
LIB_SRC += cli_cmd.c

LIB_ASRC :=
LIBRARY_NAME := cli
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC += -Icomponents/inc
LOCAL_INC += -Icomponents/drv
LOCAL_INC += -Icomponents/softmac
LOCAL_INC += -Icomponents/drv/uart
LOCAL_INC += -Icomponents/drv/timer
LOCAL_INC += -Icomponents/drv/flash
LOCAL_INC += -Icomponents/drv/flashctl
LOCAL_INC += -Icomponents/drv/wdog
LOCAL_INC += -Icomponents/drv/dma
LOCAL_INC += -Icomponents/netstack_wrapper
LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/src/include
#LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/src/include/netif
LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/ports/icomm/include
LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/src/include/ipv4

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
