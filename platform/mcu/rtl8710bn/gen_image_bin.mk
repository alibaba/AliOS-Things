AMEBAZ_DIR = $($(HOST_MCU_FAMILY)_LOCATION)
SYSTEMBIN_DIR = board/$(PLATFORM)
AMEBAZ_TOOLDIR	= $(AMEBAZ_DIR)/tools
BIN_DIR= $(AMEBAZ_DIR)

TARGET=application

ota1_offset = 0x0800B000
ota2_offset = 0x08019000
ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
EXTRA_POST_BUILD_TARGETS += gen_image_bin_2ndboot gen_standard_images_2ndboot
else
EXTRA_POST_BUILD_TARGETS += gen_image_bin gen_standard_images
endif

gen_image_bin:
ifneq ($(findstring $(HOST_OS),Win32 Win64),)
	@echo $(HOST_OS) $(ota_offset) $(BIN_DIR) $(APP_FULL) $(PLATFORM)
	$(eval OUT_MSG := $(shell cmd /c %cd%$($(HOST_MCU_FAMILY)_LOCATION)\script\postbuild_img2.bat $(ota2_offset) %cd% $(APP_FULL) $(PLATFORM)))
else
	@echo $(HOST_OS) $(ota_idx) $(SOURCE_ROOT) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)
	$(eval OUT_MSG := $(shell sh $($(HOST_MCU_FAMILY)_LOCATION)/script/manipulate_image.sh $(ota2_offset) $(SOURCE_ROOT) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)))
endif

gen_image_bin_2ndboot:
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

#2ndboot
2NDBOOT_BIN_FILE :=$(OUTPUT_DIR)/binary/image2_2ndboot.bin
2NDBOOT_OFFSET:= 0xB000

#ate firmware
ATE_BIN_FILE := $(AMEBAZ_DIR)/ate.bin
ATE_OFFSET:= 0xD0000

#app
APP_BIN_FILE := $(OUTPUT_DIR)/binary/image2_app.bin
APP_OFFSET := 0x19000

# Required to build Full binary file
GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT:= $(SCRIPTS_PATH)/gen_common_bin_output_file.py

BOOT_BIN_OUTPUT_FILE := $(OUTPUT_DIR)/binary/$(PLATFORM).2ndboot.bin
APP_BIN_OUTPUT_FILE := $(BIN_OUTPUT_FILE:.2ndboot.bin=.bin)
OTA_APP_BIN_OUTPUT_FILE := $(BIN_OUTPUT_FILE:.2ndboot.bin=_ota.bin)
ALL_BIN_OUTPUT_FILE := $(BIN_OUTPUT_FILE:.2ndboot.bin=.all.bin)

gen_standard_images: gen_image_bin
	$(QUIET)$(ECHO) Generate Standard Images: $(BIN_OUTPUT_FILE)
	$(QUIET)$(CP) $(APP_BIN_FILE) $(BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(APP_BIN_FILE)

gen_standard_images_2ndboot: gen_image_bin_2ndboot
	$(QUIET)$(ECHO) Generate Second Boot Images: $(BOOT_BIN_OUTPUT_FILE)
	$(QUIET)$(CP) $(2NDBOOT_BIN_FILE) $(BOOT_BIN_OUTPUT_FILE)
	$(QUIET)$(ECHO) Generate ALL Standard Flash Images: $(BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(ALL_BIN_OUTPUT_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(BOOT_OFFSET) $(BOOT_BIN_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(SYSTEM_OFFSET)  $(SYSTEM_BIN_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(2NDBOOT_OFFSET)  $(BOOT_BIN_OUTPUT_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(APP_OFFSET)  $(OTA_APP_BIN_OUTPUT_FILE)
	$(QUIET)$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(ATE_OFFSET)  $(ATE_BIN_FILE)
	$(QUIET)$(RM) $(APP_BIN_OUTPUT_FILE)
	$(QUIET)$(CP) $(ALL_BIN_OUTPUT_FILE) $(APP_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(ALL_BIN_OUTPUT_FILE)

