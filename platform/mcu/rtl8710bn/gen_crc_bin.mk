ifneq ($(findstring $(HOST_OS),Win32 Win64),)
AMEBAZ_DIR = $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)
else 
TOP_PATH = $(shell pwd)
AMEBAZ_DIR = $(TOP_PATH)/platform/mcu/$(HOST_MCU_FAMILY)
endif
#AMEBAZ_DIR = C:/raul/code/AliOS/AliOS-Things/platform/mcu/rtl8710bn
#AMEBAZ_DIR = C:\raul\code\AliOS\AliOS-Things\platform\mcu\rtl8710bn
AMEBAZ_TOOLDIR	= $(AMEBAZ_DIR)/tools
#BIN_DIR= $(OUTPUT_DIR)/binary
BIN_DIR= $(AMEBAZ_DIR)

TARGET=application

ota1_offset = 0x0800B000
ota2_offset = 0x08100000
EXTRA_POST_BUILD_TARGETS += gen_crc_bin gen_standard_images 
#cmd /c  %cd%\platform\mcu\$(HOST_MCU_FAMILY)\script\postbuild_img2.bat $(ota_offset) %cd% $(APP_FULL) $(PLATFORM)
#$(eval OUT_MSG := $(shell cmd /c %cd%\platform\mcu\$(HOST_MCU_FAMILY)\script\postbuild_img2.bat $(ota_offset) %cd% $(APP_FULL) $(PLATFORM)))	
#$(QUIET) $(ECHO) $(HOST_MCU_FAMILY)

gen_crc_bin:	
ifneq ($(findstring $(HOST_OS),Win32 Win64),)
	@echo $(HOST_OS) $(ota_offset) $(BIN_DIR) $(APP_FULL) $(PLATFORM)
	$(eval OUT_MSG := $(shell cmd /c %cd%\platform\mcu\$(HOST_MCU_FAMILY)\script\postbuild_img2.bat $(ota1_offset) %cd% $(APP_FULL) $(PLATFORM)))
	$(eval OUT_MSG := $(shell cmd /c %cd%\platform\mcu\$(HOST_MCU_FAMILY)\script\postbuild_img2.bat $(ota2_offset) %cd% $(APP_FULL) $(PLATFORM)))
else
	@echo $(HOST_OS) $(ota_idx) $(TOP_PATH) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)
	$(eval OUT_MSG := $(shell sh $(TOP_PATH)/platform/mcu/$(HOST_MCU_FAMILY)/script/manipulate_image.sh $(ota1_offset) $(TOP_PATH) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)))
	$(eval OUT_MSG := $(shell sh $(TOP_PATH)/platform/mcu/$(HOST_MCU_FAMILY)/script/manipulate_image.sh $(ota2_offset) $(TOP_PATH) $(APP_FULL) $(PLATFORM) $(TOOLCHAIN_PATH)))
endif

#bootloader
BOOT_BIN_FILE    := $(OUTPUT_DIR)/binary/boot_all.bin
BOOT_OFFSET      := 0x0

#application 
SYSTEM_BIN_FILE :=board/mk3080/system.bin
SYSTEM_OFFSET:= 0x9000

#application 
APP_BIN_FILE :=$(OUTPUT_DIR)/binary/image2_all_ota1.bin
APP_OFFSET:= 0xB000

#ate firmware
ATE_BIN_FILE := board/mk3080/ate.bin
ATE_OFFSET:= 0xD0000

# Required to build Full binary file
GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT:= $(SCRIPTS_PATH)/gen_common_bin_output_file.py

MOC_ALL_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.all$(BIN_OUTPUT_SUFFIX))

gen_standard_images: gen_crc_bin
	$(QUIET)$(ECHO) Generate Standard Flash Images: $(MOC_ALL_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) $(MOC_ALL_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(BOOT_OFFSET) $(BOOT_BIN_FILE)              
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(SYSTEM_OFFSET)  $(SYSTEM_BIN_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(APP_OFFSET)  $(APP_BIN_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(MOC_ALL_BIN_OUTPUT_FILE) -f $(ATE_OFFSET)  $(ATE_BIN_FILE)

