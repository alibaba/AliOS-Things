cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S $(cur_dir)*.cpp))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)
obj-y := $(obj-y:.cpp=.o)

ccflags-y := -I$(obj)/../inc/ca

CFLAGS_mmu_ARMCA.o += -DFLASH_SIZE=$(FLASH_SIZE)
ifneq ($(PSRAM_SIZE),)
CFLAGS_mmu_ARMCA.o += -DPSRAM_SIZE=$(PSRAM_SIZE)
endif
ifneq ($(PSRAMUHS_SIZE),)
CFLAGS_mmu_ARMCA.o += -DPSRAMUHS_SIZE=$(PSRAMUHS_SIZE)
endif

ifeq ($(CHIP_HAS_PSRAMUHS),1)
CFLAGS_system_ARMCA.o += -DCHIP_HAS_PSRAMUHS
ifeq ($(PSRAMUHS_ENABLE),1)
CFLAGS_system_ARMCA.o += -DPSRAMUHS_ENABLE
endif
endif
