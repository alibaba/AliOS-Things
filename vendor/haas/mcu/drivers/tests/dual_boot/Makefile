cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)
obj-y += ../../utils/hwtimer_list/


ifeq ($(RTOS),1)
ccflags-y += -DRTOS
endif

ifneq ($(FLASH_FILL),)
ccflags-y += -DFLASH_FILL=$(FLASH_FILL)
endif

ccflags-y +=  \
    -Iapps/audioplayers \
    -Iapps/battery \
    -Iapps/common \
    -Iapps/main \
    -Iapps/factory \
    -Iapps/pwl \
    -Iapps/key \
    -Iutils/boot_struct \
    -Iservices/bt_app \
    -Iservices/nvrecord \
    $(BT_PROFILES_INCLUDES) \
    -Iservices/resources \
    -Iservices/overlay \
    -Iplatform/drivers/bt \
    -Iplatform\drivers\ana \
    -Iutils/hwtimer_list