
LIB_SRC := arpping.c
LIB_SRC += common.c
LIB_SRC += dhcpd.c
LIB_SRC += files.c
LIB_SRC += leases.c
LIB_SRC += packet.c
LIB_SRC += read.c
LIB_SRC += socket.c
LIB_SRC += static_leases.c
LIB_SRC += time.c
LIB_ASRC :=
LIBRARY_NAME := udhcpd
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
LOCAL_INC += -I$(TOPDIR)/components/drv/$(CHIP_NAME)
LOCAL_INC += -I$(TOPDIR)/components/netstack_wrapper
LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/$(LWIP_PATH)/src/include
LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/$(LWIP_PATH)/src/include/ipv4
LOCAL_INC += -I$(TOPDIR)/components/net/tcpip/$(LWIP_PATH)/ports/icomm/include
RELEASE_SRC := 2
$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
