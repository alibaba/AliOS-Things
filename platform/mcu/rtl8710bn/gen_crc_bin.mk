AMEBAZ_DIR = $($(HOST_MCU_FAMILY)_LOCATION)
SYSTEMBIN_DIR = board/$(PLATFORM)
AMEBAZ_TOOLDIR	= $(AMEBAZ_DIR)/tools
BIN_DIR= $(AMEBAZ_DIR)

TARGET=application

ota1_offset = 0x0800B000
ota2_offset = 0x08019000
EXTRA_POST_BUILD_TARGETS += gen_crc_bin gen_standard_images
EXTRA_POST_BUILD_TARGETS_2BOOT += gen_crc_bin_2boot gen_standard_images_2boot

gen_crc_bin:
ifneq ($(findstring $(HOST_OS),Win32 Win64),)
	@echo $(HOST_OS) $(ota_offset) $(BIN_DIR) $(APP_FULL) $(PLATFORM)
	$(eval OUT_MSG := $(shell cmd /c %cd%$($(HOST_MCU_FAMILY)_LOCATION)\script\postbuild_img2.bat $(ota2_offset) %cd% $(APP_FULL) $(PLATFORM)))
else
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	@echo $(HOST_OS) $(ota_idx) $(TOP_PATH) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)
	$(eval OUT_MSG := $(shell sh $(TOP_PATH)/platform/mcu/$(HOST_MCU_FAMILY)/script/manipulate_image.sh $(ota_idx) $(TOP_PATH) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)))
=======
	@echo $(HOST_OS) $(ota_offset) $(TOP_PATH) $(APP_FULL) $(PLATFORM)
	$(eval OUT_MSG := $(shell sh $(TOP_PATH)/platform/mcu/$(HOST_MCU_FAMILY)/script/manipulate_image.sh $(ota_offset) $(TOP_PATH) $(APP_FULL) $(PLATFORM)))
>>>>>>> 61271e937 (make the flash address of image2_all_ota1.bin can be modified.)
endif
	
=======
	@echo $(HOST_OS) $(ota_idx) $(TOP_PATH) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)
	$(eval OUT_MSG := $(shell sh $(TOP_PATH)/platform/mcu/$(HOST_MCU_FAMILY)/script/manipulate_image.sh $(ota2_offset) $(TOP_PATH) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)))
=======
	@echo $(HOST_OS) $(ota_idx) $(SOURCE_ROOT) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)
	$(eval OUT_MSG := $(shell sh $($(HOST_MCU_FAMILY)_LOCATION)/script/manipulate_image.sh $(ota2_offset) $(SOURCE_ROOT) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)))
>>>>>>> b18dda785 (BugID: 18138458: HOST_MCU_FAMILY changed to be comp name)
endif

gen_crc_bin_2boot:
ifneq ($(findstring $(HOST_OS),Win32 Win64),)
	@echo $(HOST_OS) $(ota_offset) $(BIN_DIR) $(APP_FULL) $(PLATFORM)
	$(eval OUT_MSG := $(shell cmd /c %cd%$($(HOST_MCU_FAMILY)_LOCATION)\script\postbuild_img2.bat $(ota1_offset) %cd% $(APP_FULL) $(PLATFORM)))
else
	@echo $(HOST_OS) $(ota_idx) $(SOURCE_ROOT) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)
	$(eval OUT_MSG := $(shell sh $($(HOST_MCU_FAMILY)_LOCATION)/script/manipulate_image.sh $(ota1_offset) $(SOURCE_ROOT) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)))
endif

#bootloader
BOOT_BIN_FILE    := $(OUTPUT_DIR)/binary/boot_all.bin
BOOT_OFFSET      := 0x0

#application
SYSTEM_BIN_FILE := $(SYSTEMBIN_DIR)/system.bin
SYSTEM_OFFSET:= 0x9000

