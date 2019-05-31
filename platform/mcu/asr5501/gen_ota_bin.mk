EXTRA_POST_BUILD_TARGETS += gen_ota_bin

OTA_IMAGE_MODE := "image_compress"
ifeq ($(HOST_OS),Win32)
IMAGE_GEN_HEADER_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/tools/image_gen_header/image_gen_header.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
IMAGE_GEN_HEADER_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/tools/image_gen_header/image_gen_header"
else # Linux32
ifeq ($(HOST_OS),Linux64)
IMAGE_GEN_HEADER_TOOL := "$($(HOST_MCU_FAMILY)_LOCATION)/tools/image_gen_header/image_gen_header"
else
$(error not surport for $(HOST_OS))
endif # Linux64
endif # Linux32
endif # Win32

gen_ota_bin:
	$(IMAGE_GEN_HEADER_TOOL) $(BIN_OUTPUT_FILE) $(OTA_IMAGE_MODE)