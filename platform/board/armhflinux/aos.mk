NAME := board_armhflinux

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board armhflinux

MODULE              := 1062
HOST_ARCH           := armhflinux
HOST_MCU_FAMILY     := mcu_linuximpl
SUPPORT_MBINS       := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

GLOBAL_CFLAGS  += -I$($(NAME)_LOCATION)/include
GLOBAL_LDFLAGS += -L$($(NAME)_LOCATION)/lib
GLOBAL_DEFINES += LINUX_WIFI_MESH_IF_NAME=\"mon0\" RHINO_CONFIG_CPU_NUM=1


CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RASP
CONFIG_SYSINFO_DEVICE_NAME   := RASP

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

ifneq ($(ipv6),1)
GLOBAL_DEFINES += LWIP_IPV6=0
endif
CONFIG_AOS_MESH_TAPIF := 1

CONFIG_LIB_TFS            := y
CONFIG_TFS_ID2_RSA        := y
CONFIG_TFS_ID2_3DES       := n
CONFIG_TFS_EMULATE        := n
CONFIG_TFS_ON_LINE        := n
CONFIG_TFS_OPENSSL        := n
CONFIG_TFS_MBEDTLS        := n
CONFIG_TFS_DEBUG          := n
CONFIG_TFS_VENDOR_FACTORY := csky
CONFIG_TFS_VENDOR_VERSION := tee
CONFIG_TFS_SE_LIB         := libtfshal.a
CONFIG_TFS_TEE            := n
CONFIG_TFS_SW             := y
CONFIG_TFS_TEST           := n

GLOBAL_CFLAGS   += -std=gnu99
GLOBAL_INCLUDES += .
