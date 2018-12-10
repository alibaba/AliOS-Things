NAME := board_atsame54-xpro


$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board atsame54-xpro

MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := mcu_atsame54
SUPPORT_MBINS        := no
HOST_MCU_NAME        := ATSAME54P20A

$(NAME)_SOURCES += board.c

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += STDIO_UART=2

ywss_support       ?= 1
AOS_NETWORK_SAL    ?= y
ifeq (y,$(AOS_NETWORK_SAL))
$(NAME)_COMPONENTS += sal netmgr
module             ?= wifi.mk3060
else
GLOBAL_DEFINES     += CONFIG_NO_TCPIP
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ATSAME54-XPRO
CONFIG_SYSINFO_DEVICE_NAME   := ATSAME54

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Define default component testcase set
ifneq (, $(findstring yts, $(BUILD_STRING)))
TEST_COMPONENTS += basic api wifi_hal rhino kv yloop alicrypto cjson
endif
