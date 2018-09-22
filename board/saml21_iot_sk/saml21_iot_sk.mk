NAME := saml2x_iot_sk


$(NAME)_TYPE := kernel
HOST_ARCH            := Cortex-M0
HOST_MCU_FAMILY      := ATSAML21
HOST_MCU_VARIANT     := saml21b
HOST_MCU_PART_NUMBER := saml21j18b
SUPPORT_BINS         := no

#$(NAME)_SOURCES += aos/board_partition.c \
#                   aos/soc_init.c
                   
$(NAME)_SOURCES += Src/driver_init.c \
                   Src/stdio_start.c \
                   board.c

$(NAME)_COMPONENTS := saml21_iot_sk/IOT_AT3080_drv

GLOBAL_INCLUDES += aos Src/Config src
				   
GLOBAL_DEFINES += __SAML21J18B__ MX_DEBUG

DEFAULT_LINK_SCRIPT := Device_Startup/saml21j18b_flash$(LINK_SCRIPT_SUFFIX)


sal ?= 1
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
else
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


EXTRA_TARGET_MAKEFILES +=  board/saml21_iot_sk/saml21_download.mk
