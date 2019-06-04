ifeq ($(HOST_OS),Win32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_win.exe"
XZ := "$($(HOST_MCU_FAMILY)_LOCATION)/xz.exe"
XZ_CMD = $(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(CRC_BIN_OUTPUT_FILE)
MD5_CMD = $(PYTHON) "$(SCRIPTS_PATH)/ota_gen_md5_bin.py" $(OTA_BIN_OUTPUT_FILE)
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
XZ := /usr/bin/xz
XZ_CMD = if [ -f $(XZ) ]; then $(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(CRC_BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
MD5_CMD = $(PYTHON) "$(SCRIPTS_PATH)/ota_gen_md5_bin.py" $(OTA_BIN_OUTPUT_FILE)
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
XZ := $(TOOLS_ROOT)/cmd/linux64/xz
XZ_CMD = if [ -f $(XZ) ]; then $(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(CRC_BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
MD5_CMD = $(PYTHON) "$(SCRIPTS_PATH)/ota_gen_md5_bin.py" $(OTA_BIN_OUTPUT_FILE)
else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_osx"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
OTA_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_ota$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += gen_crc_bin
gen_crc_bin:
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 0 0 0))
	$(XZ_CMD)
	$(QUIET)$(CP) $(CRC_BIN_OUTPUT_FILE).xz $(OTA_BIN_OUTPUT_FILE)
	$(MD5_CMD)
	