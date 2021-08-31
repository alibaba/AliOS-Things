
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
    -I$(obj)/../../nv_section/fpga_section \
    -I$(obj)/../../nv_section/include \
    -I$(obj)/../platform/cmsis/inc \
    -Iutils/crc32 \
    $(BT_IF_INCLUDES) \
    -Iutils/hexdump \
    -Iapps/key \
    -Iservices/bt_app \
    -Iplatform/drivers/ana \
    -Iservices/nv_section/customparam_section \
    -Iservices/ble_stack/common/api \
    -Iservices/ble_stack/ble_ip \
    -Iutils/heap \
    -Iservices/nvrecord \
    -Iservices/norflash_api \
    -Iservices/nv_setion/userdata_section \
    -Iplatform/drivers/norflash \
    -Iplatform/hal \
    -Iplatform/cmsis/inc

ccflags-y += -DAUDIO_OUTPUT_VOLUME_DEFAULT=$(AUDIO_OUTPUT_VOLUME_DEFAULT)

ifeq ($(FLASH_SUSPEND),1)
ccflags-y += -DFLASH_SUSPEND
endif

ifeq ($(TX_IQ_CAL),1)
subdir-ccflags-y += -DTX_IQ_CAL
endif

