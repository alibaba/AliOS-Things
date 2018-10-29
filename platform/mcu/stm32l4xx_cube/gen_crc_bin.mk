OTA_BIN_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=$(BIN_OUTPUT_SUFFIX))
EXTRA_POST_BUILD_TARGETS += gen_crc_bin
gen_crc_bin:
	python $(SCRIPTS_PATH)/st_mcu_bin_pack_tool.py 0x80000 0x4000 $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/l496_bootloader.bin $(OTA_BIN_OUTPUT_FILE)
