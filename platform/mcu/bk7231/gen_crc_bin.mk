
ifeq ($(HOST_OS),Win32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_win.exe"
BOOT := "$($(HOST_MCU_FAMILY)_LOCATION)/uboot-bk7231-aos.bin"
WIFI_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/wifi.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
XZ := /usr/bin/xz
XZ_CMD = if [ -f $(XZ) ]; then $(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(OTA_BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
XZ := $(TOOLS_ROOT)/cmd/linux64/xz
XZ_CMD = if [ -f $(XZ) ]; then $(XZ) -f --lzma2=dict=32KiB --check=crc32 -k $(OTA_BIN_OUTPUT_FILE); else echo "xz need be installed"; fi
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
OTA_BIN_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.ota$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += gen_crc_bin
gen_crc_bin:
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 0 0 0))
ifeq ($(HOST_OS),Win32)
	$(eval OUT_MSG := $(shell $(WIFI_TOOL) $(CRC_BIN_OUTPUT_FILE) $(BOOT)))
endif
	$(QUIET)$(CP) $(CRC_BIN_OUTPUT_FILE) $(OTA_BIN_OUTPUT_FILE)
	$(QUIET)$(XZ_CMD)
