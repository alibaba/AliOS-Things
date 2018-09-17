EXTRA_POST_BUILD_TARGETS += gen_crc_bin
OTA_BIN_OUTPUT_FILE := $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINTYPE_LOWER)$(BIN_OUTPUT_SUFFIX)
gen_crc_bin:
	cd $(OUTPUT_DIR)/binary/; \
	$(OBJCOPY) --only-section .text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.text.bin; \
	$(OBJCOPY) --only-section .data -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.data.bin; \
    $(OBJCOPY) --only-section .rodata -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.rodata.bin; \
    $(OBJCOPY) --only-section .irom0.text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.irom0text.bin; \
	export ESP8266_NM=${NM}; \
    python2 ../../../platform/mcu/esp8266/tools/gen_appbin.py $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) 2 1 15 5; \
	unset ESP8266_NM; \
	mv eagle.app.flash.bin $(OTA_BIN_OUTPUT_FILE); \
	rm eagle.app.v6.text.bin eagle.app.v6.data.bin eagle.app.v6.rodata.bin eagle.app.v6.irom0text.bin; \
	python ../../../$(SCRIPTS_PATH)/ota_gen_md5_bin.py $(OTA_BIN_OUTPUT_FILE);
