NAME := board_esp8266

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := configuration for board esp8266
MODULE             := 1062
HOST_ARCH          := xtensa
HOST_MCU_FAMILY    := mcu_esp8266
SUPPORT_MBINS      := no

# todo: remove these after rhino/lwip ready
osal ?= rhino

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ESP8266
CONFIG_SYSINFO_DEVICE_NAME   := ESP8266

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

#for activation handle
GLOBAL_CFLAGS += -DBOARD_ESP8266

GLOBAL_INCLUDES += .
$(NAME)_SOURCES := config/partition_conf.c startup/startup.c startup/board.c

GLOBAL_DEFINES += LOCAL_PORT_ENHANCED_RAND WITH_VENDOR_LWIP

ifeq ($(APP), yts)
GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=4096
endif

ifeq ($(osal),freertos)

else
$(NAME)_SOURCES   += config/k_config.c
endif

GLOBAL_INCLUDES += ./config/ ./drivers/
