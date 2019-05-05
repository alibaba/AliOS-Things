EXTRA_POST_BUILD_TARGETS += gen_crc_bin
OTA_BIN_OUTPUT_FILE := $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINTYPE_LOWER)$(BIN_OUTPUT_SUFFIX)

ifeq ($(HOST_OS),Win32)
	SETENV = set
else
	SETENV = export
endif

gen_crc_bin:
	cd $(OUTPUT_DIR)/binary/ && \
	$(OBJCOPY) --only-section .text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.text.bin && \
	$(OBJCOPY) --only-section .data -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.data.bin && \
    $(OBJCOPY) --only-section .rodata -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.rodata.bin && \
    $(OBJCOPY) --only-section .irom0.text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.irom0text.bin && \
	$(SETENV) ESP8266_NM=${NM} && \
    python ../../../platform/mcu/esp8266/tools/gen_appbin.py $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) 2 1 15 5 && \
	$(CP) eagle.app.flash.bin $(OTA_BIN_OUTPUT_FILE) && \
	$(RM) eagle.app.flash.bin eagle.app.v6.text.bin eagle.app.v6.data.bin eagle.app.v6.rodata.bin eagle.app.v6.irom0text.bin
