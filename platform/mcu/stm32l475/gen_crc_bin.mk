PLATFORM_MCU_BOARD	:=$(subst .,/,$(HOST_MCU_FAMILY))

ifeq ($(HOST_OS),Win32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(PLATFORM_MCU_BOARD)/encrypt_win.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(PLATFORM_MCU_BOARD)/encrypt_linux"
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(PLATFORM_MCU_BOARD)/encrypt_linux"
else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(PLATFORM_MCU_BOARD)/encrypt_osx"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
OTA_BIN_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.ota$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += gen_crc_bin

gen_crc_bin:
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 0 0 0))
	$(QUIET)$(CP) $(CRC_BIN_OUTPUT_FILE) $(OTA_BIN_OUTPUT_FILE)
