
ifeq ($(HOST_OS),Win32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_win.exe"
DD_CMD = $(TOOLS_ROOT)/cmd/win32/dd.exe if=/dev/zero of=$(TMP_BIN_OUTPUT_FILE) bs=28 count=1
CAT_CMD= $(TOOLS_ROOT)/cmd/win32/cat.exe $(TMP_BIN_OUTPUT_FILE) $(BIN_OUTPUT_FILE) > $(PAD_BIN_OUTPUT_FILE)
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
XZ := /usr/bin/xz
XZ_CMD = if [ -f $(XZ) ]; then $(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
DD_CMD = dd if=/dev/zero of=$(TMP_BIN_OUTPUT_FILE) bs=28 count=1
CAT_CMD= cat $(TMP_BIN_OUTPUT_FILE) $(BIN_OUTPUT_FILE) > $(PAD_BIN_OUTPUT_FILE)
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
XZ := $(TOOLS_ROOT)/cmd/linux64/xz
XZ_CMD = if [ -f $(XZ) ]; then $(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
DD_CMD = dd if=/dev/zero of=$(TMP_BIN_OUTPUT_FILE) bs=28 count=1
CAT_CMD= cat $(TMP_BIN_OUTPUT_FILE) $(BIN_OUTPUT_FILE) > $(PAD_BIN_OUTPUT_FILE)
else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_osx"
DD_CMD = dd if=/dev/zero of=$(TMP_BIN_OUTPUT_FILE) bs=28 count=1
CAT_CMD= cat $(TMP_BIN_OUTPUT_FILE) $(BIN_OUTPUT_FILE) > $(PAD_BIN_OUTPUT_FILE)
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

OTA_OUTPUT_DIR := $(OUTPUT_DIR)/binary/ota
CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
TMP_BIN_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.tmp$(BIN_OUTPUT_SUFFIX))
PAD_BIN_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.pad0$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += gen_crc_bin
gen_crc_bin:
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 0 0 0))
	$(QUIET)$(CP) $(CRC_BIN_OUTPUT_FILE) $(BIN_OUTPUT_FILE)
	$(DD_CMD)
	$(CAT_CMD)
	$(QUIET)$(CP) $(PAD_BIN_OUTPUT_FILE) $(BIN_OUTPUT_FILE)
	@$(XZ_CMD)
	$(QUIET)$(RM) -f $(CRC_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) -f $(TMP_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) -f $(PAD_BIN_OUTPUT_FILE)

BOOT_OFFSET      := 0x0
APP_OFFSET       := 0x8E00   #0x1C000 - 0x13200
GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT:= $(SCRIPTS_PATH)/gen_common_bin_output_file.py

BOOT_OUT_BIN_OUTPUT_FILE := $(dir $(BIN_OUTPUT_FILE))mk3060.2boot.bin
APP_BIN_OUTPUT_FILE := $(BIN_OUTPUT_FILE:.2boot.bin=.bin)
ALL_BIN_OUTPUT_FILE := $(BIN_OUTPUT_FILE:.2boot.bin=.factory.bin)
OTA_BIN_OUTPUT_FILE := $(BIN_OUTPUT_FILE:.2boot.bin=.ota.bin)

EXTRA_POST_BUILD_TARGETS_2BOOT += gen_crc_bin_2boot
gen_crc_bin_2boot:
	$(CP) $(APP_BIN_OUTPUT_FILE) $(OTA_BIN_OUTPUT_FILE)
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 0 0 0))
	$(QUIET)$(RM) -f $(BIN_OUTPUT_FILE)
	$(QUIET)$(CP) $(CRC_BIN_OUTPUT_FILE) $(BOOT_OUT_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) -f $(CRC_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(BOOT_OFFSET) $(BOOT_OUT_BIN_OUTPUT_FILE)
	$(PYTHON) $(GEN_COMMON_BIN_OUTPUT_FILE_SCRIPT) -o $(ALL_BIN_OUTPUT_FILE) -f $(APP_OFFSET) $(APP_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) -f $(APP_BIN_OUTPUT_FILE)
	$(QUIET)$(CP) $(ALL_BIN_OUTPUT_FILE) $(APP_BIN_OUTPUT_FILE)
	$(QUIET)$(RM) -f $(ALL_BIN_OUTPUT_FILE)


