ifeq ($(HOST_OS),Win32)
IMAGE_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/tools/imgpkt.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
IMAGE_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/tools/imgpkt"
else # Linux32
ifeq ($(HOST_OS),Linux64)
IMAGE_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/tools/imgpkt"
else # Linux64
ifeq ($(HOST_OS),OSX)
IMAGE_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/tools/imgpkt"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

CUSTOM_OTA := 1
LZMA_PACK := $($(HOST_MCU_FAMILY)_LOCATION)/tools/ota_pack_image_lzma.py
LZMA_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_lmza$(BIN_OUTPUT_SUFFIX))

EXTRA_POST_BUILD_TARGETS += gen_standard_images

gen_standard_images:
	echo "Generate LZMA bin ..."
	$(IMAGE_TOOL) e $(BIN_OUTPUT_FILE) $(LZMA_BIN_OUTPUT_FILE)
	echo "Generate OTA bin ..."
	$(PYTHON) $(LZMA_PACK) $(BIN_OUTPUT_FILE) $(LZMA_BIN_OUTPUT_FILE) $(OTA_BIN_OUTPUT_FILE)
	$(PYTHON) $(SCRIPTS_PATH)/ota_gen_md5_bin.py $(OTA_BIN_OUTPUT_FILE) -m $(IMAGE_MAGIC)
