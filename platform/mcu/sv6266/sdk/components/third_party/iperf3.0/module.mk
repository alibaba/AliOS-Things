LIB_SRC := units.c
#LIB_SRC += cjson.c
LIB_SRC += timer.c
LIB_SRC += tcp_window_size.c
LIB_SRC += tcp_info.c
LIB_SRC += net.c
LIB_SRC += iperf_util.c
LIB_SRC += iperf_udp.c
LIB_SRC += iperf_tcp.c
LIB_SRC += iperf_server_api.c
LIB_SRC += iperf_locale.c
LIB_SRC += iperf_error.c
LIB_SRC += iperf_client_api.c
LIB_SRC += iperf_api.c
LIB_SRC += iperf_main.c
LIB_SRC += porting.c

LIB_ASRC :=
LIBRARY_NAME := iperf3
LOCAL_CFLAGS += -Wno-address
LOCAL_AFLAGS +=
LOCAL_INC += -I$(TOPDIR)/components/third_party/iperf3/inc
RELEASE_SRC := 2

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
