
LIB_SRC := cli.c
LIB_SRC += cli_cmd.c
LIB_SRC += atcmd.c
ifeq ($(strip $(WAC_EN)), 1)
LIB_SRC += atcmd_homekit.c
endif
LIB_SRC += atcmd_tcpip.c
ifeq ($(strip $(SSL_EN)), 1)
LIB_SRC += atcmd_ssl.c
endif
ifeq ($(strip $(SUPPORT_PARTITION_CFG_TABLE)), 1)
LIB_SRC += atcmd_cfg.c
endif

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
LOCAL_INC += -Icomponents/drv/wdt
LOCAL_INC += -Icomponents/drv/dma
LOCAL_INC += -Icomponents/iotapi
LOCAL_INC += -Icomponents/tools/atcmd
LOCAL_INC += -Icomponents/sys
ifeq ($(strip $(OTA_EN)), 1)
LOCAL_INC += -Icomponents/tools/ota_api
endif
LOCAL_INC += -Icomponents/netstack_wrapper
LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/src/include
#LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/src/include/netif
LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/ports/icomm/include
LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/src/include/ipv4
ifeq ($(strip $(SMARTCONFIG_EN)), 1)
LOCAL_INC += -Icomponents/third_party
endif
ifeq ($(strip $(SUPPORT_PARTITION_CFG_TABLE)), 1)
LOCAL_INC += -I$(PROJ_DIR)/src/cfg
endif

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
