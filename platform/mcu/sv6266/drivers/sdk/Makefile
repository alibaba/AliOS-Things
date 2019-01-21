##############################################################
# Make version check.
##############################################################
ifeq (3.82,$(firstword $(sort $(MAKE_VERSION) 3.82)))


##############################################################
# Global Variables
##############################################################
TOPDIR          := $(CURDIR)
TOPOUT			:= out
BUILD			:= $(TOPDIR)/build
TARGET_CHIP 	?= turismo
OS 				:= freertos
BUILD_ROM		?= 0
FLASH_MODE		?= 0
SDKDIR			?= $(TOPDIR)
FLASH_CTRL 		?= 1
SDK_VERSION		:= version.c
SDK_VERSION_OBJ	:= $(SDK_VERSION:.c=.o)
#CUSTOMER		?= none

POST_BUILD_CMD	?=

BUILD_MODE		?= release

BUILD_ENV		:= $(shell uname)
HOST_ARCH		:= $(shell uname -m)
REDIRECT_BUILDLOG := 1

SHELL=/bin/bash -o pipefail


export TOPDIR TOPOUT BUILD


# config will include project.mk
sinclude $(BUILD)/project_cfg.mk
include $(BUILD)/config.mk
include $(BUILD)/macro.mk
include $(BUILD)/link_macro.mk

#CFLAGS += -DCUSTOMER="$(CUSTOMER)"
##############################################################
# ROM Variables
##############################################################

##############################################################
# Local Variables
##############################################################
IMAGE_DIR       := $(TOPDIR)/image
IMAGE_NAME		:= $(IMAGE_DIR)/$(PROJECT)
LOG_TABLE		:= $(IMAGE_DIR)/$(PROJECT).json

##############################################################
# init Variables
##############################################################
LIB_DEP :=
LIBRARIES :=
LIBRARIES_DIR :=
CLEAN_RULE :=
TMP_SRC :=

##############################################################
# Source directories to Make
##############################################################
SRC_DIRS +=  $(IMPORT_DIR)

#$(call foreach-dir,$(SRC_DIRS))
#$(foreach _d,$(SRC_DIRS),$(eval $(call inc-module,$(_d))))

.PHONY: all clean list_projects setup sync_subprojects bootloader bootloader_clean flashtool flashtool_clean
.SECONDARY: main-build

all: pre-build main-build flashtool

