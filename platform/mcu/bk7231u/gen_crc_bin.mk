
ifeq ($(HOST_OS),Win32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_win.exe"
WIFI_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/wifi.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_linux"
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_linux"
WIFI_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/wifi_linux"
else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_osx"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

ota1_offset = 34816 # 0x8800
ota2_offset = 1044480 # 0xFF000

CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
CRC_BIN_OUTPUT_FILE_XIP2 :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.xip2_crc$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += gen_crc_bin gen_standard_images
gen_crc_bin:
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 0 0 0))
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE_XIP2) 0 0 0 0))


gen_standard_images:
	@echo $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_OUTPUT_FILE_XIP2) $(ota2_offset)
	$(eval OUT_MSG := $(shell $(WIFI_TOOL) $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_OUTPUT_FILE_XIP2) $(ota2_offset)))
	$(call CPDIR,image_all.bin,$(OUTPUT_DIR)/binary)
	$(call CPDIR,ota_appliances.bin,$(OUTPUT_DIR)/binary)
	$(call RMDIR,image_all.bin)
	$(call RMDIR,ota_appliances.bin)
