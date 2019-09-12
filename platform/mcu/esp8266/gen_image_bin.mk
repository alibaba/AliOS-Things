EXTRA_POST_BUILD_TARGETS += gen_standard_images
BIN_DIR := $(OUTPUT_DIR)/binary

ifeq ($(HOST_OS),Win32)
	SETENV = set
else
	SETENV = export
endif

# application mode or not
ifneq ($(APPDIR),)
	_AOS_PATH := $(AOS_SDK_PATH)
else
	_AOS_PATH := ../../..
endif

gen_standard_images:
	$(QUIET) $(ECHO) "Generate standard flash images $(LINK_OUTPUT_FILE)"
	$(QUIET) $(OBJCOPY) --only-section .text -O binary $(LINK_OUTPUT_FILE) $(BIN_DIR)/eagle.app.v6.text.bin
	$(QUIET) $(OBJCOPY) --only-section .data -O binary $(LINK_OUTPUT_FILE) $(BIN_DIR)/eagle.app.v6.data.bin
	$(QUIET) $(OBJCOPY) --only-section .rodata -O binary $(LINK_OUTPUT_FILE) $(BIN_DIR)/eagle.app.v6.rodata.bin
	$(QUIET) $(OBJCOPY) --only-section .irom0.text -O binary $(LINK_OUTPUT_FILE) $(BIN_DIR)/eagle.app.v6.irom0text.bin
	cd $(BIN_DIR) && $(SETENV) ESP8266_NM=${NM} && python $(_AOS_PATH)/platform/mcu/esp8266/tools/gen_appbin.py $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) 2 2 0 5 && cd -
	$(QUIET) $(CP) $(BIN_DIR)/eagle.app.flash.bin $(BIN_OUTPUT_FILE)
	$(QUIET) $(RM) $(BIN_DIR)/eagle.app.flash.bin $(BIN_DIR)/eagle.app.v6.text.bin $(BIN_DIR)/eagle.app.v6.data.bin $(BIN_DIR)/eagle.app.v6.rodata.bin $(BIN_DIR)/eagle.app.v6.irom0text.bin