pre-build:
	$(Q)echo static lib:
	$(Q)echo $(STATIC_LIB)
	$(Q)echo > build_error.log
	$(Q)if [ -f $(BUILD)/project_cfg.mk ]; then \
		$(UNLOCK) $(TOPOUT)/string_table.json; \
		$(CHMOD) $(TOPDIR)/utils/*; \
	else \
		echo $(BUILD)/project_cfg.mk not found; \
		echo please find project use \"make list_projects\", set project use \"make setup p=PROJECT_NAME\"; \
		$(MAKE) list_projects; \
	fi

post-build:
	@echo build end
ifeq ($(strip $(BUILD_SHOW_ILM_INFO)), 1)
	$(Q)echo ILM remain: $$( grep ilm_remain $(IMAGE_DIR)/$(PROJECT).map | awk --non-decimal-data '{printf "%d",$$1}' )
endif
ifeq ($(strip $(BUILD_SHOW_DLM_INFO)), 1)
	$(Q)echo DLM remain: $$( grep dlm_remain $(IMAGE_DIR)/$(PROJECT).map | awk --non-decimal-data '{printf "%d",$$1}' )
endif
	$(POST_BUILD_CMD)

ifeq ($(strip $(PROJECT)), )
main-build:
	$(Q)$(MAKE) --no-print-directory post-build
else
main-build: $(LIB_DEP) $(IMAGE_DIR)/$(PROJECT).bin $(IMAGE_DIR)/$(PROJECT).asm
	@echo "# linking with libraries $(LINK_LIBRARIES)"
	@echo "# linking with libraries flags $(LIBRARIES_DIR)"
	@echo "# making $(PROJECT) completed" "($(shell date))"
	$(Q)$(MAKE) --no-print-directory post-build
endif

include build/docs.mk

$(call foreach-dir, $(SRC_DIRS))
LINK_LIBRARIES := $(addprefix -l,$(LIBRARIES))


LINKSCRIPT := $(TOPOUT)/linkscript.lds

##############################################################
# target
##############################################################

ifeq ($(strip $(ROM_MODE)), 1)
$(LINKSCRIPT): $(LDSCRIPT_S)
	$(Q)$(CC) -E -P $< $(INC) -Irom_symbol/$(ROM_VERSION) $(CFLAGS) -o $@
else
$(LINKSCRIPT): $(LDSCRIPT_S)
	$(Q)$(CC) -E -P $< $(INC) -DNO_ROM $(CFLAGS) -o $@
endif

#$(IMAGE_DIR):
#	$(Q)$(MKDIR) $@
#

$(IMAGE_DIR)/$(PROJECT)$(ELFSUFFIX): $(LIB_DEP) $(LINKSCRIPT) $(PROJECT_SRC) $(SDK_VERSION_OBJ)
	$(Q)if [ ! -d $(IMAGE_DIR) ]; then \
		$(MKDIR) $(IMAGE_DIR); \
	fi
	$(Q)for SRC in $(PROJECT_SRC); \
	do \
		cd ${CURDIR}/`dirname $$SRC` && $(CC) $(CFLAGS) $(INC) -c `basename $$SRC`; \
	done
	$(Q)$(CC) $(LIBRARIES_DIR) $(LDFLAGS_PRE) $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(PROJECT_SRC))) $(PROJECT_OBJ) -Wl,--start-group $(LINK_LIBRARIES) -Wl,--end-group $(INC) $(CFLAGS) $(LDFLAGS) -Xlinker -Map=$(IMAGE_DIR)/$(PROJECT).map -T $(LINKSCRIPT) -o $@ $(SDK_VERSION_OBJ) $(STATIC_LIB)

ifeq ($(strip $(FLASH_MODE)), 1)
ifeq ($(filter FPGAv2 ASIC ASICv2,$(TARGET_DEF)), $(TARGET_DEF))
$(IMAGE_DIR)/$(PROJECT).bin: $(IMAGE_DIR)/$(PROJECT)$(ELFSUFFIX)
	$(Q)$(OBJCOPY) -O binary $< $@
	md5sum $@ | awk '{print $$1}' > $(IMAGE_DIR)/$(PROJECT).md5
ifeq ($(strip $(SUPPORT_BOOTLOADER)), 1)
	@if [ ! -f "$(IMAGE_DIR)/bootloader.bin" ]; then \
		make bootloader; \
	fi
	@if [ -f "$(IMAGE_DIR)/bootloader.bin" ]; then \
		echo "find bootloader.bin, will generate combined image"; \
		mv $(IMAGE_DIR)/$(PROJECT).bin $(IMAGE_DIR)/$(PROJECT).bin.tmp; \
		export bootloader_size=$$(stat -c%s $(IMAGE_DIR)/bootloader.bin); \
		export padding_size=$$(echo 16384 $$bootloader_size | awk '{print $$1 - $$2}'); \
		dd if=/dev/zero of=$(IMAGE_DIR)/padding.bin bs=1 count=$$padding_size; \
		cat $(IMAGE_DIR)/bootloader.bin $(IMAGE_DIR)/padding.bin $(IMAGE_DIR)/$(PROJECT).bin.tmp > $(IMAGE_DIR)/$(PROJECT).bin; \
		rm -f $(IMAGE_DIR)/padding.bin; \
		rm -f $(IMAGE_DIR)/$(PROJECT).bin.tmp; \
		make bootloader_clean; \
	fi
endif
else
$(IMAGE_DIR)/$(PROJECT).bin: $(IMAGE_DIR)/$(PROJECT)$(ELFSUFFIX) utils/flash_header
	$(OBJCOPY) -O binary $< $@.tmp
	utils/test_flash_bin.sh $< $@.tmp $@
endif
else
$(IMAGE_DIR)/$(PROJECT).bin: $(IMAGE_DIR)/$(PROJECT)$(ELFSUFFIX)
	$(Q)$(OBJCOPY) -O binary $< $@
	md5sum $@ | awk '{print $$1}' > $(IMAGE_DIR)/$(PROJECT).md5
endif

$(IMAGE_DIR)/$(PROJECT).asm: $(IMAGE_DIR)/$(PROJECT)$(ELFSUFFIX)
	$(Q)$(OBJDUMP) -d $< -S > $@

utils/flash_header: utils/flash_header.c
	@echo "$(Q)gcc $< -o $@"
	@echo "$(BUILD_ENV)"
	$(Q)gcc $< -o $@

all_libs: $(addsuffix .a, $(addprefix lib, $(LIBRARIES)))
	$(Q)$(SIZE) $(addsuffix .a, $(addprefix libs/lib, $(LIBRARIES)))
	
clean:
	$(Q)$(RM) -f utils/flash_header utils/SPI_burn utils/USB_burn utils/*.so utils/*.dll
	$(Q)$(RM) -f $(LOG_TABLE) 2> /dev/null | true
	$(Q)$(RM) -Rf $(TOPDIR)/out 2> /dev/null | true
	$(Q)$(RM) -f $(patsubst %.S,%.o,$(patsubst %.c,%.o,${PROJECT_SRC}))
	$(Q)$(RM) -f $(IMAGE_DIR)/$(PROJECT)$(ELFSUFFIX)
	$(Q)$(RM) -f $(IMAGE_DIR)/$(PROJECT).bin
	$(Q)$(RM) -f $(IMAGE_DIR)/$(PROJECT).asm
	$(Q)$(RM) -f $(IMAGE_DIR)/$(PROJECT).map
	$(Q)$(RM) -f $(IMAGE_DIR)/$(PROJECT).md5
	@echo "clean all completed" "($(shell date))"
#$(Q)$(RM) -Rf $(IMAGE_DIR) 2> /dev/null | true
	
distclean: flashtool_clean
	make bootloader_clean
	make clean
	$(Q)$(RM) -f $(BUILD)/project_cfg.mk
	@echo "distclean all completed" "($(shell date))"
	
bootloader:
	@if [ -f "$(PROJ_DIR)/src/bootloader/bootloader.mk" ]; then \
		echo "build bootloader"; \
		make -f $(PROJ_DIR)/src/bootloader/bootloader.mk; \
		mkdir -p $(IMAGE_DIR) && cp -f $(PROJ_DIR)/src/bootloader/bootloader.elf $(PROJ_DIR)/src/bootloader/bootloader.bin $(PROJ_DIR)/src/bootloader/bootloader.map $(IMAGE_DIR)/; \
		echo bootloader size $$(stat -c%s $(IMAGE_DIR)/bootloader.bin); \
	else \
		echo "bootloader.mk not found"; \
	fi

bootloader_clean:
	@if [ -f "$(PROJ_DIR)/src/bootloader/bootloader.mk" ]; then \
		echo "clean bootloader"; \
		make -f $(PROJ_DIR)/src/bootloader/bootloader.mk clean; \
		rm -f $(IMAGE_DIR)/bootloader.elf $(IMAGE_DIR)/bootloader.bin $(IMAGE_DIR)/bootloader.map; \
	else \
		echo "bootloader.mk not found"; \
	fi
	
flashtool:
ifeq ($(strip $(BUILD_ENV)), Linux)
	@cd $(TOPDIR)/utils/src-SPI_burn; \
	make EXTRA_CFLAGS+=-DLINUX EXTRA_CFLAGS+=-DFLASH_CTL_v2 EXTRA_CFLAGS+=-DSPI_BURN --makefile=Makefile_SPIburn_linux; \
	mkdir -p $(IMAGE_DIR); \
	cp -f SPI_burn $(IMAGE_DIR)/;
ifeq ($(strip $(HOST_ARCH)), x86_64)
	#make EXTRA_CFLAGS+=-DLINUX EXTRA_CFLAGS+=-DFLASH_CTL_v2 EXTRA_CFLAGS+=-DUSB_BURN --makefile=Makefile_USBburn_linux; \
	#cp -f USB_burn linux_x64/*.so $(IMAGE_DIR)/;
else
	#make EXTRA_CFLAGS+=-DLINUX EXTRA_CFLAGS+=-DFLASH_CTL_v2 EXTRA_CFLAGS+=-DUSB_BURN --makefile=Makefile_USBburn_linux; \
	#cp -f USB_burn linux_x86/*.so $(IMAGE_DIR)/;
endif
	echo "done"
endif

flashtool_clean:
ifeq ($(strip $(BUILD_ENV)), Linux)
	@cd $(TOPDIR)/utils/src-SPI_burn; \
	make EXTRA_CFLAGS+=-DLINUX EXTRA_CFLAGS+=-DFLASH_CTL_v2 EXTRA_CFLAGS+=-DSPI_BURN --makefile=Makefile_SPIburn_linux clean; \
	#make EXTRA_CFLAGS+=-DLINUX EXTRA_CFLAGS+=-DFLASH_CTL_v2 EXTRA_CFLAGS+=-DUSB_BURN --makefile=utils/src-SPI_burn/Makefile_USBburn_linux clean; \
	echo "clean done"
else
	@cd $(TOPDIR)/utils/src-SPI_burn; \
	make EXTRA_CFLAGS+=-DFLASH_CTL_v2 EXTRA_CFLAGS+=-DSPI_BURN --makefile=Makefile_SPIburn_win clean; \
	make EXTRA_CFLAGS+=-DFLASH_CTL_v2 EXTRA_CFLAGS+=-DUSB_BURN --makefile=Makefile_USBburn_win clean; \
	echo "clean done"
endif

	
.SECONDARY: $(TMP_SRC)
#
#_link: $(LDSCRIPT) $(SRC_DIRS)
#	@echo "# link ...\n" $(shell cat .build)
#	@$(CC) $(CFLAGS) $(LDFLAGS) $(shell cat .build) -Wl,--start-group $(LIB_OBJS) -Wl,--end-group \
#            -Xlinker -Map=$(IMAGE_NAME).map \
#            -o $(IMAGE_NAME).axf
#	@$(OBJCOPY) -O binary $(IMAGE_NAME).axf $(IMAGE_NAME).bin
#	@$(OBJDUMP) -d $(IMAGE_NAME).axf -S > $(IMAGE_NAME).asm
#	@$(SIZE)    -d $(IMAGE_NAME).axf
#
#$(LDSCRIPT): $(LDSCRIPT_S)
#	@echo "# build ld ...\n $<"
#	@$(CC) $(CFLAGS) -E -P $< -o $(LDSCRIPT)
#	@cat $(ICMD_SYMBOLS) >> $(LDSCRIPT)
#
#target_check:
#	@if [ "$(PROJECT)" = "" ]; then \
#	    echo "! no project specified"; \
#	    exit 0; \
#	fi
#	@$(RM) -rf .build
#	@echo "# make project $(PROJECT)"
#	@if ! [ -d $(IMAGE_DIR) ]; then mkdir $(IMAGE_DIR); fi
#
#clean: _clean
#	@echo "# clean ..."
#	@$(RM) $(LDSCRIPT)

#.DEFAULT_GOAL =

##############################################################
# Make rules
##############################################################
include $(BUILD)/rules.mk
include $(BUILD)/tool.mk

else
$(info Please use 3.82 or higher version)

all: utils/make
	@utils/make
	@echo "\033[0;31m"
	@echo Please update the make tool to 3.82
	@echo You can use this command
	@echo cd utils \&\& sudo make install
	@echo "\033[0m"

utils/make:
	@cd utils && third_party/make-3.82/configure
	@$(MAKE) -C utils
	@utils/make --version
	@echo "\033[0;31m"
	@echo Please update the make tool to 3.82
	@echo You can use this command
	@echo cd utils \&\& sudo make install
	@echo "\033[0m"

setup: utils/make
	@utils/make $(MAKECMDGOALS) p=$(p)
	@echo "\033[0;31m"
	@echo Please update the make tool to 3.82
	@echo You can use this command
	@echo cd utils \&\& sudo make install
	@echo "\033[0m"

clean: utils/make
	@utils/make $(MAKECMDGOALS)
	@echo "\033[0;31m"
	@echo Please update the make tool to 3.82
	@echo You can use this command
	@echo cd utils \&\& sudo make install
	@echo "\033[0m"

.DEFAULT_GOAL = all
#.PHONY: all

endif

