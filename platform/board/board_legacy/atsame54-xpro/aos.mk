NAME := board_atsame54-xpro


$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board atsame54-xpro

MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := mcu_atsame54
SUPPORT_MBINS        := no
HOST_MCU_NAME        := ATSAME54P20A

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init network
$(NAME)_COMPONENTS-$(BSP_SUPPORT_EXTERNAL_MODULE) += external_module

$(NAME)_SOURCES += board.c

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += STDIO_UART=2

ywss_support       ?= 1

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ATSAME54-XPRO
CONFIG_SYSINFO_DEVICE_NAME   := ATSAME54-XPRO

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_DEFINES += SYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_DEFINES += SYSINFO_MCU=\"$(HOST_MCU_NAME)\"

# Define default component testcase set
ifneq (, $(findstring yts, $(BUILD_STRING)))
TEST_COMPONENTS += basic_test aos_test wifi_hal_test rhino_test kv_test cjson_test
endif
