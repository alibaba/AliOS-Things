
ifeq ($(HOST_OS),Win32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_win.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_linux"
XZ := /usr/bin/xz	
XZ_CMD = if [ -f $(XZ) ]; then $(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(OTA_BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_linux"
XZ := $(TOOLS_ROOT)/cmd/linux64/xz	
XZ_CMD = if [ -f $(XZ) ]; then $(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(OTA_BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_osx"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
OTA_BIN_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.ota$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += gen_crc_bin
gen_crc_bin:
	cd $(OUTPUT_DIR)/binary/; \
	$(OBJCOPY) --only-section .text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.text.bin; \
	$(OBJCOPY) --only-section .data -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.data.bin; \
    $(OBJCOPY) --only-section .rodata -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.rodata.bin; \
    $(OBJCOPY) --only-section .irom0.text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.irom0text.bin; \
    python2 ../../../platform/mcu/esp8266/tools/gen_appbin.py $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) 2 1 15 5; \
	mv eagle.app.flash.bin $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)-0x1000$(BIN_OUTPUT_SUFFIX); \
	rm eagle.app.v6.text.bin eagle.app.v6.data.bin eagle.app.v6.rodata.bin eagle.app.v6.irom0text.bin
