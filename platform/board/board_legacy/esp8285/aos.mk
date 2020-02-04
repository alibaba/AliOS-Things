NAME := board_esp8285

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board esp8285
MODULE             := 1062
HOST_ARCH          := xtensa
HOST_MCU_FAMILY    := mcu_esp8266
SUPPORT_MBINS      := no
SUPPORT_ESP8285    := yes

# todo: remove these after rhino/lwip ready
osal ?= rhino

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ESP8266
CONFIG_SYSINFO_DEVICE_NAME   := ESP8266

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

#for activation handle
GLOBAL_CFLAGS += -DBOARD_ESP8266

GLOBAL_INCLUDES += . ./config/ ./drivers/
$(NAME)_SOURCES := config/partition_conf.c startup/startup.c startup/board.c
ifeq ($(osal),freertos)

else
$(NAME)_SOURCES   += config/k_config.c
endif

GLOBAL_DEFINES += BOARD_ESP8285 LOCAL_PORT_ENHANCED_RAND WITH_VENDOR_LWIP