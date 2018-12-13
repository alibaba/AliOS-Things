SFLASH_LOG_FILE      ?= $(BUILD_DIR)/sflash_writer.log
SFLASH_REDIRECT	= > $(SFLASH_LOG_FILE)

SFLASH_APP_TARGET := sub_build.spi_flash_write@NoRTOS@$(PLATFORM)
SFLASH_APP_PLATFROM_BUS := $(PLATFORM)
SFLASH_PREBUILD_APP := board/$(PLATFORM)/flash_prog.elf

clean:
	$(QUIET)$(RM) -rf $(SFLASH_PREBUILD_APP)

# If Downloading is required, then the Serial Flash app need to be built
$(SFLASH_PREBUILD_APP):
	$(QUIET)$(ECHO) Building Flash Loader App...
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(SFLASH_APP_TARGET)  SFLASH= EXTERNAL_AOS_GLOBAL_DEFINES=$(EXTERNAL_AOS_GLOBAL_DEFINES) SUB_BUILD=sflash_app $(SFLASH_REDIRECT)
	$(QUIET)$(CP) -f $(BUILD_DIR)/$(SFLASH_APP_TARGET)/binary/$(SFLASH_APP_TARGET).elf   $(SFLASH_PREBUILD_APP)
	$(QUIET)$(ECHO) Finished Building Flash Loader App
	$(QUIET)$(ECHO_BLANK_LINE)

sflash_write_app: $(SFLASH_PREBUILD_APP)



