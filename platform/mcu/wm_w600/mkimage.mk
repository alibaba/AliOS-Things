
EXTRA_POST_BUILD_TARGETS += mkimage

SECBOOT_IMG = $($(HOST_MCU_FAMILY)_LOCATION)/drivers/tools/secboot.img
VERSION_TXT = $($(HOST_MCU_FAMILY)_LOCATION)/drivers/tools/version.txt
TOOLS_DIR = $($(HOST_MCU_FAMILY)_LOCATION)/drivers/tools

# application mode or not
ifneq ($(APPDIR),)
    SOURCE_BIN = $(APPDIR)/out/$(CLEANED_BUILD_STRING)/binary/$(CLEANED_BUILD_STRING).bin
    OUT_DIR = $(APPDIR)/out/$(CLEANED_BUILD_STRING)/binary/
else
    SOURCE_BIN = $(SOURCE_ROOT)out/$(CLEANED_BUILD_STRING)/binary/$(CLEANED_BUILD_STRING).bin
    OUT_DIR = $(SOURCE_ROOT)out/$(CLEANED_BUILD_STRING)/binary/
endif

mkimage:
	$(OBJDUMP) -S $(OUT_DIR)$(CLEANED_BUILD_STRING).elf > $(OUT_DIR)System.asm	
	$(TOOLS_DIR)/makeimg $(SOURCE_BIN) $(OUT_DIR)WM_W600.img 0 0 $(VERSION_TXT) E000
	$(TOOLS_DIR)/makeimg $(SOURCE_BIN) $(OUT_DIR)WM_W600_SEC.img 0 0 $(VERSION_TXT) 7E800
	$(TOOLS_DIR)/makeimg_all $(SECBOOT_IMG) $(OUT_DIR)WM_W600.img $(OUT_DIR)WM_W600.FLS

