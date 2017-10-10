
IMAGE_FILE := $(YOC_OUT_PATH)/aos
L_TARGET_ROOT_PATH := targets/$(CONFIG_TARGET_ROOT_PATH)
L_LINK_SCRIPT := $(L_TARGET_ROOT_PATH)/STM32L475VGTx_FLASH.ld
L_IMAGE_BINARY_PATH := $(L_TARGET_ROOT_PATH)/bin
L_IMAGE_CFG_FILE := $(L_TARGET_ROOT_PATH)/boards/$(CONFIG_TARGET_BOARD)/image.cfg
L_TOOLS_PATH := $(YOC_BASE_PATH)/$(L_TARGET_ROOT_PATH)/tools
L_LDFLAGS := -T$(L_LINK_SCRIPT)

image: $(IMAGE_FILE)
	@ echo "*************************************************"

$(IMAGE_FILE): PRIVATE_LDFLAGS:=$(L_LDFLAGS) $(CSP_LDFLAGS)
$(IMAGE_FILE): $(YOC_OUT_TARGET_FILE) $(YOC_L_PRE_LIBS) $(YOC_L_EXCLUDE_LIBS)
	@ echo LD: $@
	$(CPRE) $(CC) -o $@ -Wl,--whole-archive $^ -Wl,--no-whole-archive -Wl,--gc-sections -Wl,-Map,$(YOC_OUT_PATH)/aos.map $(PRIVATE_LDFLAGS)
	$(CPRE) $(OBJDUMP) -d $@ > $@.asm
	$(CPRE) $(READELF) -a $@ > $@.s
	$(CPRE) $(OBJCOPY) -O binary $@ $@.bin

