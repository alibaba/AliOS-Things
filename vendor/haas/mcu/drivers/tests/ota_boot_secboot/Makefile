cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))

obj-y += ../../utils/crc32/ ../../utils/hwtimer_list/ ../../utils/hexdump/ ../../utils/secure_boot/mincrypt/

ifneq ($(OTA_BARE_BOOT),1)
obj-y += ../../platform/main/startup_main.S
endif
obj-y += board_haas100.a mcu_haas1000.a ota_updater.a ota_2ndboot.a
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y +=  \
	-Iutils/boot_struct \

ifeq ($(SECURE_BOOT),1)
ccflags-y += -DSECURE_BOOT
endif

ifeq ($(HAAS_OTA_ENABLED),1)
ccflags-y += -DHAAS_OTA_ENABLED=1
endif

ccflags-y += -Iutils/crc32 -Iutils/hwtimer_list -Iutils/hexdump -Iutils/boot_struct -Iutils/secure_boot/mincrypt -Iplatform/drivers/norflash

CFLAGS_main.o += -DOTA_BOOT_SIZE=$(OTA_BOOT_SIZE) -DOTA_CODE_OFFSET=$(OTA_CODE_OFFSET) -DOTA_REMAP_OFFSET=$(OTA_REMAP_OFFSET)

ifeq ($(OTA_BARE_BOOT),1)
CFLAGS_main.o += -DOTA_BARE_BOOT
endif

ifneq ($(FLASH_SIZE),)
CFLAGS_main.o += -DFLASH_SIZE=$(FLASH_SIZE)
endif

ifneq ($(DEBUG_PORT),)
CFLAGS_main.o += -DDEBUG_PORT=$(DEBUG_PORT)
endif

