ifeq ($(HOST_OS),Win32)
	ESPTOOL = $(shell where esptool.py)
	ifneq (,$(findstring esptool.py,$(ESPTOOL)))
	    ESPTOOL = esptool.py
	else
	    ESPTOOL = esptool.exe
	endif
else
	ESPTOOL = esptool.py
endif

EXTRA_POST_BUILD_TARGETS += gen_crc_bin
gen_crc_bin:
	$(ESPTOOL) --chip esp32 elf2image $(LINK_OUTPUT_FILE)
