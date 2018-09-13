
no_with_xip := 0

ifeq ($(HOST_OS),Win32)
MKIMAGE_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/tools/mkimage.exe"
else  # Win32
ifeq ($(HOST_OS),Linux32)
MKIMAGE_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/tools/mkimage"
XZ_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/tools/xz"
else # Linux32
ifeq ($(HOST_OS),Linux64)
MKIMAGE_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/tools/mkimage"
XZ_TOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/tools/xz64"
else # Linux64
$(error not surport for $(HOST_OS))
endif # Linux64
endif # Linux32
endif # Win32

EXTRA_POST_BUILD_TARGETS += mkimage

ifneq ($(no_with_xip),1)
IMAGE_XIP := -xip
else
IMAGE_XIP :=
endif

ifneq ($(no_with_image_compress),1)
IMAGE_XZ := -xz
else
IMAGE_XZ :=
endif

ifneq ($(no_with_ota),1)
IMAGE_OTA := -O
else
IMAGE_OTA :=
endif

IMAGE_CFG_FILE ?= "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/image-${APP}${IMAGE_XZ}.cfg"
IMAGE_PACK_DIR ?= "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/pack"
BINARY_DIR ?= $(SOURCE_ROOT)out/$(CLEANED_BUILD_STRING)/binary

mkimage:
	$(OBJCOPY) -O binary -R .xip -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes $(BINARY_DIR)/$(CLEANED_BUILD_STRING).elf $(BINARY_DIR)/$(CLEANED_BUILD_STRING).bin
	$(OBJCOPY) -O binary -j .xip $(BINARY_DIR)/$(CLEANED_BUILD_STRING).elf $(BINARY_DIR)/$(CLEANED_BUILD_STRING).xip.bin
	$(CP) -vf $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/bin/xr871/*.bin  $(IMAGE_PACK_DIR)/
	$(CP) -vf $(SOURCE_ROOT)out/$(CLEANED_BUILD_STRING)/binary/$(CLEANED_BUILD_STRING).bin  $(IMAGE_PACK_DIR)/app.bin
	$(CP) -vf $(SOURCE_ROOT)out/$(CLEANED_BUILD_STRING)/binary/$(CLEANED_BUILD_STRING).xip.bin  $(IMAGE_PACK_DIR)/app-xip.bin
ifneq ($(no_with_image_compress),1)
	$(RM) -vf $(IMAGE_PACK_DIR)/*.bin.xz
	$(XZ_TOOL) -vk --check=crc32 --lzma2=preset=6e,dict=32KiB $(IMAGE_PACK_DIR)/net.bin
	$(XZ_TOOL) -vk --check=crc32 --lzma2=preset=6e,dict=32KiB $(IMAGE_PACK_DIR)/net_ap.bin
endif
	$(CP) -vf $(MKIMAGE_TOOL)  $(IMAGE_PACK_DIR)/
	$(CP) -vf $(IMAGE_CFG_FILE)  $(IMAGE_PACK_DIR)/
	cd $(IMAGE_PACK_DIR) && ./mkimage ${IMAGE_OTA} -c image-${APP}${IMAGE_XZ}.cfg
