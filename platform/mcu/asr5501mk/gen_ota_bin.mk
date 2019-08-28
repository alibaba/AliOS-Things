EXTRA_POST_BUILD_TARGETS += gen_standard_images
CUSTOM_OTA := 1
gen_standard_images:
	$(PYTHON) platform/mcu/asr5501mk/ota.py $(BIN_OUTPUT_FILE)
	$(PYTHON) $(SCRIPTS_PATH)/ota_gen_md5_bin.py $(OTA_BIN_OUTPUT_FILE) -m $(IMAGE_MAGIC)