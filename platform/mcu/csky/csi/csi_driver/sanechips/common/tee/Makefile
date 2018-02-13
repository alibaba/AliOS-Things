#
# Makefile for TEE
#

TARGETS_ROOT_PATH ?=
TEEOS_LIB_PATH ?=
TEEOS_LIB ?=
TARGET_TEE ?= bin/$(TEEOS_LIB:lib%.a=%)

CC      = csky-abiv2-elf-gcc
OBJDUMP = csky-abiv2-elf-objdump
OBJCOPY = csky-abiv2-elf-objcopy

ASFLAGS := -c -Os -mcpu=ck802t -Wa,--gdwarf2 -Wa,-melrw
LDFLAGS := -mcpu=ck802t -nostartfiles -Wl,--gc-sections -fno-builtin

TEE_LD := tee.ld

.PHONY: clean

all: $(TARGET_TEE)

$(TARGET_TEE) : $(TEE_LD) $(TEEOS_LIB_PATH)/$(TEEOS_LIB)
	@echo LINK $@
	@mkdir -p bin
	@$(CC) $(LDFLAGS) -Wl,-ckmap='$@.map' -T$(TEE_LD) -o $@ -Wl,--whole-archive $(TEEOS_LIB_PATH)/$(TEEOS_LIB) -Wl,--no-whole-archive
	$(OBJDUMP) -d $@  > $@.dump
	$(OBJCOPY) -O binary -R .note -R .comment $@ $@.bin
	$(OBJCOPY) -O ihex $@ $@.hex

$(TEE_LD) : $(TEE_LD).S
	@echo CC $@ $(TEEOS_LIB_PATH)
	@$(CC) -I$(TEEOS_LIB_PATH) -I$(TARGETS_ROOT_PATH)/include -P -E -o $@ $<

clean:
	@-rm -fr bin $(TEE_LD)

