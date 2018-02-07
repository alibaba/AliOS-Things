NAME := udhcp

$(NAME)_TYPE := kernel

$(NAME)_SOURCES := usr_dhcpd.c \
					arpping.c \
					files.c \
					leases.c \
					serverpacket.c \
					options.c \
					socket.c \
					packet.c \
					dhcp_time.c \
					dns_server.c

VER := 0.9.8
$(NAME)_CFLAGS += -DVERSION='"$(VER)"' -DXR_DHCPD
$(NAME)_CFLAGS += -DDHCPD_TIMEALT \
                  -DDHCPD_LWIP \
                  -DDHCPD_HEAP_REPLACE_STACK \
                  -DDHCPD_USRCFG \
                  -DDHCPD_FREERTOS \
                  -DDHCPD_ICMPPING \
                  -DDHCPD_LOW_LEVEL \
                  -DDHCPD_DNS
