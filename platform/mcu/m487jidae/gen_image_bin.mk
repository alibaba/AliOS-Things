ifeq ($(HOST_OS),Win32)
SCAT := "$($(HOST_MCU_FAMILY)_LOCATION)/drivers/merge_fw/srec_cat.exe"
else #
$(error not surport for $(HOST_OS))
endif # Win32

CLONER_BIN_FILE := "$($(HOST_MCU_FAMILY)_LOCATION)/drivers/merge_fw/AOS_Bootloader_Cloner.bin"
LDROM_BIN_FILE  := "$($(HOST_MCU_FAMILY)_LOCATION)/drivers/merge_fw/AOS_Bootloader_LDROM.bin"

OTA_HEX_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_ota.hex)

CLONER_HEX_OUTPUT_FILE := $(OUTPUT_DIR)/binary/AOS_Bootloader_Cloner.hex
LDROM_HEX_OUTPUT_FILE :=$(OUTPUT_DIR)/binary/AOS_Bootloader_LDROM.hex
AOS_HEX_OUTPUT_FILE := $(OUTPUT_DIR)/binary/Merged_AOS.hex

EXTRA_POST_BUILD_TARGETS += gen_standard_images

gen_standard_images:
	$(QUIET)$(CP) $(BIN_OUTPUT_FILE) $(OTA_BIN_OUTPUT_FILE)
	$(SCAT) $(OTA_BIN_OUTPUT_FILE)  -binary -offset 0x0     -output $(OTA_MD5_HEX_OUTPUT_FILE) -Intel
	$(SCAT) $(CLONER_BIN_FILE)  -binary -offset 0x7c000     -output $(CLONER_HEX_OUTPUT_FILE) -Intel
	$(SCAT) $(LDROM_BIN_FILE)       -binary -offset 0x100000    -output $(LDROM_HEX_OUTPUT_FILE) -Intel
	echo "$(SCAT)   $(OTA_HEX_OUTPUT_FILE) -Intel $(CLONER_HEX_OUTPUT_FILE) -Intel $(LDROM_HEX_OUTPUT_FILE) -Intel -output $(AOS_HEX_OUTPUT_FILE) -Intel"
	$(SCAT) $(OTA_HEX_OUTPUT_FILE) -Intel $(CLONER_HEX_OUTPUT_FILE) -Intel $(LDROM_HEX_OUTPUT_FILE) -Intel -output $(AOS_HEX_OUTPUT_FILE) -Intel
