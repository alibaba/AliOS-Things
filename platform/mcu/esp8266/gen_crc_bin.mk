EXTRA_POST_BUILD_TARGETS += gen_crc_bin
gen_crc_bin:
	cd $(OUTPUT_DIR)/binary/; \
	$(OBJCOPY) --only-section .text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.text.bin; \
	$(OBJCOPY) --only-section .data -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.data.bin; \
    $(OBJCOPY) --only-section .rodata -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.rodata.bin; \
    $(OBJCOPY) --only-section .irom0.text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.irom0text.bin; \
	export ESP8266_NM=${NM}; \
    python2 ../../../platform/mcu/esp8266/tools/gen_appbin.py $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) 2 1 15 5; \
	unset ESP8266_NM; \
	mv eagle.app.flash.bin $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)-0x1000$(BIN_OUTPUT_SUFFIX); \
	rm eagle.app.v6.text.bin eagle.app.v6.data.bin eagle.app.v6.rodata.bin eagle.app.v6.irom0text.bin
