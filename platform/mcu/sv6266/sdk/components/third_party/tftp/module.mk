LIB_SRC := packets.c
LIB_SRC += common.c
LIB_SRC += pong.c
LIB_SRC += tftp.c

LIB_ASRC :=
LIBRARY_NAME := tftp
LOCAL_CFLAGS += -Wno-address
LOCAL_AFLAGS += 
LOCAL_INC += -Icomponents/third_party/tftp
LOCAL_INC += -Icomponents/netstack_wrapper
LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/src/include
LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/ports/icomm/include
LOCAL_INC += -Icomponents/net/tcpip/lwip-1.4.0/src/include/ipv4

RELEASE_SRC := 2

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
