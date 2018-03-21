
ifeq ($(HOST_OS),Win32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_win.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_linux"
else # Linux32
ifeq ($(HOST_OS),Linux64)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_linux"
else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/encrypt_osx"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

BASE_PWD=`pwd`
MCU_FILE := $(BASE_PWD)/platform/mcu/$(HOST_MCU_FAMILY)

CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))
OTA_BIN_OUTPUT_FILE := $(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.ota$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += gen_crc_bin

gen_crc_bin:
	$(QUIET)cp -f $(BIN_OUTPUT_FILE) $(MCU_FILE)/generated/imgs/prim
	$(QUIET)sh $(MCU_FILE)/generated/mtbhex.sh $(MCU_FILE) $(OBJCOPY)
