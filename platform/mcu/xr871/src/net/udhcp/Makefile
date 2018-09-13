#
# Rules for building library
#

# ----------------------------------------------------------------------------
# common rules
# ----------------------------------------------------------------------------
ROOT_PATH := ../../..

include $(ROOT_PATH)/gcc.mk

# ----------------------------------------------------------------------------
# library and objects
# ----------------------------------------------------------------------------
LIBS := libudhcp.a

OBJS := usr_dhcpd.o \
	arpping.o \
	files.o \
	leases.o \
	serverpacket.o \
	options.o \
	socket.o \
	packet.o \
	dhcp_time.o

# extra flags
VER := 0.9.8
CC_FLAGS += -DVERSION='"$(VER)"' -DXR_DHCPD
CC_FLAGS += -DDHCPD_TIMEALT \
            -DDHCPD_LWIP \
            -DDHCPD_HEAP_REPLACE_STACK \
            -DDHCPD_USRCFG \
            -DDHCPD_FREERTOS \
            -DDHCPD_ICMPPING \
            -DDHCPD_LOW_LEVEL

# library make rules
include $(LIB_MAKE_RULES)
