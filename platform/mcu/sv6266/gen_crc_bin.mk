EXTRA_POST_BUILD_TARGETS += gen_crc_bin
OTA_BIN_OUTPUT_FILE := $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINTYPE_LOWER)$(BIN_OUTPUT_SUFFIX)
$(info hello)
$(info $(OTA_BIN_OUTPUT_FILE))
$(info $(OUTPUT_DIR))

$(info python $(PYTHON))
gen_crc_bin:
	cd $(OUTPUT_DIR)/binary/; \
	/usr/bin/python ../../../$(SCRIPTS_PATH)/ota_gen_md5_bin.py $(OTA_BIN_OUTPUT_FILE);
