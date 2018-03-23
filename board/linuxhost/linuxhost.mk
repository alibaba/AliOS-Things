
NAME := board_linuxhost

MODULE              := 1062
HOST_ARCH           := linux
HOST_MCU_FAMILY     := linux
SUPPORT_BINS        := no


# Change to use the correct deivce here if necessary.
CONFIG_LIB_TFS := y
CONFIG_TFS_ID2_RSA := y
CONFIG_TFS_ID2_3DES := n
CONFIG_TFS_EMULATE := n
CONFIG_TFS_ON_LINE := n
CONFIG_TFS_OPENSSL := n
CONFIG_TFS_MBEDTLS := n
CONFIG_TFS_ALICRYPTO := n
CONFIG_TFS_DEBUG := n
CONFIG_TFS_TEE := n
CONFIG_TFS_SW := y
CONFIG_TFS_TEST := n

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LINUXHOST
CONFIG_SYSINFO_DEVICE_NAME := LINUXHOST
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_ASMFLAGS += -m32
GLOBAL_CFLAGS += -m32  -std=gnu99
GLOBAL_LDFLAGS += -m32

GLOBAL_INCLUDES += .

sal ?= 0

ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
endif

# Define the default component testcase set
TEST_COMPONENTS += basic api cjson digest_algorithm hashtable
TEST_COMPONENTS += rhino vfs yloop kv deviceIO vcall fatfs
TEST_COMPONENTS += alink mqtt fota netmgr wifi_hal alicrypto tfs tls
