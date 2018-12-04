EXTRA_POST_BUILD_TARGETS_2BOOT += gen_standard_images_2boot

OTA_OUTPUT_DIR := $(OUTPUT_DIR)/binary/ota
BOOT2_BIN_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=$(BIN_OUTPUT_SUFFIX))

MOC_APP_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:.2boot$(LINK_OUTPUT_SUFFIX)=$(BIN_OUTPUT_SUFFIX))
MOC_ALL_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:.2boot$(LINK_OUTPUT_SUFFIX)=.factory$(BIN_OUTPUT_SUFFIX))
TMP_OTA_BIN_OUTPUT_FILE :=$(OTA_OUTPUT_DIR)/$(notdir $(LINK_OUTPUT_FILE:.2boot$(LINK_OUTPUT_SUFFIX)=.ota$(BIN_OUTPUT_SUFFIX)))
MOC_OTA_BIN_OUTPUT_FILE :=$(OTA_OUTPUT_DIR)/$(subst @,_,$(notdir $(TMP_OTA_BIN_OUTPUT_FILE)))

1BOOT_BIN_FILE :=$(SOURCE_ROOT)/board/developerkit/developerkit_boot_1nd.bin

BANK1_BOOT_OFFSET:= 0x0
BANK1_2BOOT_OFFSET:= 0x1800
BANK1_APP_OFFSET := 0x9000
BANK2_BOOT_OFFSET:= 0x80000
BANK2_2BOOT_OFFSET:= 0x81800
BANK2_APP_OFFSET := 0x89000

# Required to build Full binary file
GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT:= $(SCRIPTS_PATH)/gen_common_bin_output_file.py

gen_standard_images_2boot:
	mkdir -p $(OTA_OUTPUT_DIR)
	$(QUIET)$(CP) $(MOC_APP_BIN_OUTPUT_FILE) $(MOC_OTA_BIN_OUTPUT_FILE)
	$(QUIET)$(ECHO) Generate Second Boot Standard Flash Images: $(BOOT2_BIN_OUTPUT_FILE)
	$(QUIET)$(ECHO) Generate ALL Standard Flash Images: $(MOC_ALL_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(MOC_ALL_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(BANK1_BOOT_OFFSET) $(1BOOT_BIN_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(BANK1_2BOOT_OFFSET) $(BOOT2_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(BANK1_APP_OFFSET) $(MOC_OTA_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(BANK2_BOOT_OFFSET)  $(1BOOT_BIN_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(BANK2_2BOOT_OFFSET) $(BOOT2_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(BANK2_APP_OFFSET) $(MOC_OTA_BIN_OUTPUT_FILE)
