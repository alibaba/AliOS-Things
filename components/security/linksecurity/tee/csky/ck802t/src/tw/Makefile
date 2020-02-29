
TOP := .
CURRENT_BUILD := TW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

TW_DIRS := $(TOP)/platform/$(PLATFORM) $(TOP)/drv $(TOP)/mon_emu
OBJS := $(TOP)/platform/$(PLATFORM)/plat.o \
		$(TOP)/drv/drv.o \
		$(TOP)/mon_emu/mon_emu.o

ifeq ($(RSA_SUPPORT), Y)
    OBJS += $(TOP)/tee/crypto.rsa.o
endif
ifeq ($(AES_SUPPORT), Y)
    OBJS += $(TOP)/tee/crypto.aes.o
endif
ifeq ($(SHA_SUPPORT), Y)
    OBJS += $(TOP)/tee/crypto.sha.o
endif
ifeq ($(RAND_SUPPORT), Y)
    OBJS += $(TOP)/tee/crypto.rand.o
endif
ifeq ($(TRNG_SUPPORT), Y)
    OBJS += $(TOP)/tee/crypto.rand.o
endif

.PHONY: all clean

LDFLAGS += -L$(TOP)/platform/$(PLATFORM) -T$(TOP)/sw.lds

BIN_NAME := sw

all: $(BIN_NAME).bin

$(BIN_NAME).bin: $(TOP)/sw.lds
	for dir in $(TW_DIRS); do $(MAKE) -C $$dir; done
	@echo 'const char build_message[] = "version $(VERSION), build time $(BUILD_TIME)";' | \
         $(CC) -xc -c - -o $(TOP)/build_message.o $(CFLAGS)
	$(NOECHO)$(CC) $(LDFLAGS) -Wl,--start-group $(OBJS) $(TOP)/build_message.o $(TOP)/tee/tw.sdk.o -Wl,--end-group -o $(BIN_NAME).elf -lgcc
	$(NOECHO)$(OBJDUMP) -d $(BIN_NAME).elf > $(BIN_NAME).dump
	$(NOECHO)$(NM) -n $(BIN_NAME).elf | grep -v '\( [aUw] \)\|\(__crc_\)\|\( \$[adt]\)' > $(TOP)/system.map
	$(NOECHO)$(OBJCOPY) -O binary -R .note -R .comment $(BIN_NAME).elf $@

$(TOP)/sw.lds: $(TOP)/sw.lds.S
	$(NOECHO)$(CC) $(GEN_LDS_FLAGS) -I$(TOP)/inc -I$(TOP)/platform/$(PLATFORM)/inc -P -E -o $@ $<

clean:
	@echo "cleaning up ..."
	@for dir in $(TW_DIRS); do $(MAKE) -C $$dir clean; done
	$(NOECHO)rm -f $(BIN_NAME).bin $(BIN_NAME).dump $(BIN_NAME).elf $(TOP)/sw.lds $(TOP)/system.map $(TOP)/build_message.o
