EXTRA_POST_BUILD_TARGETS += gen_standard_images

CUSTOM_OTA := 1

ifeq ($(HW_MODULE),1)
GEN_BIN_OUTPUT_FILE:= haas1000_genbin.py 1
else
GEN_BIN_OUTPUT_FILE:= haas1000_genbin.py 0
endif

TARGET_OTA_FILE:=$($(HOST_MCU_FAMILY)_LOCATION)release/write_flash_gui/ota_bin/ota_rtos.bin
TARGET_FS_FILE:=$($(HOST_MCU_FAMILY)_LOCATION)release/write_flash_gui/ota_bin/littlefs.bin
OTA_BIN_OUTPUT_FILE:=$($(HOST_MCU_FAMILY)_LOCATION)release/write_flash_gui/ota_bin/ota_rtos_ota.bin
AOS_ROOT_DIR:=$(abspath .)

gen_standard_images:
	$(info gen signature and release image ...)
	$(info OUTPUT_DIR is $(OUTPUT_DIR))
	cd $($(HOST_MCU_FAMILY)_LOCATION)release/auto_build_tool/ && $(PYTHON) $(GEN_BIN_OUTPUT_FILE) $(OUTPUT_DIR) $(AOS_ROOT_DIR)
	$(QUIET)$(CP) $(TARGET_OTA_FILE) $(OTA_BIN_OUTPUT_FILE)
	echo "gen ota image ..."
	$(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(OTA_BIN_OUTPUT_FILE)
	$(PYTHON) $(SCRIPTS_PATH)/ota_gen_md5_bin.py $(OTA_BIN_OUTPUT_FILE) -m $(IMAGE_MAGIC)
	$(PYTHON) $(SCRIPTS_PATH)/ota_gen_md5_bin.py $(OTA_BIN_OUTPUT_FILE).xz -m $(IMAGE_MAGIC)
