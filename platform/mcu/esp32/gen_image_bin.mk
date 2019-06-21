ifeq ($(HOST_OS),Win32)
	ESPTOOL = $(shell where esptool.py)
	ifneq (,$(findstring esptool.py,$(ESPTOOL)))
	    ESPTOOL = esptool.py
	else
	    ESPTOOL = esptool.exe
	endif
else
	ESPTOOL = $(shell which esptool.py)
	ifeq (,$(findstring esptool.py,$(ESPTOOL)))
	    ESPTOOL = esptool.py
	endif
endif

EXTRA_POST_BUILD_TARGETS += gen_standard_images

gen_standard_images:
	$(ESPTOOL) --chip esp32 elf2image $(LINK_OUTPUT_FILE)
