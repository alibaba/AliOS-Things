
NAME := board_linuxhost

MODULE              := 1062
HOST_ARCH           := linux
HOST_MCU_FAMILY     := linux
SUPPORT_BINS        := no

$(NAME)_COMPONENTS  :=  tfs

# Change to use the correct deivce here if necessary.
ifeq ($(sal),1)
$(NAME)_COMPONENTS += sal.wifi.mk3060
endif

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

GLOBAL_CFLAGS += -m32  -std=gnu99
GLOBAL_LDFLAGS += -m32

GLOBAL_INCLUDES += .

TEST_COMPONENTS += basic api cjson digest_algorithm hashtable
TEST_COMPONENTS += rhino vfs yloop kv deviceIO vcall fatfs
TEST_COMPONENTS += alink mqtt fota netmgr wifi_hal alicrypto tfs tls
