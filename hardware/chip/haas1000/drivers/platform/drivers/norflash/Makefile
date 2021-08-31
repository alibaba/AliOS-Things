cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libnorflash

ifneq ($(SDK),1)
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
$(LIB_NAME)-y := $(src_obj)
endif
obj-y :=
obj-y += $(LIB_NAME).a

ccflags-y := -I$(obj)/inc -Itests/programmer -Iutils/boot_struct

CFLAGS_norflash_drv.o += -DCHIP_FLASH_CTRL_VER=$(CHIP_FLASH_CTRL_VER)

ifeq ($(FLASH_HPM),1)
ccflags-y += -DFLASH_HPM
endif

ifeq ($(FLASH_SUSPEND),1)
CFLAGS_norflash_drv.o += -DFLASH_SUSPEND
endif
ifeq ($(FLASH_SECURITY_REGISTER),1)
CFLAGS_norflash_drv.o += -DFLASH_SECURITY_REGISTER
endif

ifeq ($(FLASH_PAGE_MPM_ENABLE),1)
ccflags-y += -DFLASH_PAGE_MPM_ENABLE
endif

ifeq ($(PSRAM_ENABLE),1)
CFLAGS_norflash_drv.o += -DPSRAM_ENABLE
endif
ifeq ($(PSRAMUHS_ENABLE),1)
CFLAGS_norflash_drv.o += -DPSRAMUHS_ENABLE
endif

ifeq ($(FLASH_CHIP),)
ifeq ($(PROGRAMMER),1)
FLASH_CHIP := ALL
else
FLASH_CHIP := GD25Q32C
endif
endif
ifeq ($(FLASH_CHIP),)
$(error *** Error: FLASH_CHIP is NOT defined!)
endif

CFLAGS_norflash_drv.o += $(foreach flash,$(FLASH_CHIP),-D__NORFLASH_$(flash)__)

