ifeq ($(HOST_OS),Win32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_win.exe"
WIFI_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/wifi_win.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_linux"
WIFI_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/wifi_linux"
else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_osx"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

ota1_offset = 69632 # 0x11000
ota2_offset = 1044480 # 0xFF000

CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
CRC_BIN_OUTPUT_FILE_XIP2 :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.xip2_crc$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += gen_crc_bin gen_standard_images
gen_crc_bin:
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE) 0 0 0 0))
	$(eval OUT_MSG := $(shell $(ENCRYPT) $(BIN_OUTPUT_FILE_XIP2) 0 0 0 0))
	

gen_standard_images:
	@echo $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_OUTPUT_FILE_XIP2) $(ota2_offset) 
	$(eval OUT_MSG := $(shell $(WIFI_TOOL) $(CRC_BIN_OUTPUT_FILE) $(ota1_offset) $(CRC_BIN_OUTPUT_FILE_XIP2) $(ota2_offset)))

