NAME := board_linuxhost

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := configuration for board linuxhost

MODULE          := 1062
HOST_ARCH       := linux
HOST_MCU_FAMILY := mcu_linuximpl
SUPPORT_MBINS   := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

# Link Security Config
CONFIG_LS_DEBUG      := y
CONFIG_LS_ID2_OTP    := y
CONFIG_LS_KM_SE      := n
CONFIG_LS_KM_TEE     := n

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LINUXHOST
CONFIG_SYSINFO_DEVICE_NAME   := LINUXHOST
GLOBAL_CFLAGS                += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS                += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_ASMFLAGS += -m32
GLOBAL_CFLAGS   += -m32  -std=gnu99
GLOBAL_LDFLAGS  += -m32

GLOBAL_INCLUDES += .

# Define the default component testcase set
TEST_COMPONENTS += basic_test aos_test cjson_test
TEST_COMPONENTS += rhino_test kv_test fatfs_test
TEST_COMPONENTS += netmgr_test wifi_hal_test
TEST_COMPONENTS += rbtree_test

MESHLOWPOWER := 1
