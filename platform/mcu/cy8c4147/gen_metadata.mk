CYELFTOOL := "$(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/Export/cyelftool.exe"

EXTRA_POST_BUILD_TARGETS += gen_metadata

gen_metadata:
	$(CYELFTOOL) -C $(LINK_OUTPUT_FILE) --flash_row_size 256 --flash_size 131072 --flash_offset 0x0
