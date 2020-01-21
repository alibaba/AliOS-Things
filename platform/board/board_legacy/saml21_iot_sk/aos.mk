NAME := board_saml21_iot_sk

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board saml21_iot_sk
MODULE             := 1062
HOST_ARCH          := Cortex-M0
HOST_MCU_FAMILY    := mcu_atsaml21
SUPPORT_MBINS      := no
HOST_MCU_NAME      := saml21b
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init netmgr

GLOBAL_DEFINES += __SAML21J18B__ MX_DEBUG

$(NAME)_SOURCES += config/k_config.c \
                   config/partition_conf.c \
                   startup/board.c   \
                   startup/startup.c \
                   startup/startup_saml21.c

#IOT_AT3080_drv
$(NAME)_SOURCES += drivers/IOT_AT3080_drv/AliCloud_SDS/alicloud_sds.c \
                   drivers/IOT_AT3080_drv/drv_button/io_button.c \
                   drivers/IOT_AT3080_drv/drv_color_led/color_led.c \
                   drivers/IOT_AT3080_drv/drv_color_led/hsb2rgb.c \
                   drivers/IOT_AT3080_drv/drv_sht20/sht2x.c \
                   drivers/IOT_AT3080_drv/drv_VGM128064/oled.c \
                   drivers/IOT_AT3080_drv/helper/emhost/ATCmdParser/ATCmdParser.c \
                   drivers/IOT_AT3080_drv/helper/emhost/emh_alisds.c \
                   drivers/IOT_AT3080_drv/helper/emhost/emh_arg.c \
                   drivers/IOT_AT3080_drv/helper/emhost/emh_module.c \
                   drivers/IOT_AT3080_drv/helper/emhost/emh_wlan.c \
                   drivers/IOT_AT3080_drv/helper/hal/mx_i2c.c \
                   drivers/IOT_AT3080_drv/helper/hal/mx_serial.c \
                   drivers/IOT_AT3080_drv/helper/hal/mx_stdio.c \
                   drivers/IOT_AT3080_drv/helper/hal/mx_tick.c \
                   drivers/IOT_AT3080_drv/helper/jsmn/jsmn.c \
                   drivers/IOT_AT3080_drv/helper/jsmn/json_escape_str.c \
                   drivers/IOT_AT3080_drv/helper/jsmn/json_generator.c \
                   drivers/IOT_AT3080_drv/helper/jsmn/json_utils.c \
                   drivers/IOT_AT3080_drv/helper/jsmn/json_wrappers.c \
                   drivers/IOT_AT3080_drv/drv_board.c \
                   drivers/IOT_AT3080_drv/drv_board_test.c

$(NAME)_INCLUDES += drivers/IOT_AT3080_drv/helper/jsmn

GLOBAL_INCLUDES += drivers/IOT_AT3080_drv/AliCloud_SDS \
                   drivers/IOT_AT3080_drv/drv_button \
                   drivers/IOT_AT3080_drv/drv_color_led \
                   drivers/IOT_AT3080_drv/drv_sht20 \
                   drivers/IOT_AT3080_drv/drv_VGM128064 \
                   drivers/IOT_AT3080_drv/helper/emhost/ATCmdParser \
                   drivers/IOT_AT3080_drv/helper/emhost \
                   drivers/IOT_AT3080_drv/helper/hal \
                   drivers/IOT_AT3080_drv/helper \
                   drivers/IOT_AT3080_drv

GLOBAL_LDFLAGS  += -T platform/board/board_legacy/saml21_iot_sk/saml21j18b_flash.ld

GLOBAL_INCLUDES += config

ifneq (y,$(strip $(BSP_SUPPORT_EXTERNAL_MODULE)))
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_SAML21_IOT_SK
CONFIG_SYSINFO_DEVICE_NAME := saml2x_iot_sk

GLOBAL_DEFINES += SYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_DEFINES += SYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_DEFINES += SYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_DEFINES += SYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_DEFINES += SYSINFO_MCU=\"ATSAML21J18B\"
GLOBAL_DEFINES += SYSINFO_BOARD=\"$(MODULE)\"


EXTRA_TARGET_MAKEFILES +=  platform/board/board_legacy/saml21_iot_sk/saml21_download.mk

