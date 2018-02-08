AMEBAZ_DIR = $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)
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

gen_crc_bin:
	$(QUIET) $(ECHO) raulraul $(HOST_MCU_FAMILY)
	cmd /c  %cd%\platform\mcu\$(HOST_MCU_FAMILY)\script\postbuild_img2.bat $(ota_offset) %cd% $(APP_FULL) $(PLATFORM)
