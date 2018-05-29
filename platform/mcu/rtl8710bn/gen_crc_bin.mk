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

ota_idx = 1
ota_offset = 0x0800B000
#ota1_offset = 0x0800B000
#ota2_offset = 0x08100000
EXTRA_POST_BUILD_TARGETS += gen_crc_bin
#cmd /c  %cd%\platform\mcu\$(HOST_MCU_FAMILY)\script\postbuild_img2.bat $(ota_offset) %cd% $(APP_FULL) $(PLATFORM)
#$(eval OUT_MSG := $(shell cmd /c %cd%\platform\mcu\$(HOST_MCU_FAMILY)\script\postbuild_img2.bat $(ota_offset) %cd% $(APP_FULL) $(PLATFORM)))	
#$(QUIET) $(ECHO) $(HOST_MCU_FAMILY)

gen_crc_bin:
ifneq ($(findstring $(HOST_OS),Win32 Win64),)
	@echo $(HOST_OS) $(ota_offset) $(BIN_DIR) $(APP_FULL) $(PLATFORM)
	$(eval OUT_MSG := $(shell cmd /c %cd%\platform\mcu\$(HOST_MCU_FAMILY)\script\postbuild_img2.bat $(ota_offset) %cd% $(APP_FULL) $(PLATFORM)))
else

	@echo $(HOST_OS) $(ota_offset) $(TOP_PATH) $(APP_FULL) $(PLATFORM)
	$(eval OUT_MSG := $(shell sh $(TOP_PATH)/platform/mcu/$(HOST_MCU_FAMILY)/script/manipulate_image.sh $(ota_offset) $(TOP_PATH) $(APP_FULL) $(PLATFORM)))

endif
	
