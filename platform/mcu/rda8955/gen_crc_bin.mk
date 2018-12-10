
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

SRECMAP := "$($(HOST_MCU_FAMILY)_LOCATION)/build/srecmap"
SRECMAP_CFG := "$($(HOST_MCU_FAMILY)_LOCATION)/build/8955_map_cfg"
TARGETGEN := "$($(HOST_MCU_FAMILY)_LOCATION)/build/targetgen"
LODMERGE := "$($(HOST_MCU_FAMILY)_LOCATION)/build/dual_boot_merge.py"
LODBOOT := "$($(HOST_MCU_FAMILY)_LOCATION)/build/blfota_8955_modem_test_debug_flash.lod"

else # Linux64
ifeq ($(HOST_OS),OSX)
ENCRYPT := "$($(HOST_MCU_FAMILY)_LOCATION)/encrypt_osx"
else # OSX
$(error not surport for $(HOST_OS))
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32

#CRC_BIN_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_crc$(BIN_OUTPUT_SUFFIX))

SREC_OUTPUT_FILE           :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.srec)	
SREC_OUTPUT_TMPFILE           :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=)
LOD_OUTPUT_TMPFILE           :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=flash.lod)
LOD_OUTPUT_FILE           :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_final.lod)

EXTRA_POST_BUILD_TARGETS += gen_crc_bin
gen_crc_bin:
	$(SRECMAP) -c $(SRECMAP_CFG) -m flsh_spi32m -b $(TARGETGEN) $(SREC_OUTPUT_FILE)  $(SREC_OUTPUT_TMPFILE)
	python $(LODMERGE) --bl="$(LODBOOT)" \
	--lod="$(LOD_OUTPUT_TMPFILE)" \
	--output="$(LOD_OUTPUT_FILE)"