#2boot
2BOOT_BIN_FILE :=$(OUTPUT_DIR)/binary/image2_2boot.bin
2BOOT_OFFSET:= 0xB000

#ate firmware
ATE_BIN_FILE := $(AMEBAZ_DIR)/ate.bin
ATE_OFFSET:= 0xD0000

#app
APP_BIN_FILE := $(OUTPUT_DIR)/binary/image2_app.bin
APP_OFFSET := 0x19000

OTA_OUTPUT_DIR := $(OUTPUT_DIR)/binary/ota

# Required to build Full binary file
GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT:= $(SCRIPTS_PATH)/gen_common_bin_output_file.py
APP_BIN_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=$(BIN_OUTPUT_SUFFIX))
2BOOT_BIN_OUTPUT_FILE := $(OUTPUT_DIR)/binary/$(PLATFORM).2boot.bin

MOC_APP_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:.2boot$(LINK_OUTPUT_SUFFIX)=$(BIN_OUTPUT_SUFFIX))
MOC_ALL_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:.2boot$(LINK_OUTPUT_SUFFIX)=.factory$(BIN_OUTPUT_SUFFIX))
MOC_OTA_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:.2boot$(LINK_OUTPUT_SUFFIX)=.ota$(BIN_OUTPUT_SUFFIX))

gen_standard_images: gen_crc_bin
	$(QUIET)$(ECHO) Generate Standard Flash Images: $(APP_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(APP_BIN_OUTPUT_FILE)
	$(QUIET)$(CP) $(APP_BIN_FILE) $(APP_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(APP_BIN_FILE)

gen_standard_images_2boot: gen_crc_bin_2boot
	$(QUIET)$(CP) $(MOC_APP_BIN_OUTPUT_FILE) $(MOC_OTA_BIN_OUTPUT_FILE)
	$(QUIET)$(ECHO) Generate Second Boot Standard Flash Images: $(2BOOT_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(2BOOT_BIN_OUTPUT_FILE)
	$(QUIET)$(CP) $(2BOOT_BIN_FILE) $(2BOOT_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(2BOOT_BIN_FILE)
	$(QUIET)$(ECHO) Generate ALL Standard Flash Images: $(MOC_APP_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(MOC_ALL_BIN_OUTPUT_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(BOOT_OFFSET) $(BOOT_BIN_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(SYSTEM_OFFSET)  $(SYSTEM_BIN_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(2BOOT_OFFSET)  $(2BOOT_BIN_OUTPUT_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(APP_OFFSET)  $(MOC_APP_BIN_OUTPUT_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(ATE_OFFSET)  $(ATE_BIN_FILE)
	$(QUIET)$(RM) $(MOC_APP_BIN_OUTPUT_FILE)
	$(QUIET)$(CP) $(MOC_ALL_BIN_OUTPUT_FILE) $(MOC_APP_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(MOC_ALL_BIN_OUTPUT_FILE)
<<<<<<< HEAD
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(BOOT_OFFSET) $(BOOT_BIN_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(SYSTEM_OFFSET)  $(SYSTEM_BIN_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(2BOOT_OFFSET)  $(2BOOT_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(APP_OFFSET)  $(MOC_APP_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(ATE_OFFSET)  $(ATE_BIN_FILE)
<<<<<<< HEAD
<<<<<<< HEAD

>>>>>>> 6177c74fe (code synchronization at 2018-06-06@13:59)
=======
>>>>>>> ea3b1f788 (BugID:17115756:[OTA] support mk3080 2nd bootloader)
=======
	$(RM) $(MOC_APP_BIN_OUTPUT_FILE)
	$(CP) $(MOC_ALL_BIN_OUTPUT_FILE) $(MOC_APP_BIN_OUTPUT_FILE)
>>>>>>> 92313b42b (BugID:17115756:[OTA]change final bin name)
=======
>>>>>>> d18c41a2e (BugID:17115756:[OTA] change 2nd boot binary path)
