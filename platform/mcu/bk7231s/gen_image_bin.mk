ifeq ($(HOST_OS),Win32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_win.exe"
WIFI_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/wifi_win.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
WIFI_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/wifi_linux"
else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_osx"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

ota1_offset = 69632 # 0x11000
ota2_offset = 1044480 # 0xFF000

ALL_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_all$(BIN_OUTPUT_SUFFIX))
CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
CRC_BIN_OUTPUT_FILE_XIP2 :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.xip2_crc$(BIN_OUTPUT_SUFFIX))

ifeq ($(PING_PONG_OTA),1)
EXTRA_POST_BUILD_TARGETS += gen_standard_images gen_pingpong_images
else
EXTRA_POST_BUILD_TARGETS += gen_standard_images
endif

gen_standard_images:
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 0 0 0))
	$(QUIET)$(CP) $(CRC_BIN_OUTPUT_FILE) $(BIN_OUTPUT_FILE)

gen_pingpong_images:
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE_XIP2) 0 0 0 0))
	@echo $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_OUTPUT_FILE_XIP2) $(ota2_offset) 
	$(eval OUT_MSG := $(shell $(WIFI_TOOL) $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_OUTPUT_FILE_XIP2) $(ota2_offset)))
ifeq ($(PING_PONG_OTA),1)
	$(call CPDIR,image_all.bin,$(ALL_BIN_OUTPUT_FILE))
	$(info Generate PINGPONG OTA image: $(OTA_BIN_OUTPUT_FILE) ...)
	$(call CPDIR,ota_appliances.bin,$(OTA_BIN_OUTPUT_FILE))
	$(call RMDIR,image_all.bin)
	$(call RMDIR,ota_appliances.bin)
	$(QUIET)$(RM) $(OUTPUT_DIR)/binary/*.xip2*
endif
	$(QUIET)$(RM) -f $(CRC_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) -f $(CRC_BIN_OUTPUT_FILE_XIP2)

