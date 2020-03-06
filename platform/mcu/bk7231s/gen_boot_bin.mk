
ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
EXTRA_POST_BUILD_TARGETS += gen_boot_images


FIRST_BOOT_OFFSET      := 0x0
SECOND_BOOT_OFFSET     := 0x20F0
GEN_BIN_OUTPUT_FILE:= $(SCRIPTS_PATH)/gen_common_bin_output_file.py

BOOT_FIRST_FILE := platform/mcu/bk7231s/bootloader_bk7231s_low.bin

BOOT_SECOND_FILE := $(OUTPUT_DIR)/binary/*.2ndboot.bin

BOOT_OUTPUT_FILE := $(OUTPUT_DIR)/binary/boot_bk7231s_all.bin

gen_boot_images:
	$(QUIT)$(RM) -f $(BOOT_OUTPUT_FILE)
	$(PYTHON) $(GEN_BIN_OUTPUT_FILE) -o $(BOOT_OUTPUT_FILE) -f $(FIRST_BOOT_OFFSET) $(BOOT_FIRST_FILE)
	$(PYTHON) $(GEN_BIN_OUTPUT_FILE) -o $(BOOT_OUTPUT_FILE) -f $(SECOND_BOOT_OFFSET) $(BOOT_SECOND_FILE)

endif
