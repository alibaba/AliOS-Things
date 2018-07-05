EXTRA_POST_BUILD_TARGETS += gen_crc_bin

ESP8266_PYTHON := python2
ESP8266_PATH := ../../../platform/mcu/esp8266

ifneq ($(ESP8266_DWN_APP),)
ESP8266_DWN := 1
endif

ifneq ($(ESP8266_DWN),)
DOWNLOADBAUD ?= 921600
ESPPORT ?= /dev/ttyUSB0
BIN_OPTS := 0x1000 $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(BIN_OUTPUT_SUFFIX)
ifeq ($(ESP8266_DWN_APP),)
BIN_OPTS += 0 $(ESP8266_PATH)/bsp/boot_v1.7.bin \
	        0x3fc000 $(ESP8266_PATH)/bsp/esp_init_data_default.bin \
	        0x3fe000 $(ESP8266_PATH)/bsp/blank.bin
endif
DOWNLOAD_OPTS := --port $(ESPPORT) --baud $(DOWNLOADBAUD) write_flash --flash_size 4MB-c1 $(BIN_OPTS)
endif

ifneq ($(ESP8266_DBG),)
MONITORBAUD ?= 921600
ESPPORT ?= /dev/ttyUSB0
MONITOR_OPTS := --baud $(MONITORBAUD) --port $(ESPPORT) --toolchain-prefix $(TOOLCHAIN_PREFIX) --make "$(MAKE)"
APP_ELF := $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(LINK_OUTPUT_SUFFIX)
endif

gen_crc_bin:
	cd $(OUTPUT_DIR)/binary/; \
	$(OBJCOPY) --only-section .text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.text.bin; \
	$(OBJCOPY) --only-section .data -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.data.bin; \
    $(OBJCOPY) --only-section .rodata -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.rodata.bin; \
    $(OBJCOPY) --only-section .irom0.text -O binary $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) eagle.app.v6.irom0text.bin; \
	export ESP8266_NM=${NM}; \
    $(ESP8266_PYTHON) $(ESP8266_PATH)/tools/gen_appbin.py $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX) 2 1 15 5; \
	unset ESP8266_NM; \
	mv eagle.app.flash.bin $(CLEANED_BUILD_STRING)$(RADIXPOINT)$(BINSTYPE_LOWER)$(BIN_OUTPUT_SUFFIX); \
	rm eagle.app.v6.text.bin eagle.app.v6.data.bin eagle.app.v6.rodata.bin eagle.app.v6.irom0text.bin
ifneq ($(ESP8266_DWN),)
	cd $(OUTPUT_DIR)/binary/; \
	$(ESP8266_PYTHON) $(ESP8266_PATH)/tools/esptool.py $(DOWNLOAD_OPTS)
endif
ifneq ($(ESP8266_DBG),)
	cd $(OUTPUT_DIR)/binary/; \
	$(ESP8266_PYTHON) $(ESP8266_PATH)/tools/idf_monitor.py $(MONITOR_OPTS) $(APP_ELF)
endif
