#
# Component Makefile
#
COMPONENT_DIR = lwesp
COMPONENT_SUBMODULES += $(COMPONENT_DIR)

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src

INC_HAAS += $(addprefix $(MODULES_DIR)/$(COMPONENT_DIR)/, \
	src/include \
	port/micropython/include \
	port/stm32/include \
)
SRC_HAAS += $(addprefix modules/$(COMPONENT_DIR)/,\
    src/lwesp/lwesp.c \
    src/lwesp/lwesp_conn.c \
    src/lwesp/lwesp_evt.c \
    src/lwesp/lwesp_mdns.c \
    src/lwesp/lwesp_ping.c \
    src/lwesp/lwesp_sta.c \
    src/lwesp/lwesp_utils.c \
    src/lwesp/lwesp_ap.c \
    src/lwesp/lwesp_debug.c \
    src/lwesp/lwesp_hostname.c \
    src/lwesp/lwesp_mem.c \
    src/lwesp/lwesp_server.c \
    src/lwesp/lwesp_threads.c \
    src/lwesp/lwesp_webserver.c \
    src/lwesp/lwesp_buff.c \
    src/lwesp/lwesp_dhcp.c \
    src/lwesp/lwesp_input.c \
    src/lwesp/lwesp_parser.c \
    src/lwesp/lwesp_smart.c \
    src/lwesp/lwesp_timeout.c \
    src/lwesp/lwesp_wps.c \
    src/lwesp/lwesp_cli.c \
    src/lwesp/lwesp_dns.c \
    src/lwesp/lwesp_int.c \
    src/lwesp/lwesp_pbuf.c \
    src/lwesp/lwesp_sntp.c \
    src/lwesp/lwesp_unicode.c \
    src/api/lwesp_netconn.c \
    port/micropython/src/modusocket.c \
    port/micropython/src/modnetwork.c \
    port/stm32/src/lwesp_port_mem.c \
    port/stm32/src/lwesp_port_sys.c \
    port/stm32/src/lwesp_port_ll.c \
)

CFLAGS += -DMICROPY_PY_LWESP_WIFI=1
