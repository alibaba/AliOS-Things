NAME := Lib_Bluetooth_Embedded_Dual_mode_Stack_for_$(BT_CHIP)$(BT_CHIP_REVISION)

BTE_PLATFORM_DIR := ../BTE_platform

GLOBAL_INCLUDES += . \
                   ../include

# Include BTE stack as component
#$(NAME)_COMPONENTS := libraries/bluetooth/BTE

BLUETOOTH_LIB_TYPE := dual_mode

ifneq ($(wildcard $(CURDIR)BTE_$(BLUETOOTH_LIB_TYPE).$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a),)
$(NAME)_PREBUILT_LIBRARY := BTE_$(BLUETOOTH_LIB_TYPE).$(HOST_ARCH).$(TOOLCHAIN_NAME).release.a
else
# Build from source (MXCHIP internal)
include $(CURDIR)$(BLUETOOTH_LIB_TYPE)_src.mk
endif


# Include appropriate firmware as component
$(NAME)_COMPONENTS += bluetooth/firmware

$(NAME)_SOURCES += $(BTE_PLATFORM_DIR)/mico_bt_bus.c \
                   $(BTE_PLATFORM_DIR)/mico_bt_hcd.c \
                   $(BTE_PLATFORM_DIR)/mico_bt_logmsg.c \
                   $(BTE_PLATFORM_DIR)/mico_bt_nvram_access.c \
                   $(BTE_PLATFORM_DIR)/mico_upio.c
                   
VALID_PLATFORMS := MK3238 MK3239
                   
