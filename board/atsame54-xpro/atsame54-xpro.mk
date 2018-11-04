NAME := board_atsame54-xpro


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := atsamd5x_e5x
SUPPORT_BINS         := no
HOST_MCU_NAME        := ATSAME54P20A

$(NAME)_SOURCES += board.c
$(NAME)_SOURCES += soc_init.c

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += STDIO_UART=2

ywss_support ?= 1
sal ?= 0
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
else
module ?= wifi.wilc1000
$(NAME)_COMPONENTS += mcu/atsamd5x_e5x/Drivers/wilc1000
$(NAME)_COMPONENTS += protocols.net
GLOBAL_DEFINES += WITH_LWIP
endif


CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ATSAME54-XPRO
CONFIG_SYSINFO_DEVICE_NAME := ATSAME54-XPRO

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_DEFINES += SYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_DEFINES += SYSINFO_MCU=\"$(HOST_MCU_NAME)\"

# Define default component testcase set
ifneq (, $(findstring yts, $(BUILD_STRING)))
TEST_COMPONENTS += basic api wifi_hal rhino kv yloop alicrypto cjson digest_algorithm hashtable
endif
