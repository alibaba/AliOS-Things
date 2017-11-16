include $(MAKEFILES_PATH)/aos_partition.mk
include $(MAKEFILES_PATH)/aos_flash_download_app.mk

SFLASH_LOG_FILE      ?= $(BUILD_DIR)/sflash_writer.log
SFLASH_REDIRECT	= > $(SFLASH_LOG_FILE)

IMAGE_SIZE_SCRIPT  := $(MAKEFILES_PATH)/scripts/image_size.py

IMAGES_SECTORS_DEFAULT_COUNT	:= 1

APPS_HEADER_DEFINES :=
CURRENT_DEPENDENCY :=

OPENOCD_LOG_FILE ?= $(BUILD_DIR)/openocd_log.txt
DOWNLOAD_LOG := >> $(OPENOCD_LOG_FILE)

###############################################################################
# MACRO: BUILD_IMAGES_RULES

define BUILD_IMAGES_RULES
$(if $($(1)),$(eval $(1)_ENTRY_COUNT := 1),$(eval $(1)_ENTRY_COUNT := 0))
$(if $($(1)),$(eval $(1)_SIZE := $(shell $(PYTHON) $(IMAGE_SIZE_SCRIPT) $($(1)))),$(eval $(1)_SIZE := 0))
endef
### end of BUILD_APPS_RULES

###############################################################################

# MACRO: BUILD_IMAGE_DOWNLOAD_DEPENDENCY
define BUILD_IMAGE_DOWNLOAD_DEPENDENCY
$(if $($(1)),$(eval $(1)_DOWNLOAD_DEPENDENCY := $($(1)) $(CURRENT_DEPENDENCY) sflash_write_app display_map_summary $(EXT_IMAGES_DOWNLOAD_DEP) IMAGES_DOWNLOAD_RULES $(LINK_APPS_FILE)),)
$(if $($(1)),$(eval CURRENT_DEPENDENCY += $(1)_DOWNLOAD),)
endef
#### end of BUILD_IMAGE_DOWNLOAD_DEPENDENCY

IMAGES_DOWNLOADS_DEPENDENCY := 
IMAGES := WIFI_FIRMWARE BT_PATCH_FIRMWARE BOOTLOAD_FIRMWARE APPLICATION_FIRMWARE ATE_FIRMWARE PARAMETER_1_IMAGE PARAMETER_2_IMAGEFILESYSTEM_IMAGE
$(foreach IMAGE,$(IMAGES),$(eval $(if $($(IMAGE)), IMAGES_DOWNLOADS_DEPENDENCY += $(IMAGE)_DOWNLOAD)))
$(foreach IMAGE,$(IMAGES),$(eval $(call BUILD_IMAGE_DOWNLOAD_DEPENDENCY,$(IMAGE))))

.PHONY: FILESYSTEM_IMAGE_DOWNLOAD BT_PATCH_DOWNLOAD WIFI_FIRMWARE_DOWNLOAD EXT_IMAGE_DOWNLOAD

IMAGES_DOWNLOAD_RULES: $(APPS_HEADER_DEPENDENCY)
	$(foreach IMAGE,$(IMAGES),$(eval $(call BUILD_IMAGES_RULES,$(IMAGE), $($(IMAGE)_SECTOR_START))))

ifneq ($(WIFI_FIRMWARE),)
WIFI_FIRMWARE_DOWNLOAD: $(WIFI_FIRMWARE_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading WIFI_FIRMWARE to partition: $(WIFI_FIRMWARE_PARTITION_TCL) size: $(WIFI_FIRMWARE_SIZE) bytes... 
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_CFG_PATH)interface/$(JTAG).cfg -f $(OPENOCD_CFG_PATH)$(HOST_OPENOCD)/$(HOST_OPENOCD).cfg -f demos/sub_build/spi_flash_write/sflash_write.tcl -c "sflash_write_file $(WIFI_FIRMWARE) $(WIFI_FIRMWARE_PARTITION_TCL) 0x0 $(SFLASH_APP_PLATFROM_BUS) 0" -c shutdown $(DOWNLOAD_LOG) 2>&1 && $(ECHO) Download complete && $(ECHO_BLANK_LINE) || $(ECHO) Download failed. See $(OPENOCD_LOG_FILE) for detail.
endif

ifneq ($(BT_PATCH_FIRMWARE),)
BT_PATCH_FIRMWARE_DOWNLOAD: $(BT_PATCH_FIRMWARE_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading BT_PATCH to partition: $(BT_PATCH_FIRMWARE_PARTITION_TCL) size: $(BT_PATCH_FIRMWARE_SIZE) bytes ... 
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_CFG_PATH)interface/$(JTAG).cfg -f $(OPENOCD_CFG_PATH)$(HOST_OPENOCD)/$(HOST_OPENOCD).cfg -f demos/sub_build/spi_flash_write/sflash_write.tcl -c "sflash_write_file $(BT_PATCH_FIRMWARE) $(BT_PATCH_FIRMWARE_PARTITION_TCL) 0x0 $(SFLASH_APP_PLATFROM_BUS) 0" -c shutdown $(DOWNLOAD_LOG) 2>&1 && $(ECHO) Download complete && $(ECHO_BLANK_LINE) || $(ECHO) Download failed. See $(OPENOCD_LOG_FILE) for detail.
endif

ifneq ($(FILESYSTEM_IMAGE),)
FILESYSTEM_IMAGE_DOWNLOAD: $(FILESYSTEM_IMAGE_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading resources filesystem ... $(FILESYSTEM_IMAGE) at sector $(FILESYSTEM_IMAGE_SECTOR_START) size $(FILESYSTEM_IMAGE_SECTOR_COUNT)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_CFG_PATH)interface/$(JTAG).cfg -f $(OPENOCD_CFG_PATH)$(HOST_OPENOCD)/$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(FILESYSTEM_IMAGE) $(FILESYSTEM_IMAGE_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0" -c shutdown $(DOWNLOAD_LOG) 2>&1 && $(ECHO) Download complete && $(ECHO_BLANK_LINE) || $(ECHO) Download failed. See $(OPENOCD_LOG_FILE) for detail.
endif
	
EXT_IMAGE_DOWNLOAD: sflash_write_app  $(IMAGES_DOWNLOADS_DEPENDENCY) 
