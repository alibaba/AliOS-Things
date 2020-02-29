#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

TOP := ../../src/tw/
CURRENT_BUILD := NTW
include $(TOP)/tee.cfg
include $(TOP)/Make.rules

SRC += $(wildcard ./test/*.c)
SRC += $(wildcard ./*.c)
SRC += $(wildcard ./libs/libc/*.c) $(wildcard ./libs/pool/*.c) $(wildcard ./libs/sem/*.c)
# add ntw app src
SRC += $(wildcard ./ntw_app/*.c)
OBJS := $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(SRC)))

CFLAGS += -I./ntw_app/ -I$(TOP)/platform/$(PLATFORM)/inc
CFLAGS += -I. -I./inc
ASFLAGS += -I./inc
LDFLAGS += -L./os_lib/$(PLATFORM)/ -Tntw.lds

BIN_NAME := test_bed

PREBUILD := prebuild
DRV_DEP := ./ntw_drv/tee_ntw.o \
           ./ntw_drv/ntw.lpmapi.o \

ifeq ($(PLATFORM), zx297100)
	DRV_DEP += ./ntw_drv/tststub.o
endif

all: $(PREBUILD) ntw.lds $(OBJS) $(BIN_NAME).elf $(BIN_NAME).bin

$(BIN_NAME).elf: $(OBJS) os_lib/os_lib.o \
				 $(DRV_DEP) \
                 $(wildcard ./test/*.o)

	@echo "LD  $(BIN_NAME).elf"
	$(CC) $(LDFLAGS) $^ -o $(BIN_NAME).elf -lgcc

$(BIN_NAME).bin: $(BIN_NAME).elf
	@$(OBJDUMP) -d $(BIN_NAME).elf > $(BIN_NAME).dump
	@$(OBJDUMP) -h $(BIN_NAME).elf > $(BIN_NAME).section
	@$(NM) -n $(BIN_NAME).elf | grep -v '\( [aUw] \)\|\(__crc_\)\|\( \$[adt]\)' > system.map
	@$(OBJCOPY) -O binary -R .note -R .comment  $< $@

os_lib/os_lib.o:
	@$(MAKE) -C os_lib

$(PREBUILD):

ntw.lds: ntw.lds.S
	@$(CC) $(GEN_LDS_FLAGS) -I$(TOP)/inc -I$(TOP)/platform/$(PLATFORM)/inc -P -E -o $@ $<
clean:
	@echo "clean ..."
	@rm $(OBJS) $(BIN_NAME).* system.map ntw.lds -rf
	@$(MAKE) clean -C os_lib
